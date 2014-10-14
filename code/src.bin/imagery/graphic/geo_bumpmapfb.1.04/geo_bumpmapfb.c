/* geo_bumpmapfb.c
   =============== 
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

#include "rfbuffer.h"
#include "fontdb.h"
#include "rplot.h"

#include "raster.h"

#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"
#include "sza.h"
#include "eldata.h"
#include "phong.h"
#include "gmap.h"
#include "key.h"
#include "clip.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: geo_bumpmapfb.c,v $
 Revision 1.4  2005/08/11 14:27:48  barnes
 Added option to dump the command line options.

 Revision 1.3  2004/06/21 22:13:11  barnes
 Added missing headers.

 Revision 1.2  2004/05/14 20:14:19  barnes
 Modifications to deal with changes to libraries.

 Revision 1.1  2003/09/29 15:21:27  barnes
 Initial revision

*/

unsigned char gry[256];
struct key key;

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



int main(int argc,char *argv[]) {

  int s=0;

  char *name=NULL;
  char *dname={"bump"};

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
 

  unsigned char sqflg=0;

  MapTransform  tfunc;
  unsigned char flip=0;
  unsigned char gvp=0;
  unsigned char ortho=0;
  unsigned char stereo=0;
  unsigned char cylind=0;
  float lat=90,lon=0;
  float latmin=50.0;
  float sf=1.0;

  char *key_fname=NULL;

  float min=1e6,max=-1e6;
  unsigned char sflg=0;
  unsigned char lflg=0;

  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char lstflg=0;

  unsigned char smflg=0;
  unsigned char phflg=0;

  float bc=0.8;
  float ds=0.2;
  float ss=1.0;

  float marg[4];
  int i;

  int x,y;
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;
  unsigned char *a;

  float e;
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
 
  float glat,glon;
  float fx,fy;
  int px,py;

  double LsoT,LT,Hangle,dec,eqt,mlon;

  float J,K;
  int rv,gv,bv;
  float minlat,minlon,maxlat,maxlon;

  float Lvec[3]={-1.0,0.0,0.5}; /* lighting vector */
 
  for (i=0;i<255;i++) gry[i]=i;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

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

  OptionAdd(&opt,"key",'t',&key_fname);

  OptionAdd(&opt,"min",'f',&min);
  OptionAdd(&opt,"max",'f',&max);

  OptionAdd(&opt,"log",'x',&lflg);
  OptionAdd(&opt,"stretch",'x',&sflg);

  OptionAdd(&opt,"lst",'x',&lstflg);
 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"name",'t',&name);

  OptionAdd(&opt,"smooth",'x',&smflg);
  OptionAdd(&opt,"phong",'x',&phflg);

  OptionAdd(&opt,"bc",'f',&bc);
  OptionAdd(&opt,"ds",'f',&ds);
  OptionAdd(&opt,"ss",'f',&ss);

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

  if (min==1e6) min=eldata.min;
  if (max==-1e6) max=eldata.max;
 


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
    key.r=gry;
    key.g=gry; 
    key.b=gry;
    key.num=255;
  } 


  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }
    
  if (name==NULL) name=dname;
  
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
 
  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
      zbuf[y*wdt+x]=-1e6;
      if ((ilat[y*wdt+x]<-90.0) || (ilat[y*wdt+x]>90)) continue;
      glat=ilat[y*wdt+x];
      glon=ilon[y*wdt+x];
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

 if (phflg) PhongModel(wdt,hgt,0,zbuf,Lvec,0.4,0.3,4,&dbuf,&sbuf);
  
  if (cylind) fbclp=NULL;
  else if (! sqflg) fbclp=fbclip(wdt,hgt);

  img=FrameBufferMake(name,wdt,hgt,24);
  FrameBufferClear(img,0,0x0f);
  r=img->img;
  g=img->img+img->wdt*img->hgt;
  b=img->img+img->wdt*img->hgt*2;
  a=img->msk;
  

  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
      if ((fbclp !=NULL) && (fbclp->clp[y*wdt+x]==0)) continue;
      e=zbuf[y*wdt+x];

      if ((lflg) && (e>0)) c=log10(e);
      else c=e;
      if (c<0) continue;
      if (c<min) c=min;
      if (c>max) c=max;
      if ((sflg) || (lflg)) c=(c-min)/(max-min);
      else c=c/max;

      if (c<0.0) c=0.0;
      if (c>1.0) c=1.0;

      ci=(key.num-1)*c;

      if (key.r !=NULL) r[y*wdt+x]=key.r[ci];
      if (key.g !=NULL) g[y*wdt+x]=key.g[ci];
      if (key.b !=NULL) b[y*wdt+x]=key.b[ci];
      a[y*wdt+x]=255.0;

      if (phflg) {
        J=bc+ds*dbuf[y*wdt+x];
        K=ss*sbuf[y*wdt+x];
        
        rv=r[y*wdt+x]*J+255*K;
        gv=g[y*wdt+x]*J+255*K;
        bv=b[y*wdt+x]*J+255*K;
        if (rv>255) rv=255;
        if (gv>255) gv=255;
        if (bv>255) bv=255;
        if (rv<0) rv=0;
        if (gv<0) gv=0;
        if (bv<0) bv=0;
        r[y*wdt+x]=rv;
        g[y*wdt+x]=gv;
        b[y*wdt+x]=bv;
      }
    
    }
  }
  
  if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0;
  
}  


