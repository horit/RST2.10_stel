/* fov_plot.c
   ========== 
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

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif

#include "rtypes.h"
#include "rxml.h"
#include "option.h"

#include "rfbuffer.h"
#include "rps.h"
#include "fontdb.h"
#include "rplot.h"

#include "rplotin.h"
#include "imagedb.h"
#include "fbrplot.h"
#include "psrplot.h"

#include "polygon.h"


#ifdef _XLIB_
#include "xwin.h"
#endif
#include "raster.h"
#include "rtime.h"
#include "rmap.h"
#include "contour.h"
#include "rplotgraw.h"
#include "rplotgstd.h"
#include "aacgm.h"

#include "radar.h" 

#include "make_fov.h"
#include "text_box.h"
#include "plot_time_label.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"
#include "clip.h"


#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: fov_plot.c,v $
 Revision 1.12  2006/07/14 15:31:50  code
 Modification to allow altitude to be specified on the command line.

 Revision 1.11  2006/01/30 20:03:37  barnes
 Removed debug code.

 Revision 1.10  2005/11/10 22:23:11  barnes
 Fixed bug in placing dots on the plot.

 Revision 1.9  2005/11/10 21:55:44  barnes
 Changes to implement vector only plotting and placing dots at the station
 locations.

 Revision 1.8  2005/11/09 18:59:12  barnes
 Fixed bug in drawing radars under construction

 Revision 1.7  2005/11/02 20:32:38  barnes
 Corrected bug in rendering the filled fields of view.

 Revision 1.6  2005/08/10 15:04:01  barnes
 Added option to dump the command line options.

 Revision 1.5  2005/04/18 18:32:42  barnes
 Added the ability to control the padding, and place the grid on top.
 ./

 Revision 1.4  2004/06/21 22:43:19  barnes
 Added missing header.

 Revision 1.3  2004/06/03 19:57:49  barnes
 Adopted the use of strings as radar identifiers.

 Revision 1.2  2004/05/10 21:55:45  barnes
 Removed redundant code.

 Revision 1.1  2003/04/25 14:45:33  barnes
 Initial revision

*/



struct OptionData opt;
struct OptionFile *optf=NULL;

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;
 
struct PolygonData *bnd;
struct PolygonData *nbnd;
struct PolygonData *pbnd;

struct PolygonData *grd;
struct PolygonData *ngrd;
struct PolygonData *pgrd;

struct PolygonData *tmk;
struct PolygonData *ntmk;
struct PolygonData *ptmk;

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *fov;
struct PolygonData *nfov;
struct PolygonData *pfov;

struct PolygonData *clip;

struct FrameBufferClip *fbclp=NULL;

float *zenith;

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

