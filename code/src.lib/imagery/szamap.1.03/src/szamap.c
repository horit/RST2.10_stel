/* szamap.c
   ========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Software Toolkit - SuperDARN Toolkit" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "rtime.h"
#include "aacgm.h"
#include "polygon.h"
#include "raster.h"
#include "contour.h"
#include "sza.h"

/*
 $Log: szamap.c,v $
 Revision 1.3  2004/05/04 18:14:14  barnes
 Removed redundant header.

 Revision 1.2  2004/04/26 21:28:07  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/24 18:07:30  barnes
 Initial revision

 Revision 1.1  2004/03/09 19:50:34  barnes
 Initial revision

*/


int SZATransform(int ssze,void *src,int dsze,void *dst,void *data) {
  float *pnt;
  float lat=0,lon=0;
  float step=((float *) data)[0];
  float flg=((float *) data)[1];

  pnt=(float *) src;
  
  lon=step*pnt[0]*360.0-180;
  if (flg==0) lat=-90+step*pnt[1]*180;
  if (flg==1) lat=step*pnt[1]*90.0;
  if (flg==-1) lat=-90+step*pnt[1]*90.0;
  pnt=(float *) dst;
  pnt[0]=lat;
  pnt[1]=lon;
  return 0;
}

struct PolygonData **SZAContour(int yr,int mo,int dy,int hr,int mt,int sc,
                                 int flg,int mode,float step,int znum,
                                double *zenith) {
  int s;
  float arg[2];
  int c=0;
  int yrsec;
  double *zbuffer;
  double mlon,dec;
  double LsoT; 
  double Hangle;
  double Z,eqt;
  double lat,lon;
  double tlon,tlat,r;

  struct PolygonData **ctr=NULL;

 
  /* if mode !=0 then we are working in geomagnetic co-ordinates
   * and must transform to geographic when calculating zenith angle */

  arg[0]=step;
  arg[1]=flg;

  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
  SZASolarLoc(yr,yrsec,&mlon,&dec);
  eqt=SZAEqOfTime(mlon,yr);
  
  zbuffer=malloc(sizeof(double)*360*182);
 
  c=0; 
  if (flg==0) {
    for (lat=-90;lat<=90;lat+=step) {
      for (lon=-180;lon<180;lon+=step) {

        if (mode==0) {
           tlat=lat;
           tlon=lon;
        } else s=AACGMConvert(lat,lon,0,&tlat,&tlon,&r,1);
        
        LsoT=(hr*3600+mt*60)+(tlon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(tlon,tlat,dec,Hangle);
        zbuffer[c]=Z;
        c++;     
      }
    }
  } else if (flg==1) {
    for (lon=-180;lon<180;lon+=step) {
      zbuffer[c]=0;
      c++;
    }
    for (lat=step;lat<=90;lat+=step) {
      for (lon=-180;lon<180;lon+=step) {

        if (mode==0) {
           tlat=lat;
           tlon=lon;
        } else s=AACGMConvert(lat,lon,0,&tlat,&tlon,&r,1);
        
        LsoT=(hr*3600+mt*60)+(tlon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(tlon,tlat,dec,Hangle);
        zbuffer[c]=Z;
        c++;     
      }
    }
  } else {
    for (lat=-90;lat<0;lat+=step) {
      for (lon=-180;lon<180;lon+=step) {

        if (mode==0) {
           tlat=lat;
           tlon=lon;
        } else s=AACGMConvert(lat,lon,0,&tlat,&tlon,&r,1);
        
        LsoT=(hr*3600+mt*60)+(tlon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(tlon,tlat,dec,Hangle);
        zbuffer[c]=Z;
        c++;     
      }
    }
    for (lon=-180;lon<180;lon+=step) {
      zbuffer[c]=0;
      c++;
    }
  }
  /* okay now generate contours */
  if (flg==0) ctr=Contour(zbuffer,contour_DOUBLE,360,181,znum,
              zenith,3,0,1,0x00);
  else ctr=Contour(zbuffer,contour_DOUBLE,360,91,znum,
              zenith,3,0,1,0x00);

  /* transform them so that they are stored in longitude and latitudes */

  free(zbuffer);

  ContourModify(znum,ctr,SZATransform,arg);
  return ctr;
}

struct PolygonData *SZATerminator(int yr,int mo,int dy,int hr,int mt,int sc,
                                    int flg,int mode,float step,
                                    float zenith) {

  double cval[1]={102};
  struct PolygonData **ctr;
  struct PolygonData *trm;
  if (zenith !=0) cval[0]=zenith;
  ctr=SZAContour(yr,mo,dy,hr,mt,sc,flg,mode,step,1,cval);
  trm=ctr[0];
  free(ctr);
  return trm;
}


float *SZAMap(int yr,int mo,int dy,int hr,int mt,int sc,float latmin,
               int wdt,int hgt,int mode,
	       int (*trf)(int ssze,void *src,int dsze, void *dst,void *data),
                 void *data) {

  float lat,lon;
  float xstep=1.0;
  float ystep=1.0;
  int xnum,ynum;

  int x,y;
  int s,i,j;
  float *pnt=NULL;
  float *zbuf=NULL;
  int *vertex=NULL;
  int *mapping=NULL;
  float *image=NULL;
  int num=0,tnum,poly=0,tpoly;
  float zeroval=-400;

  int yrsec;
  double LsoT;
  double Hangle;
  double Z,eqt;
  double dec,mlon;

  double tlon,tlat,r;


  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
  SZASolarLoc(yr,yrsec,&mlon,&dec);
  eqt=SZAEqOfTime(mlon,yr);

  xnum=(360/xstep)+1;
  ynum=180.0/ystep+1;

  zbuf=malloc(sizeof(float)*xnum*ynum);
  pnt=malloc(sizeof(float)*xnum*ynum*2);
  vertex=malloc(sizeof(int)*xnum*ynum*4);
  mapping=malloc(sizeof(int)*xnum*ynum);

  for (y=0;y<ynum;y++) {
    for (x=0;x<xnum;x++) {
      lon=x*xstep;
      lat=-90+y*ystep;


      if (mode==0) {
	tlat=lat;
	tlon=lon;
      } else {
	s=AACGMConvert(lat,lon,0,&tlat,&tlon,&r,1);
      }

      LsoT=(hr*3600+mt*60+sc)+(tlon*4*60)+eqt;
      Hangle=15*((LsoT/3600)-12);
      Z=SZAAngle(tlon,tlat,dec,Hangle);
      
      if ((y !=ynum-1) && (x !=xnum-1)) {
	vertex[4*poly]=num;
	vertex[4*poly+1]=num+1;
	vertex[4*poly+2]=vertex[4*poly+1]+xnum;
	vertex[4*poly+3]=num+xnum;
	poly++;
      }

      zbuf[num]=Z;
      pnt[2*num]=lat;
      pnt[2*num+1]=lon;
      num++;
    }
  }
 
  tnum=0;
  for (i=0;i<num;i++) {
    mapping[i]=-1;
    s=(*trf)(2*sizeof(float),&pnt[2*i],2*sizeof(float),
              &pnt[2*tnum],data);
    if (s !=0) continue;
    mapping[i]=tnum;
    zbuf[tnum]=zbuf[i];
    pnt[2*tnum]=pnt[2*tnum]*wdt;
    pnt[2*tnum+1]=pnt[2*tnum+1]*hgt;
    tnum++;
  }

  tpoly=0;
  for (i=0;i<poly;i++) {
    for (j=0;j<4;j++) {
      if (mapping[vertex[4*i+j]]==-1) break;
      vertex[4*tpoly+j]=mapping[vertex[4*i+j]];
    }
    if (j<4) continue;
    tpoly++;
  }

  
  image=Raster(wdt,hgt,0,raster_FLOAT,&zeroval,tpoly,pnt,vertex,
               zbuf);
  
  free(pnt);
  free(vertex);
  free(zbuf);
  free(mapping);
  return image;

}



