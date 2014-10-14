/* geo_bumpmap.c
   ============= 
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
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "aacgm.h"
#include "rconvert.h"
#include "rfbuffer.h"
#include "raster.h"
#include "rplot.h"
#include "rtime.h"
#include "rmap.h"
#include "sza.h"
#include "eldata.h"
#include "gmap.h"
#include "phong.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: geo_bumpmap.c,v $
 Revision 1.8  2005/08/11 14:23:55  barnes
 Added option to dump the command line options.

 Revision 1.7  2004/08/25 13:42:13  barnes
 Added greater control over the lighting parameters.

 Revision 1.6  2004/06/22 17:51:30  barnes
 Changed references of Long to Int.

 Revision 1.5  2004/06/21 22:12:17  barnes
 Added missing headers.

 Revision 1.4  2004/05/17 21:59:41  barnes
 Removed debugging code.

 Revision 1.3  2004/05/17 21:58:22  barnes
 Fixed problems with magnetic coordinates.

 Revision 1.2  2004/05/14 19:49:20  barnes
 Changed header names.

 Revision 1.1  2003/09/29 15:19:47  barnes
 Initial revision

*/

struct OptionData opt;
struct OptionFile *optf=NULL;

struct FrameBufferClip *fbclp=NULL;

struct ElevationData eldata;

float *ilat=NULL;
float *ilon=NULL;

float *dbuf,*sbuf;
float *zbuf;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

double strtime(char *text) {
  int hr,mn;
  double sc;
  int i,j;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  for (j=i+1;(text[j] !=':') && (text[j] !=0);j++);
  if (text[j]==0) { 
    mn=atoi(text+i+1);
    return (double) hr*3600L+mn*60L;
  }
  text[j]=0;
  mn=atoi(text+i+1);
  sc=atof(text+j+1);
  return (double) hr*3600L+mn*60L+sc;
}  


struct FrameBufferClip *fbclip(int wdt,int hgt) {

  int i;
  float sf=0.55;
  int px[12];
  int py[12];
  int t[12];

  px[0]=0.5*wdt;
  py[0]=0;
  px[1]=0.5*wdt;
  py[1]=0.5*hgt*sf;
  px[2]=0.5*wdt*sf;
  py[2]=0.5*hgt;
  px[3]=0;
  py[3]=0.5*hgt;
  px[4]=-0.5*wdt*sf;
  py[4]=0.5*hgt;
  px[5]=-0.5*wdt;
  py[5]=0.5*hgt*sf;
  px[6]=-0.5*wdt;
  py[6]=0;
  px[7]=-0.5*wdt;
  py[7]=-0.5*hgt*sf;
  px[8]=-0.5*wdt*sf;
  py[8]=-0.5*hgt;
  px[9]=0;
  py[9]=-0.5*hgt;
  px[10]=0.5*wdt*sf;
  py[10]=-0.5*hgt;
  px[11]=0.5*wdt;
  py[11]=-0.5*hgt*sf;

  for (i=0;i<12;i++) {
    t[i]=1;
    px[i]+=0.5*wdt;
    py[i]+=0.5*hgt;
  }
  return FrameBufferMakeClip(wdt,hgt,12,px,py,t);
}



