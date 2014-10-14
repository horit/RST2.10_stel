/* geomap.c
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
#include <math.h>
#include "raster.h"
#include "aacgm.h"

/*
 $Log: geomap.c,v $
 Revision 1.1  2004/05/14 19:54:29  barnes
 Initial revision

*/

void GeoMap(int wdt,int hgt,
                 int (*trf)(int ssze,void *src,int dsze, 
                 void *dst,void *data),
		 void *data,float **latptr,float **lonptr) {

  float lat,lon;

  float xstep=1;
  float ystep=1;
  int x,y;
  int s,i,j;

  int xnum,ynum;
  float *pnt;
  int *vertex;
  float *latdata;
  float *londata;
  int *mapping;
 
  float *latbuf;
  float *lonbuf;

  float zeroval=-400.0;
  int num=0,tnum,poly=0,tpoly;
  
  xnum=360/xstep+1;
  ynum=180.0/ystep;
  
  latdata=malloc(sizeof(float)*xnum*ynum);
  londata=malloc(sizeof(float)*xnum*ynum);

  pnt=malloc(sizeof(float)*xnum*ynum*2);
  vertex=malloc(sizeof(int)*xnum*ynum*4);

  mapping=malloc(sizeof(int)*xnum*ynum);

  for (y=0;y<ynum;y++) {
     for (x=0;x<xnum;x++) {
       lon=x*xstep;
       lat=-90+(y*ystep);
                 
       if ((y !=ynum-1) && (x !=xnum-1)) {
         vertex[4*poly]=num;
         vertex[4*poly+1]=num+1;
         vertex[4*poly+2]=vertex[4*poly+1]+xnum;
         vertex[4*poly+3]=num+xnum;     
         poly++;  
       } 

       latdata[num]=lat;
       londata[num]=lon;
      
       pnt[2*num]=lat;
       pnt[2*num+1]=lon;

       num++;
     }
  }
  
  
  /* now do the transform remembering to strip off bad points */
  
  tnum=0;
  for (i=0;i<num;i++) {
      mapping[i]=-1;
      s=(*trf)(2*sizeof(float),&pnt[2*i],2*sizeof(float),
              &pnt[2*tnum],data);
      if (s !=0) continue;
      mapping[i]=tnum;
      latdata[tnum]=latdata[i];
      londata[tnum]=londata[i];
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
  latbuf=Raster(wdt,hgt,1,raster_FLOAT,&zeroval,tpoly,pnt,vertex,
              latdata);
  lonbuf=Raster(wdt,hgt,1,raster_FLOAT,&zeroval,tpoly,pnt,vertex,
              londata);
  free(vertex);
  free(pnt);
  free(latdata);
  free(londata);
  free(mapping);

  *latptr=latbuf;
  *lonptr=lonbuf;
}