int xmldecode(char *buf,int sze,void *data) {
  struct XMLdata *xmldata;
  xmldata=(struct XMLdata *) data;
  return XMLDecode(xmldata,buf,sze);
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



#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="fov_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  struct FrameBuffer *img=NULL; 
  struct FrameBufferRplot *fbfbplot=NULL;
  struct XMLdata *fbxmldata=NULL;
  struct RplotInPlot *fbrplotin=NULL;
  struct Rplot *fbrplot=NULL;

  struct FrameBuffer *fbimg=NULL;
  struct FrameBufferRplot *fbplot=NULL;
  struct PostScriptRplot *psplot=NULL;
  struct PostScript *psdata=NULL;
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int txtcol;

  int arg;

  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;



  float wdt=540,hgt=540;
  float pad=0;

  char *fontname=NULL;
  float fontsize=10.0;
  char *dfontname="Helvetica";

  float width=0.5;

  struct Rplot *rplot=NULL;

  float xpoff=40,ypoff=40;
  unsigned char psflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char gflg=0;
  unsigned char pflg=0;
  unsigned char rflg=0;

  unsigned char help=0; 
  unsigned char option=0; 

  char *bgcol_txt=NULL;
  char *txtcol_txt=NULL;


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

  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char mapflg=0;
  unsigned char fmapflg=0;
  unsigned char bndflg=0;
  unsigned char grdflg=0;
  unsigned char tmkflg=0;
  unsigned char tlblflg=0;
  unsigned char ofovflg=0;
  unsigned char fofovflg=0;

  unsigned char cfovflg=0;
  unsigned char fcfovflg=0;

  unsigned char fovflg=0;
  unsigned char ffovflg=0;

  unsigned char grdontop=0;

  unsigned char dotflg=0;

  int tmtick=3;

  unsigned char lstflg=0;
  unsigned char trmflg=0;
  unsigned char ftrmflg=0;
  unsigned char strmflg=0;
  float tmin=0.5;
  float tscl=10.0;

  char *grdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *lndcol_txt=NULL;
  char *seacol_txt=NULL;
  char *trmcol_txt=NULL;
  char *ftrmcol_txt=NULL;
  char *tmkcol_txt=NULL;
  char *ofovcol_txt=NULL;
  char *fofovcol_txt=NULL;
  char *fovcol_txt=NULL;
  char *ffovcol_txt=NULL;

  char *cfovcol_txt=NULL;
  char *fcfovcol_txt=NULL;


  unsigned int grdcol;
  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;
  unsigned int ftrmcol;
  unsigned int tmkcol;
  unsigned int ofovcol;
  unsigned int fofovcol;
  unsigned int fovcol;
  unsigned int ffovcol;

  unsigned int cfovcol;
  unsigned int fcfovcol;
  

  FILE *mapfp;
  float marg[4];
  int i;

  int iwdt,ihgt;
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
  double Z; 
  float dotr=2; 
 
  int stid=-1;
  char *ststr=NULL;
  int stnum=0;

  char tsfx[16];


  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

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


  bgcol=RplotColor(0xff,0xff,0xff,0xff);
  txtcol=RplotColor(0x00,0x00,0x00,0xff);

  grdcol=RplotColor(0x20,0x40,0x60,0xff);
  cstcol=RplotColor(0xa0,0xa0,0xa0,0xff);
  bndcol=RplotColor(0x80,0x80,0x80,0xff);
  lndcol=RplotColor(0xff,0xff,0xf0,0xff);
  seacol=RplotColor(0xe0,0xf0,0xff,0xff);
  tmkcol=RplotColor(0x00,0x00,0x00,0xff);
  ofovcol=RplotColor(0x00,0x00,0x00,0xff);
  fofovcol=RplotColor(0xff,0xff,0xff,0xff);
  fovcol=RplotColor(0x00,0x00,0x00,0xff);
  ffovcol=RplotColor(0xc0,0x00,0x00,0xff);
  trmcol=RplotColor(0x80,0x80,0x80,0xff);
  ftrmcol=RplotColor(0x80,0x80,0x80,0xff);

  cfovcol=RplotColor(0x00,0x00,0x00,0xff);
  fcfovcol=RplotColor(0xff,0xff,0xff,0xff);
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);