int main(int argc,char *argv[]) {

  int s=0;

  int arg;
  
  char *cfname=NULL;
  FILE *fp;
  

  int wdt=540,hgt=540;

  unsigned char help=0; 
  unsigned char option=0; 


  MapTransform  tfunc;
  unsigned char flip=0;
  unsigned char gvp=0;
  unsigned char ortho=0;
  unsigned char stereo=0;
  unsigned char cylind=0;
  float lat=90,lon=0;
  float latmin=50.0;
  float sf=1.0;

  unsigned char sqflg=0;


  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char lstflg=0;
  unsigned char smflg=0;

  float marg[4];


  int x,y;

  float e;

  char *tmetxt=NULL;
  char *dtetxt=NULL;

  double tval=-1;
  double dval=-1;

  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  float tme_shft;
 
  float glat,glon;
  float fx,fy;
  int px,py;

  float ka=0.8;
  float kd=0.3;
  float ks=0.4;
  float sn=4;

  double LsoT,LT,Hangle,dec,eqt,mlon;

  float minlat,minlon,maxlat,maxlon;

  float Lvec[3]={-1.0,0.0,0.5}; /* lighting vector */
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);

  OptionAdd(&opt,"square",'x',&sqflg);


  OptionAdd(&opt,"ortho",'x',&ortho);
  OptionAdd(&opt,"stereo",'x',&stereo);
  OptionAdd(&opt,"gvp",'x',&gvp);

  OptionAdd(&opt,"lat",'f',&lat);
  OptionAdd(&opt,"lon",'f',&lon);
  OptionAdd(&opt,"latmin",'f',&latmin);
  OptionAdd(&opt,"sf",'f',&sf);
  OptionAdd(&opt,"mag",'x',&magflg);
  OptionAdd(&opt,"rotate",'x',&rotflg);
  OptionAdd(&opt,"flip",'x',&flip);

  OptionAdd(&opt,"lst",'x',&lstflg);
 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"smooth",'x',&smflg);

  OptionAdd(&opt,"ka",'f',&ka); 
  OptionAdd(&opt,"kd",'f',&kd);
  OptionAdd(&opt,"ks",'f',&ks);
  OptionAdd(&opt,"n",'f',&sn);
 


  arg=OptionProcess(1,argc,argv,&opt,NULL);  

  if (cfname !=NULL) { /* load the configuration file */
    int farg;
    do {
      fp=fopen(cfname,"r");
      if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        farg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
        OptionFreeFile(optf);
       }   
       fclose(fp);
    } while (cfname !=NULL);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  
  if (argc==arg) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  s=ElevationLoad(fp,&eldata);
  fclose(fp);

  if (s !=0) {
    fprintf(stderr,"Error loading elevation data.\n");
    exit(-1);
  }
  
  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) {
    dval=strdate(dtetxt);
    tval+=dval;
  }
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  if ((ortho==0) && (stereo==0) && (gvp==0)) cylind=1;

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  tfunc=MapCylindrical;
  if (ortho) tfunc=MapOrthographic;
  if (stereo) tfunc=MapStereographic;
  if (gvp) tfunc=MapGeneralVerticalPerspective;


  marg[0]=lat;
  marg[1]=lon;
  
  if ((ortho) || (gvp)) marg[2]=sf;
  else if (stereo) marg[2]=1.25*0.5*sf*90.0/(90-fabs(latmin));
  else marg[2]=1;
  marg[3]=flip;

  SZASolarLoc(yr,yrsec,&mlon,&dec);
  eqt=SZAEqOfTime(mlon,yr);

  if (magflg) tme_shft=-AACGMConvertMLT(yr,yrsec,0.0)*15.0; 
    else {
      if (lstflg) {
        LsoT=(hr*3600+mt*60+sc)+eqt;
        Hangle=15*(LsoT/3600);
        tme_shft=-Hangle;
      } else {
        LT=(hr*3600+mt*60+sc);
        Hangle=15*(LT/3600);
        tme_shft=-Hangle;
      }
    }

  if (rotflg)  marg[1]=lon+tme_shft;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }
      
  if (cylind) {
    marg[1]=0;
    marg[2]=0;
  }
 
  GeoMap(wdt,hgt,tfunc,marg,&ilat,&ilon);
  
  zbuf=malloc(sizeof(float)*wdt*hgt);

  minlat=eldata.lat+eldata.latsec/3600.0;
  minlon=eldata.lon+eldata.lonsec/3600.0;
  maxlat=minlat+eldata.numlat*eldata.latstp/36000.0;
  maxlon=minlon+eldata.numlon*eldata.lonstp/36000.0;
 
  if ((cylind) || (sqflg)) fbclp=NULL;
  else fbclp=fbclip(wdt,hgt);

  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
      zbuf[y*wdt+x]=-1e6;
      if ((fbclp !=NULL) && (fbclp->clp[y*wdt+x]==0)) continue;
      if ((ilat[y*wdt+x]<-90.0) || (ilat[y*wdt+x]>90)) continue;
      glat=ilat[y*wdt+x];
      glon=ilon[y*wdt+x];

      if (magflg) {
	double mlat,mlon,rho;
        s=AACGMConvert(glat,glon,300.0,&mlat,&mlon,&rho,1);
        glat=mlat;
        glon=mlon;
      }

      if (cylind) {
        if (rotflg) glon+=lon+tme_shft;
        else glon+=lon;
      }
      if (glon>180) glon-=360;

      if (glat<minlat) continue;
      if (glon<minlon) continue;
      if (glat>=maxlat) continue;
      if (glon>=maxlon) continue;

      fy=(glat-minlat)/(maxlat-minlat);
      fx=(glon-minlon)/(maxlon-minlon);
     
      px=eldata.numlon*fx;
      py=eldata.numlat*fy;
 
      if (smflg) {    
        float l,r,b,t;
        l=eldata.data[px*eldata.numlat+py];
        if (px<eldata.numlon-1) r=eldata.data[(px+1)*eldata.numlat+py];
        else r=l;
        b=l+(r-l)*(eldata.numlon*fx-px);
        if (py<eldata.numlat-1) {
          l=eldata.data[px*eldata.numlat+py+1];
          if (px<eldata.numlon-1) r=eldata.data[(px+1)*eldata.numlat+py+1];
          else r=l;
	  t=l+(r-l)*(eldata.numlon*fx-px);
	} else t=b;
        e=b+(t-b)*(eldata.numlat*fy-py);
      } else e=eldata.data[px*eldata.numlat+py];
      zbuf[y*wdt+x]=e;
    }
  }
  PhongModel(wdt,hgt,0,zbuf,Lvec,kd,ks,sn,&dbuf,&sbuf); 
  ConvertFwriteInt(stdout,wdt);
  ConvertFwriteInt(stdout,hgt);
  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      ConvertFwriteFloat(stdout,zbuf[y*wdt+x]);
    }
  }
  
  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      ConvertFwriteFloat(stdout,ka+dbuf[y*wdt+x]);
    }
  }

  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      ConvertFwriteFloat(stdout,sbuf[y*wdt+x]);
    }
  }

  return 0;
}  


