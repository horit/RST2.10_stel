/* fovp.c
   ====== 
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
#include <time.h>

#include "rtypes.h"
#include "rmath.h"
#include "rxml.h"
#include "option.h"

#include "rfbuffer.h"
#include "rps.h"
#include "fontdb.h"
#include "rplot.h"

#include "polygon.h"

#include "rtime.h"
#include "rmap.h"
#include "sza.h"
#include "aacgm.h"

#include "radar.h" 

#include "make_fov.h"
#include "clip.h"


#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: fovp.c,v $
 Revision 1.4  2006/07/14 15:59:41  code
 Allowed altitude to be set on the command line.

 Revision 1.3  2005/09/09 15:51:09  barnes
 Modification to allow the number of ranges, distance to first range and range separation to be modified.

 Revision 1.2  2005/08/10 15:02:09  barnes
 Added option to dump command line options.

 Revision 1.1  2005/04/13 19:50:07  barnes
 Initial revision

*/



struct OptionData opt;
struct OptionFile *optf=NULL;

struct PolygonData *fov;
struct PolygonData *nfov;
struct PolygonData *pfov;

struct PolygonData *clip;

struct RadarNetwork *network;

double eqn_of_time(double mean_lon,int yr);
int solar_loc(int yr,int t1,double *mean_lon,double *dec);

void record_info(struct Rplot *rplot,int argc,char *argv[]) {
  int i;
  char *date;
  char buf[256];
  time_t ltime;
  struct tm *time_of_day;
  time(&ltime);
  time_of_day=gmtime(&ltime);
  date=asctime(time_of_day);
  date[strlen(date)-1]=0;
  sprintf(buf,"<created>%s</created>\n",date);
  RplotInfo(rplot,buf,strlen(buf));
  sprintf(buf,"<command>\n");
  RplotInfo(rplot,buf,strlen(buf));
  for (i=0;i<argc;i++) {
    if (i !=0) RplotInfo(rplot," ",1);
    RplotInfo(rplot,argv[i],strlen(argv[i]));
  }
  sprintf(buf,"\n</command>\n");
  RplotInfo(rplot,buf,strlen(buf));
  sprintf(buf,"<version><major>%d</major><minor>%d</minor></version>\n",
    	  MAJOR_VERSION,MINOR_VERSION);
  RplotInfo(rplot,buf,strlen(buf));

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


struct PolygonData *wrap(struct PolygonData *src) {
  struct PolygonData *dst;
  dst=MapWraparound(src);
  PolygonFree(src);
  return dst;
}

int main(int argc,char *argv[]) {

  int arg;

  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;

  float wdt=540,hgt=540;
 
  float width=0.5;

  struct Rplot *rplot=NULL;


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
  float alt=0.0;

  unsigned char lstflg=0;

  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char fovflg=0;
  unsigned char ffovflg=0;

  char *fovcol_txt=NULL;
  char *ffovcol_txt=NULL;

  unsigned int fovcol;
  unsigned int ffovcol;


  float marg[4];
  

  char *tmetxt=NULL;
  char *dtetxt=NULL;

  double tval=-1;
  double dval=-1;

  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  float tme_shft;

  double LsoT,LT,Hangle,dec,eqt,mlon;

  int frang=180,rsep=45,nrang=0;
 
  int stid=3;
  char *ststr=NULL;
  int pnum=0,n;
  int stnum[255];

  envstr=getenv("MAPDATA");

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  fovcol=RplotColor(0x00,0x00,0x00,0xff);
  ffovcol=RplotColor(0xc0,0x00,0x00,0xff);
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);

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
  OptionAdd(&opt,"alt",'f',&alt);

  OptionAdd(&opt,"fov",'x',&fovflg);
  OptionAdd(&opt,"ffov",'x',&ffovflg);
 
  OptionAdd(&opt,"fovcol",'t',&fovcol_txt);
  OptionAdd(&opt,"ffovcol",'t',&ffovcol_txt);

  OptionAdd(&opt,"st",'t',&ststr);
 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);

  OptionAdd(&opt,"frang",'i',&frang);
  OptionAdd(&opt,"rsep",'i',&rsep);
  OptionAdd(&opt,"nrang",'i',&nrang);
  
  
  
  OptionAdd(&opt,"lnewdt",'f',&width);

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


  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  tval+=dval;
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  fov=make_fov(tval,frang,rsep,nrang,network,alt); 

  if (magflg) PolygonModify(fov,AACGMtransform,NULL);

  if ((ortho==0) && (stereo==0)) cylind=1;

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;
 
  marg[0]=lat;
  marg[1]=lon;

  tfunc=MapCylindrical;
  if (ortho) tfunc=MapOrthographic;
  if (stereo) tfunc=MapStereographic;
  if (gvp) tfunc=MapGeneralVerticalPerspective;

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
  
  if ((cylind) || (sqflg)) clip=MapSquareClip(); 
  else clip=MapCircleClip(10); 

  nfov=PolygonTransform(fov,2*sizeof(float),PolygonXYbbox,
                     tfunc,marg);
  if (cylind) nfov=wrap(nfov);
  pfov=PolygonClip(clip,nfov);

  if (fovcol_txt !=NULL) fovcol=RplotColorStringRGB(fovcol_txt);
  if (ffovcol_txt !=NULL) ffovcol=RplotColorStringRGB(ffovcol_txt);

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }

  if (ststr !=NULL) {
    char *tmp;
    tmp=strtok(ststr,",");
    do {
       stid=RadarGetID(network,tmp);
       for (n=0;n<network->rnum;n++) 
         if (stid==network->radar[n].id) break;  
       if (n==network->rnum) continue;
       stnum[pnum]=n;
       pnum++;
     } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  if (arg !=argc) {
    int x;
    for (x=arg;x<argc;x++) {
      stid=atoi(argv[x]);
       for (n=0;n<network->rnum;n++) 
         if (stid==network->radar[n].id) break;  
       if (n==network->rnum) continue;
       stnum[pnum]=n;
       pnum++;
    }
  }


  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);

  RplotMakePlot(rplot,"fov_plot",wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);
 

  for (n=0;n<pnum;n++) {
    if (ffovflg) MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,ffovcol,0x0f,
                                width,NULL,pfov,stnum[n]);
  }
  for (n=0;n<pnum;n++) {
    if (fovflg) MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,0,fovcol,0x0f,width,NULL,
                               pfov,stnum[n]);
  }
  RplotEndPlot(rplot);  
  return 0;

}  