#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
#endif

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);

  OptionAdd(&opt,"xml",'x',&xmlflg);
  OptionAdd(&opt,"ps",'x',&psflg); 

  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"pad",'f',&pad);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);

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
  OptionAdd(&opt,"alt",'f',&alt);
  OptionAdd(&opt,"flip",'x',&flip);

  OptionAdd(&opt,"coast",'x',&mapflg);
  OptionAdd(&opt,"fcoast",'x',&fmapflg);
  OptionAdd(&opt,"bnd",'x',&bndflg);
  OptionAdd(&opt,"grd",'x',&grdflg);
  OptionAdd(&opt,"tmk",'x',&tmkflg);

 OptionAdd(&opt,"grdontop",'x',&grdontop);


  OptionAdd(&opt,"fov",'x',&fovflg);
  OptionAdd(&opt,"ffov",'x',&ffovflg);

  OptionAdd(&opt,"ofov",'x',&ofovflg);
  OptionAdd(&opt,"fofov",'x',&fofovflg);

  OptionAdd(&opt,"cfov",'x',&cfovflg);
  OptionAdd(&opt,"fcfov",'x',&fcfovflg);

  OptionAdd(&opt,"tmtick",'i',&tmtick);
  OptionAdd(&opt,"lst",'x',&lstflg);
  OptionAdd(&opt,"term",'x',&trmflg);
  OptionAdd(&opt,"fterm",'x',&ftrmflg);
  OptionAdd(&opt,"sterm",'x',&strmflg);
  OptionAdd(&opt,"tmin",'f',&tmin);
  OptionAdd(&opt,"tscl",'f',&tscl);

  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"lndcol",'t',&lndcol_txt);
  OptionAdd(&opt,"seacol",'t',&seacol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
  OptionAdd(&opt,"tmkcol",'t',&tmkcol_txt);
 
  OptionAdd(&opt,"fovcol",'t',&fovcol_txt);
  OptionAdd(&opt,"ffovcol",'t',&ffovcol_txt);
  OptionAdd(&opt,"ofovcol",'t',&ofovcol_txt);
  OptionAdd(&opt,"fofovcol",'t',&fofovcol_txt);

  OptionAdd(&opt,"cfovcol",'t',&cfovcol_txt);
  OptionAdd(&opt,"fcfovcol",'t',&fcfovcol_txt);

  OptionAdd(&opt,"st",'t',&ststr);

  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"tmlbl",'x',&tlblflg);

  OptionAdd(&opt,"fontname",'t',&fontname);
  OptionAdd(&opt,"fontsize",'f',&fontsize);
  OptionAdd(&opt,"lnewdt",'f',&width);

  OptionAdd(&opt,"dotr",'f',&dotr);
  OptionAdd(&opt,"dot",'x',&dotflg);


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

  if (magflg) {
    PolygonModify(map,AACGMtransform,NULL);
    PolygonModify(bnd,AACGMtransform,NULL);
  }

  fov=make_fov(tval,network,alt); 

  if (magflg) PolygonModify(fov,AACGMtransform,NULL);


  if (tmtick<1) tmtick=1;
  if (tmtick>6) tmtick=6;

  if ((ortho==0) && (stereo==0)) cylind=1;

  if (grdflg) grd=make_grid(15,10,cylind);   
 
  if (tmkflg) tmk=make_grid(30*tmtick,10,cylind);

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  if (trmflg || ftrmflg) {
    if ((cylind) || (ortho) | (gvp))
       trm=SZATerminator(yr,mo,dy,hr,mt,sc,0,magflg,
                           1.0,90.0);
     else if (lat>0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,magflg,
                                     1.0,90.0);
     else trm=SZATerminator(yr,mo,dy,hr,mt,sc,-1,magflg,1.0,90.0);
  }
 
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

  strcpy(tsfx,"LT");
  if (magflg) strcpy(tsfx,"MLT");
  else if (lstflg) strcpy(tsfx,"LST");

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

  if (mapflg || fmapflg) {
     nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
                           tfunc,marg);
     if (cylind) nmap=wrap(nmap);
     pmap=PolygonClip(clip,nmap); 
  }
  if (bndflg) {
     nbnd=PolygonTransform(bnd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
     if (cylind) nbnd=wrap(nbnd);
     pbnd=PolygonClip(clip,nbnd);
  }
  if (grdflg) {
     ngrd=PolygonTransform(grd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
     if (cylind) ngrd=wrap(ngrd);
     pgrd=PolygonClip(clip,ngrd);
  }
  if (trmflg || ftrmflg) {
     ntrm=PolygonTransform(trm,2*sizeof(float),PolygonXYbbox,
                           tfunc,marg);
     if (cylind) ntrm=wrap(ntrm);
     ptrm=PolygonClip(clip,ntrm); 
  }

  nfov=PolygonTransform(fov,2*sizeof(float),PolygonXYbbox,
                     tfunc,marg);
  if (cylind) nfov=wrap(nfov);
  pfov=PolygonClip(clip,nfov);

  if (tmkflg) {
     float tmp;
     tmp=marg[1];
     if (rotflg) marg[1]=0;
     else marg[1]=lon-tme_shft;
     ntmk=PolygonTransform(tmk,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
     ptmk=PolygonClip(clip,ntmk);
     marg[1]=tmp;
  }


  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=RplotColorStringRGB(txtcol_txt);

  if (grdcol_txt !=NULL) grdcol=RplotColorStringRGB(grdcol_txt);
  if (cstcol_txt !=NULL) cstcol=RplotColorStringRGB(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=RplotColorStringRGB(bndcol_txt);
  if (lndcol_txt !=NULL) lndcol=RplotColorStringRGB(lndcol_txt);
  if (seacol_txt !=NULL) seacol=RplotColorStringRGB(seacol_txt);
  if (trmcol_txt !=NULL) trmcol=RplotColorStringRGB(trmcol_txt);
  if (ftrmcol_txt !=NULL) ftrmcol=RplotColorStringRGB(ftrmcol_txt);

  if (tmkcol_txt !=NULL) tmkcol=RplotColorStringRGB(tmkcol_txt);

  if (fovcol_txt !=NULL) fovcol=RplotColorStringRGB(fovcol_txt);
  if (ofovcol_txt !=NULL) ofovcol=RplotColorStringRGB(ofovcol_txt);
  if (cfovcol_txt !=NULL) cfovcol=RplotColorStringRGB(cfovcol_txt);
 
  if (ffovcol_txt !=NULL) ffovcol=RplotColorStringRGB(ffovcol_txt);
  if (fofovcol_txt !=NULL) fofovcol=RplotColorStringRGB(fofovcol_txt);
  if (fcfovcol_txt !=NULL) fcfovcol=RplotColorStringRGB(fcfovcol_txt);


  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }


  if (tlblflg) pad=10+fontsize;
  
  if (fontname==NULL) fontname=dfontname;

  fntdbfname=getenv("FONTDB");
  fontfp=fopen(fntdbfname,"r");
  if (fontfp !=NULL) {
   fontdb=FrameBufferFontDBLoad(fontfp);
   fclose(fontfp);
  }
 
  if (fontdb==NULL) {
   fprintf(stderr,"Could not load fonts.\n");
   exit(-1);
  }


  if (ststr !=NULL) stid=RadarGetID(network,ststr);
  for (stnum=0;stnum<network->rnum;stnum++) 
     if (stid==network->radar[stnum].id) break;  
  if (stnum==network->rnum) stnum=0;



  /* now determine our output type */

  if (psflg) pflg=1;
  if (xmlflg) gflg=1;
  if (ppmflg) gflg=1;
  if (ppmxflg) gflg=1;

#ifdef _XLIB_ 
   if (xd !=0) {
     pflg=0; 
     gflg=1;
   }
#endif

  if (pflg) gflg=0;
  rflg=pflg | gflg; /* we must render the output */

  if (rflg) {
     xmldata=XMLMake();
     rplotin=RplotInMake(xmldata);  

     if (gflg) {
       fbplot=FrameBufferRplotMake(fontdb,NULL);
       FrameBufferRplotSetAll(rplotin,fbplot);
     }
     if (pflg) {
       psdata=PostScriptMake();
       PostScriptSetText(psdata,stream,stdout);
       psplot=PostScriptRplotMake(psdata,NULL);
       PostScriptRplotSetAll(rplotin,psplot);
       psplot->land=0;
       psplot->xoff=xpoff;
       psplot->yoff=ypoff;
     }
     XMLSetStart(xmldata,RplotInStart,rplotin);
     XMLSetEnd(xmldata,RplotInEnd,rplotin);
  }

  rplot=RplotMake();
  if (rflg) RplotSetText(rplot,xmldecode,xmldata);
  else RplotSetText(rplot,stream,stdout);


  fbxmldata=XMLMake();
  fbrplotin=RplotInMake(fbxmldata);  
  fbfbplot=FrameBufferRplotMake(fontdb,NULL);
  FrameBufferRplotSetAll(fbrplotin,fbfbplot);
  XMLSetStart(fbxmldata,RplotInStart,fbrplotin);
  XMLSetEnd(fbxmldata,RplotInEnd,fbrplotin);
  fbrplot=RplotMake();
  RplotSetText(fbrplot,xmldecode,fbxmldata);

 
  iwdt=wdt-2*pad;
  ihgt=hgt-2*pad;
  if (pflg) {
    iwdt=iwdt*2;
    ihgt=ihgt*2;
  }
 
  if (strmflg) {
    if (cylind) {
      marg[1]=0;
      marg[2]=0;
    }
    zenith=SZAMap(yr,mo,dy,hr,mt,sc,-90.0,
                   iwdt,ihgt,magflg,tfunc,marg);   
 
  
    RplotMakePlot(fbrplot,"fov_plot",iwdt,ihgt,24);
    RplotRectangle(fbrplot,NULL,0,0,iwdt,ihgt,1,bgcol,0x0f,0,NULL);
    if (fmapflg) {
        if ((cylind) || (sqflg)) RplotRectangle(fbrplot,NULL,0,0,iwdt,ihgt,
                                 1,seacol,0x0f,0,NULL);
        RplotEllipse(fbrplot,NULL,iwdt/2,ihgt/2,iwdt/2,ihgt/2,
                  1,seacol,0x0f,0,NULL);
    }

    if ((grdflg) && (!grdontop)) {
      MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,0,
                                  grdcol,0x0f,width,NULL,
                                  pgrd,1);
    }

    if (fmapflg) {
      MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,1,lndcol,0x0f,0,NULL,
                     pmap,1);
  
      MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,1,lndcol,0x0f,0,NULL,
                     pmap,3);
  
      MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,1,seacol,0x0f,0,NULL,
                     pmap,0);
    }

    if (fofovflg) {
      for (i=0;i<network->rnum;i++) {
        if (network->radar[i].id==stid) continue;
        if (network->radar[i].status !=1) continue;
        MapPlotPolygon(fbrplot,NULL,0,0,
                     iwdt,ihgt,1,fofovcol,0x0f,0,NULL,pfov,i); 
      }
    }

    if (fcfovflg) {
      for (i=0;i<network->rnum;i++) {
        if (network->radar[i].id==stid) continue;
        if (network->radar[i].status!=0) continue;
        MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,1,fcfovcol,0x0f,0,
                       NULL,pfov,i); 
      }
    }


    if (ffovflg) {
      MapPlotPolygon(fbrplot,NULL,0,0,
                    iwdt,ihgt,1,ffovcol,0x0f,0,NULL,pfov,stnum); 
    }

    RplotEndPlot(fbrplot);
    if ((cylind) || (sqflg)) fbclp=NULL;
    else fbclp=fbclip(iwdt,ihgt);
    img=fbplot->img; 

  
    img=fbfbplot->img; 
    r=img->img;
    g=img->img+img->wdt*img->hgt;
    b=img->img+img->wdt*img->hgt*2;

    for (x=0;x<iwdt;x++) {
      for (y=0;y<ihgt;y++) {
        if ((fbclp !=NULL) && (fbclp->clp[y*iwdt+x]==0)) continue;
        Z=zenith[y*iwdt+x];    
        if (Z>90) {
	  if (tscl !=0) Z=(1-(Z-90)/tscl); 
          else Z=0.0;
	} else Z=1.0;
        if (Z>1.0) Z=1.0;
        if (Z<tmin) Z=tmin;

	r[y*iwdt+x]=r[y*iwdt+x]*Z;
	g[y*iwdt+x]=g[y*iwdt+x]*Z;
	b[y*iwdt+x]=b[y*iwdt+x]*Z;
      }
    }
  }

  RplotMakePlot(rplot,"fov_plot",wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
  if (!strmflg) {   
    if (fmapflg) {
        if ((cylind) || (sqflg)) RplotRectangle(rplot,NULL,pad,pad,
                                 wdt-2*pad,hgt-2*pad,
                                 1,seacol,0x0f,0,NULL);
        RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                    1,seacol,0x0f,0,NULL);
    }

    if ((grdflg) && (!grdontop)) {
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
    }

    if (fmapflg) {
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                     lndcol,0x0f,0,NULL,pmap,1);
  
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                     lndcol,0x0f,0,NULL,pmap,3);
  
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                     seacol,0x0f,0,NULL,pmap,0);
    }

    if (ftrmflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                               1,ftrmcol,0x0f,0,NULL,ptrm,1);

  
    if (fofovflg) {
      for (i=0;i<network->rnum;i++) {
        if (network->radar[i].id==stid) continue;
        if (network->radar[i].status !=1) continue;
        MapPlotPolygon(rplot,NULL,pad,pad,
                     wdt-2*pad,hgt-2*pad,1,fofovcol,0x0f,0,NULL,pfov,i); 
      }
    }

    if (fcfovflg) {
      for (i=0;i<network->rnum;i++) {
        if (network->radar[i].id==stid) continue;
        if (network->radar[i].status!=0) continue;
        MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                       fcfovcol,0x0f,0,NULL,pfov,i); 
      }
    }


    if (ffovflg) {
     MapPlotPolygon(rplot,NULL,pad,pad,
                  wdt-2*pad,hgt-2*pad,1,ffovcol,0x0f,0,NULL,pfov,stnum); 
    }
  }
  if (strmflg) {
    if (pflg) {
      struct RplotMatrix m;
      m.a=(wdt-2*pad)/img->wdt;
      m.b=0;
      m.c=0;
      m.d=(hgt-2*pad)/img->hgt;
      RplotImage(rplot,&m,img,0x0f,pad,pad,0);
    } else RplotImage(rplot,NULL,img,0x0f,pad,pad,0);
  }
  if (mapflg) {
     MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    width,NULL,
                    pmap,1);
  
     MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,cstcol,0x0f,
                    width,NULL,
                    pmap,2);
  
     MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    width,NULL,
                    pmap,0);
  }

  if (ofovflg)  {
    for (i=0;i<network->rnum;i++) {
      if (network->radar[i].id==stid) continue;
      if (network->radar[i].status !=1) continue;
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                    ofovcol,0x0f,width,NULL,
                    pfov,i);
    } 
  }

 if (cfovflg)  {
   
    for (i=0;i<network->rnum;i++) {
      if (network->radar[i].id==stid) continue;
      if (network->radar[i].status !=0) continue;
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                    cfovcol,0x0f,width,NULL,
                    pfov,i);
    } 
  }

 if (dotflg) {
   int s=0;
   struct RadarSite *site; 
   float pnt[2]; 
   if (cfovflg | fcfovflg)  {
     for (i=0;i<network->rnum;i++) {
       if (network->radar[i].id==stid) continue;
       if (network->radar[i].status !=0) continue;
       site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,sc);
       pnt[0]=site->geolat;
       pnt[1]=site->geolon; 
       s=(*tfunc)(sizeof(float)*2,pnt,2*sizeof(float),pnt,marg);
       if (s==0) RplotEllipse(rplot,NULL,pad+pnt[0]*(wdt-2*pad),
                    pad+pnt[1]*(hgt-2*pad),dotr,dotr,
                    1,fcfovcol,0x0f,0,NULL);
           
     } 
   }
   if (ofovflg | fofovflg)  {
     for (i=0;i<network->rnum;i++) {
       if (network->radar[i].id==stid) continue;
       if (network->radar[i].status !=1) continue;
       site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,sc);
       pnt[0]=site->geolat;
       pnt[1]=site->geolon; 
       s=(*tfunc)(sizeof(float)*2,pnt,2*sizeof(float),pnt,marg);
       if (s==0) RplotEllipse(rplot,NULL,pad+pnt[0]*(wdt-2*pad),
                    pad+pnt[1]*(hgt-2*pad),dotr,dotr,
                    1,fofovcol,0x0f,0,NULL);
           
     } 
   }

   if (fovflg) {
     
     site=RadarYMDHMSGetSite(&(network->radar[stnum]),yr,mo,dy,hr,mt,sc);
     pnt[0]=site->geolat;
     pnt[1]=site->geolon; 
     s=(*tfunc)(sizeof(float)*2,pnt,2*sizeof(float),pnt,marg);
     if (s==0) RplotEllipse(rplot,NULL,pad+pnt[0]*(wdt-2*pad),
                    pad+pnt[1]*(hgt-2*pad),dotr,dotr,
                    1,ffovcol,0x0f,0,NULL);
   }


 }

  if (fovflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                    fovcol,0x0f,width,NULL,
                    pfov,stnum);

  if (bndflg) MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                bndcol,0x0f,width,NULL,
                                pbnd,-1);

  if (trmflg) MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                trmcol,0x0f,width,NULL,
                                ptrm,1);

  if (tmkflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                tmkcol,0x0f,width,NULL,
                                ptmk,1);

  if ((grdflg) && (grdontop)) {
    MapPlotPolygon(rplot,NULL,0,0,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }


  if ((cylind) || (sqflg)) 
     RplotRectangle(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
               0,grdcol,0x0f,width,NULL);
  else RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
               0,grdcol,0x0f,width,NULL);

  if (tlblflg){
      if (lat>0) plot_time_label(rplot,pad,pad,wdt-2*pad,hgt-2*pad,
				 90,flip,tsfx,
                                 lon-tme_shft*(! rotflg),
                                 (wdt/2)-pad,6,
                                 txtcol,0x0f,fontname,fontsize,fontdb);
      else plot_time_label(rplot,pad,pad,wdt-2*pad,hgt-2*pad,
			   -90,flip,tsfx,
                           lon-tme_shft*(! rotflg),
                           (wdt/2)-pad,6,
                           txtcol,0x0f,fontname,fontsize,fontdb);
  }

  RplotEndPlot(rplot);  

  if (fbplot==NULL) exit(0);
  if (fbplot->img==NULL) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }
  fbimg=fbplot->img;

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-fbimg->wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-fbimg->hgt)/2;


    win=XwinMakeWindow(xdoff,ydoff,fbimg->wdt,fbimg->hgt,0,
                       dp,wname,
                       wname,argv[0],wname,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }

    XwinFrameBufferWindow(fbimg,win);

    XwinShowWindow(win);

    XwinDisplayEvent(dp,1,&win,1,NULL);

    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  } else {
    if (xmlflg==1) FrameBufferSaveXML(fbimg,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(fbimg,stdout);
    else FrameBufferSavePPMX(fbimg,stdout);
  }
  #else 
    if (xmlflg==1) FrameBufferSaveXML(fbimg,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(fbimg,stdout);
    else FrameBufferSavePPMX(fbimg,stdout);
  #endif
    return 0;
}  


