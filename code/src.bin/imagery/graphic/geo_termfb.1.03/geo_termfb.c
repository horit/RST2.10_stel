/* geo_termfb.c
   ============ 
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
#include <sys/types.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "raster.h"
#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"


#include "sza.h"
#include "szamap.h"
#include "clip.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: geo_termfb.c,v $
 Revision 1.6  2005/08/11 14:38:43  barnes
 Added option to dump command line options.

 Revision 1.5  2004/06/21 22:18:39  barnes
 Added missing header.

 Revision 1.4  2004/05/05 16:32:29  barnes
 Added missing header.

 Revision 1.3  2004/05/04 20:08:03  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 20:29:41  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/24 18:08:49  barnes
 Initial revision

*/


struct OptionData opt;
struct OptionFile *optf=NULL;

struct FrameBufferClip *fbclp=NULL;

float *zenith;

struct FrameBuffer *loadxml(FILE *fp) {

  struct FrameBuffer *img=NULL;
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;
  
  char lbuf[256];
  int s=0;

  xmldata=XMLMake();
  xmlimg.xml=xmldata;
  xmlimg.img=&img;

  XMLSetStart(xmldata,FrameBufferXMLStart,&xmlimg);
  XMLSetEnd(xmldata,FrameBufferXMLEnd,&xmlimg);

  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }
  XMLFree(xmldata);
  return img;
}


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

  struct FrameBuffer *img=NULL;

  int arg;
  char *cfname=NULL;
  FILE *fp;
 
  float Z=0;

  int wdt=540,hgt=540;
  int pad=0;
 
  unsigned char ippmflg=0;
  unsigned char ippmxflg=0;
  unsigned char ixmlflg=0;

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

  unsigned char magflg=0;
  unsigned char rotflg=0;
  unsigned char lstflg=0;

  float tmin=0.5;
  float tscl=10.0;

  
  float marg[4];
  

  int x,y;
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;

  char *tmetxt=NULL;
  char *dtetxt=NULL;

  double tval=-1;
  double dval=-1;

  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  float tme_shft;

  double LsoT,LT,Hangle,dec,eqt,mlon;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);
  
  OptionAdd(&opt,"pad",'i',&pad);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"ippm",'x',&ippmflg);
  OptionAdd(&opt,"ippmx",'x',&ippmxflg);
  OptionAdd(&opt,"ixml",'x',&ixmlflg);

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
  OptionAdd(&opt,"tmin",'f',&tmin);
  OptionAdd(&opt,"tscl",'f',&tscl);

 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
 
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

  if (ippmflg) img=FrameBufferLoadPPM(fp,"image");
  else if (ippmxflg) img=FrameBufferLoadPPMX(fp);
  else img=loadxml(fp);
  fclose(fp);

  wdt=img->wdt;
  hgt=img->hgt;

  if (img==NULL) {
    fprintf(stderr,"Error loading image.\n");
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
 
  marg[0]=lat;
  marg[1]=lon;
  
  tfunc=MapCylindrical;
  if (ortho) tfunc=MapOrthographic;
  if (stereo) tfunc=MapStereographic;
  if (gvp) tfunc=MapGeneralVerticalPerspective;

  if ((ortho) || (gvp)) marg[2]=sf;
  else marg[2]=1.25*0.5*sf*90.0/(90-fabs(latmin));
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

  zenith=SZAMap(yr,mo,dy,hr,mt,sc,-90.0,
                   wdt-2*pad,hgt-2*pad,magflg,tfunc,marg);   
    
  if ((cylind) || (sqflg)) fbclp=NULL;
  else fbclp=fbclip(wdt-2*pad,hgt-2*pad);
 
  r=img->img;
  g=img->img+img->wdt*img->hgt;
  b=img->img+img->wdt*img->hgt*2;

  for (x=0;x<wdt-2*pad;x++) {
    for (y=0;y<hgt-2*pad;y++) {
      if ((fbclp !=NULL) && (fbclp->clp[y*(wdt-2*pad)+x]==0)) continue;
      if (zenith !=NULL) Z=zenith[y*(wdt-2*pad)+x];       

      if (Z>90) {
        if (tscl !=0) Z=(1-(Z-90)/tscl); 
        else Z=0.0;
      } else Z=1.0;
      if (Z>1.0) Z=1.0;
      if (Z<tmin) Z=tmin;

      r[(y+pad)*wdt+pad+x]=r[(y+pad)*wdt+pad+x]*Z;
      g[(y+pad)*wdt+pad+x]=g[(y+pad)*wdt+pad+x]*Z;
      b[(y+pad)*wdt+pad+x]=b[(y+pad)*wdt+pad+x]*Z;
    }
  } 
  if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0;
  
}  


