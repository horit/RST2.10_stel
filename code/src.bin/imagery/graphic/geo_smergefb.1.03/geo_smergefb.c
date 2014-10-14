/* geo_smergefb.c
   ============== 
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
#include <sys/types.h>

#include "rtypes.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"

#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"
#include "simage.h"
#include "sza.h"
#include "image_table.h"

#include "clip.h"
#include "key.h"
#include "map.h"

#include "hlpstr.h"
#include "errstr.h"


/*
 $Log: geo_smergefb.c,v $
 Revision 1.5  2005/08/11 14:36:26  barnes
 Added option to dump command line options.

 Revision 1.4  2004/05/05 16:32:29  barnes
 Added missing header.

 Revision 1.3  2004/05/04 20:07:41  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 20:27:09  barnes
 Audit - Enforced warning set to "all".

*/


struct imgtable *imgtable;

unsigned char gry[256];

struct key key;

struct OptionData opt;
struct OptionFile *optf=NULL;

unsigned int iflgA=0;
struct SImage simageA;
unsigned int iflgB=0;
struct SImage simageB;

struct FrameBufferClip *fbclp=NULL;

float *imageA;
float *imageB;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int AACGMtransform(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  int s;
  double mlon,mlat;
  double glon,glat,r;

  pnt=(float *)src;

  if (data==NULL) {
    glat=pnt[0];
    glon=pnt[1];
    s=AACGMConvert(glat,glon,300,&mlat,&mlon,&r,0);
    pnt=(float *)dst;
    pnt[0]=mlat;
    pnt[1]=mlon;
  } else {
    mlat=pnt[0];
    mlon=pnt[1];
    s=AACGMConvert(mlat,mlon,300,&glat,&glon,&r,1);
    pnt=(float *)dst;
    pnt[0]=glat;
    pnt[1]=glon;
  }
  return s;

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

int main(int argc,char *argv[]) {

  int s=0;

  int finx;

  char *name=NULL;
  char *dname={"simage"};
  struct FrameBuffer *img=NULL; 
  
  int arg;

  char *cfname=NULL;
  FILE *fp;


  int wdt=540,hgt=540;
 
  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char help=0; 
  unsigned char option=0; 

  char *key_fname=NULL;
 
  int inx=0;
  int num=0;
  float min=1e6,max=-1e6;
  unsigned char sflg=0;
  unsigned char lflg=0; 

  unsigned char sqflg=0;

  MapTransform  tfunc;

  unsigned char flip=0;
  unsigned char gvp=0;
  unsigned char ortho=0;
  unsigned char stereo=0;
  unsigned char cylind=0;
  float lat=1e10,lon=0;
  float latmin=50.0;
  float sf=1.0;

  unsigned char lstflg=0;

  unsigned char magflg=0;
  unsigned char rotflg=0;

  float marg[4];
  int i;
  int x,y;
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;
  unsigned char *a;
  float ratio;
  float tf=1.0;
  float c;
  int ci;



  char *tmetxt=NULL;
  char *dtetxt=NULL;

  double tval=-1;
  double dval=-1;

  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  float tme_shft;

  for (i=0;i<256;i++) gry[i]=i;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);

  OptionAdd(&opt,"xml",'x',&xmlflg);
 
  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);

  OptionAdd(&opt,"log",'x',&lflg);
  OptionAdd(&opt,"stretch",'x',&sflg);

  OptionAdd(&opt,"inx",'i',&inx);

  OptionAdd(&opt,"min",'f',&min);
  OptionAdd(&opt,"max",'f',&max);

  OptionAdd(&opt,"tf",'f',&tf);

  OptionAdd(&opt,"key",'t',&key_fname);

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
 
  OptionAdd(&opt,"name",'t',&name);
 
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

 
  fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  imgtable=read_imgtable(fp);
  fclose(fp);

  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  if (tval==-1) tval= ( (int) imgtable->img[0].st_time % (24*3600));
  if (dval==-1) tval+=imgtable->img[0].st_time -
		  ( (int) imgtable->img[0].st_time  % (24*3600));
  else tval+=dval;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  finx=find_image(imgtable,tval);

  if (finx==0) {
    fp=fopen(imgtable->img[0].fname,"r");
    if (fp==NULL) s=-1;
    else s=SImageFread(fp,&simageA);
    if (s==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
    iflgA=1;
    fclose(fp);
  } else {
    fp=fopen(imgtable->img[finx-1].fname,"r");
    if (fp==NULL) s=-1;
    else s=SImageFread(fp,&simageA);
    if (s==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
    iflgA=1;
    fclose(fp);
  }
  if (finx<imgtable->num) {
    fp=fopen(imgtable->img[finx].fname,"r");
    if (fp==NULL) s=-1;
    s=SImageFread(fp,&simageB);
    if (s==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
    iflgB=1;
    fclose(fp);
  }

  num=simageA.colors;

  if (inx<0) inx=0;
  if (inx>=num) inx=num-1;


  if (lat>90) {
    if (simageA.c.glat<0) lat=-90;
    else lat=90;
  }

  if ((ortho==0) && (stereo==0) && (gvp==0)) cylind=1;


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

  if (magflg) tme_shft=-AACGMConvertMLT(yr,yrsec,0.0)*15.0; 
    else {
      double mlon,dec,eqt,LsoT,LT,Hangle;
      if (lstflg) {
        SZASolarLoc(yr,yrsec,&mlon,&dec);
        eqt=SZAEqOfTime(mlon,yr);
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

  if ((min==1e6) && (simageA.min !=NULL)) min=simageA.min[inx];
  if ((max==-1e6)&& (simageA.max !=NULL)) max=simageA.max[inx];
  if ((lflg) && (min<=0)) min=1;
  if (lflg) {
    max=log10(max);
    min=(int) log10(min)+max-(int) max;
  }

  if ((ppmflg==0) && (xmlflg==0) && (ppmxflg==0)) xmlflg=1;

  if (key_fname !=NULL) {
    fp=fopen(key_fname,"r");
    if (fp !=NULL) {
      load_key(fp,&key);
      fclose(fp);
    }
  }

  if (key.num==0) {
    key.num=255;
    key.r=gry;
    key.g=gry;
    key.b=gry;
  }

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }

  imageA=SImageMap(wdt,hgt,magflg,tfunc,
                             marg,&simageA,inx);

  if (iflgB) imageB=SImageMap(wdt,hgt,magflg,tfunc,
                                marg,&simageB,inx);
 
 
  if ((cylind) || (sqflg)) fbclp=NULL;
  else fbclp=fbclip(wdt,hgt); 

  if (name==NULL) name=dname;

  img=FrameBufferMake(name,wdt,hgt,24);  
  FrameBufferClear(img,0,0x0f);
  r=img->img;
  g=img->img+img->wdt*img->hgt;
  b=img->img+img->wdt*img->hgt*2;
  a=img->msk;  
  
  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
      if ((fbclp !=NULL) && (fbclp->clp[y*wdt+x]==0)) continue;
      if (iflgB) {
        float ca,cb;
        float mr;
        if ((lflg) && (imageA[y*wdt+x]>0)) ca=log10(imageA[y*wdt+x]);
        else ca=imageA[y*wdt+x];
        if ((lflg) && (imageB[y*wdt+x]>0)) cb=log10(imageB[y*wdt+x]);
        else cb=imageB[y*wdt+x];
        mr=(tval-simageA.st_time)/(simageB.st_time-simageA.st_time);
        if (mr<0) mr=0;
        if (mr>1) mr=1;
        c=ca*(1-mr)+cb*mr;
      } else {
        if ((lflg) && (imageA[y*wdt+x]>0)) c=log10(imageA[y*wdt+x]);
        else c=imageA[y*wdt+x];
      }
      if (c<min) continue;
      if (c>max) c=max;
      if ((sflg) || (lflg)) c=(c-min)/(max-min);
      else c=c/max;

      if (c<0.0) c=0.0;
      if (c>1.0) c=1.0;

      ci=(key.num-1)*c;
      ratio=(1-tf)+tf*c;

      if (key.r !=NULL) r[y*wdt+x]=key.r[ci];
      if (key.g !=NULL) g[y*wdt+x]=key.g[ci];
      if (key.b !=NULL) b[y*wdt+x]=key.b[ci]; 
      a[y*wdt+x]=ratio*255;
      
    }
  }
  
  if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0;  

  
}  


