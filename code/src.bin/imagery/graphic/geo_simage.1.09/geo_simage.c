/* geo_simage.c
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

#include "simage.h"

#include "key.h"
#include "text_box.h"
#include "plot_time.h"
#include "make_grid.h"
#include "sza.h"

#include "clip.h"

#include "plot_time_label.h"
#include "map.h"
#include "szamap.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: geo_simage.c,v $
 Revision 1.11  2005/11/02 20:45:14  barnes
 Modifications to handle GUVI L1C data.

 Revision 1.10  2005/09/01 21:32:35  barnes
 Fixed overlay.

 Revision 1.9  2005/09/01 20:46:53  barnes
 Fixed basic colors.

 Revision 1.8  2005/08/11 14:35:02  barnes
 Added option to dump command line options.

 Revision 1.7  2004/06/21 22:16:21  barnes
 Added missing header.

 Revision 1.6  2004/06/21 14:41:25  barnes
 Fixed bug in plotting the time value.

 Revision 1.5  2004/06/08 21:03:41  barnes
 Added the alternative grid, and the ability to turn the frame on and off
 for overlays. Also allowed the user to set the image padding.

 Revision 1.4  2004/05/04 20:07:06  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/04/28 15:31:21  barnes
 Added time.h header.

 Revision 1.2  2004/04/27 20:25:21  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/09 19:50:34  barnes
 Initial revision

*/

unsigned char gry[256];

struct key key;

struct OptionData opt;
struct OptionFile *optf=NULL;

struct SImage simage;

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;
 
struct PolygonData *bnd;
struct PolygonData *nbnd;
struct PolygonData *pbnd;

struct PolygonData *grd;
struct PolygonData *ngrd;
struct PolygonData *pgrd;

struct PolygonData *igrd;
struct PolygonData *nigrd;
struct PolygonData *pigrd;

struct PolygonData *tmk;
struct PolygonData *ntmk;
struct PolygonData *ptmk;

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *clip;

struct FrameBufferClip *fbclp=NULL;


float *image;
float *zenith;


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

char *label_key(double val,double min,double max,void *data) {
  char *txt=NULL;
  char *unit=NULL;
  if (data !=NULL) unit=(char *)data;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (unit==NULL) {
    if (val==max) sprintf(txt,"%g",val);
    if (val==min) sprintf(txt,"%g",val); 
  } else {
    if (val==max) sprintf(txt,"%g",val);
    if (val==min) sprintf(txt,"%g %s",val,unit); 
  }
  return txt;
}

char *label_keyall(double val,double min,double max,void *data) {
  char *txt=NULL;
  char *unit=NULL;
  if (data !=NULL) unit=(char *)data;
  txt=malloc(32); 
  if (unit==NULL) sprintf(txt,"%g",val);
  else {
    if (val==min) sprintf(txt,"%g %s",val,unit); 
    else sprintf(txt,"%g",val);
  }
  return txt;
}


char *label_keylog(double val,double min,double max,void *data) {
  char *txt=NULL;
  char *unit=NULL;
  if (data !=NULL) unit=(char *)data;

  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (unit==NULL) {
    if (val==max) sprintf(txt,"%g",pow(10,val));
    if (val==min) sprintf(txt,"%g",pow(10,val)); 
  } else {
    if (val==max) sprintf(txt,"%g",pow(10,val));
    if (val==min) sprintf(txt,"%g %s",pow(10,val),unit); 
  }
  return txt; 
}


struct PolygonData *wrap(struct PolygonData *src) {
  struct PolygonData *dst;
  dst=MapWraparound(src);
  PolygonFree(src);
  return dst;
}

int main(int argc,char *argv[]) {

  int s=0;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="geo_simage";
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
  float pad=-1;
  float apad=22;
  float khgt=80;
  float kwdt=8;

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
  char *key_fname=NULL;
  char *key_unit=NULL;
  unsigned char *keyaflg=0;
  float kstp=0;

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
  unsigned char tmerc=0;
  unsigned char cylind=0;
 
  float lat=1e10,lon=0;
  float latmin=50.0;
  float sf=1.0;

  unsigned char magflg=0;
  unsigned char rotflg=0;

  unsigned char mapflg=0;
  unsigned char fmapflg=0;
  unsigned char bndflg=0;
  unsigned char grdflg=0;
  unsigned char igrdflg=0;
  unsigned char tmkflg=0;

  unsigned char frmflg=0;
  unsigned char ovrflg=0; 

  int tmtick=3;

  unsigned char lstflg=0;
  unsigned char trmflg=0;
  unsigned char ftrmflg=0;
  float tmin=0.5;
  float tscl=10.0;

  char *grdcol_txt=NULL;
  char *igrdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *lndcol_txt=NULL;
  char *seacol_txt=NULL;
  char *trmcol_txt=NULL;
  char *tmkcol_txt=NULL;

  unsigned int grdcol;
  unsigned int igrdcol;
  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;
  unsigned int tmkcol;



  FILE *mapfp;
  float marg[4];
  int i;

  int iwdt,ihgt;
  int x,y;
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;
  int rv,gv,bv;
  unsigned char addflg=0;
  float tf=1.0;
  float ratio;
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

  float grdlat=10;
  float grdlon=15;

  float igrdlat=10;
  float igrdlon=15;

  unsigned char grdtop=0;
  unsigned char igrdtop=0;

  unsigned char keyflg=0;
  unsigned char tmeflg=0;
  unsigned char tmhdr=0;
  unsigned char tlblflg=0;

  double LsoT,LT,Hangle,dec,eqt,mlon;
  double Z; 
 
  char tsfx[16];


  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

  for (i=0;i<256;i++) gry[i]=i;

  bgcol=RplotColor(0xff,0xff,0xff,0xff);
  txtcol=RplotColor(0x00,0x00,0x00,0xff);

  grdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  igrdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  cstcol=RplotColor(0xa0,0xa0,0xa0,0xff);
  bndcol=RplotColor(0x80,0x80,0x80,0xff);
  lndcol=RplotColor(0xff,0xff,0xf0,0xff);
  seacol=RplotColor(0xe0,0xf0,0xff,0xff);
  tmkcol=RplotColor(0x00,0x00,0x00,0xff);
  trmcol=RplotColor(0x80,0x80,0x80,0xff);

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


  OptionAdd(&opt,"log",'x',&lflg);
  OptionAdd(&opt,"stretch",'x',&sflg);

  OptionAdd(&opt,"inx",'i',&inx);

  OptionAdd(&opt,"min",'f',&min);
  OptionAdd(&opt,"max",'f',&max);

  OptionAdd(&opt,"tf",'f',&tf);
  OptionAdd(&opt,"add",'x',&addflg);


  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"key",'t',&key_fname);

  OptionAdd(&opt,"square",'x',&sqflg);

  OptionAdd(&opt,"ortho",'x',&ortho);
  OptionAdd(&opt,"stereo",'x',&stereo);
  OptionAdd(&opt,"gvp",'x',&gvp);
  OptionAdd(&opt,"tmerc",'x',&tmerc);

  OptionAdd(&opt,"lat",'f',&lat);
  OptionAdd(&opt,"lon",'f',&lon);
  OptionAdd(&opt,"latmin",'f',&latmin);
  OptionAdd(&opt,"sf",'f',&sf);
  OptionAdd(&opt,"mag",'x',&magflg);
  OptionAdd(&opt,"rotate",'x',&rotflg);
  OptionAdd(&opt,"flip",'x',&flip);

  OptionAdd(&opt,"coast",'x',&mapflg);
  OptionAdd(&opt,"fcoast",'x',&fmapflg);
  OptionAdd(&opt,"bnd",'x',&bndflg);
  OptionAdd(&opt,"grd",'x',&grdflg);

  OptionAdd(&opt,"grdlat",'f',&grdlat);
  OptionAdd(&opt,"grdlon",'f',&grdlon);

  OptionAdd(&opt,"igrd",'x',&igrdflg);
  OptionAdd(&opt,"igrdlat",'f',&igrdlat);
  OptionAdd(&opt,"igrdlon",'f',&igrdlon);

  OptionAdd(&opt,"grdontop",'x',&grdtop);
  OptionAdd(&opt,"igrdontop",'x',&igrdtop);

  OptionAdd(&opt,"tmk",'x',&tmkflg);

  OptionAdd(&opt,"tmtick",'i',&tmtick);
  OptionAdd(&opt,"lst",'x',&lstflg);
  OptionAdd(&opt,"term",'x',&trmflg);

  OptionAdd(&opt,"fterm",'x',&ftrmflg);
  OptionAdd(&opt,"tmin",'f',&tmin);
  OptionAdd(&opt,"tscl",'f',&tscl);

  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"igrdcol",'t',&igrdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"lndcol",'t',&lndcol_txt);
  OptionAdd(&opt,"seacol",'t',&seacol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
  OptionAdd(&opt,"tmkcol",'t',&tmkcol_txt);
 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"keyp",'x',&keyflg);
  OptionAdd(&opt,"keyu",'t',&key_unit);
  OptionAdd(&opt,"kstp",'f',&kstp);
  OptionAdd(&opt,"keya",'x',&keyaflg);

  OptionAdd(&opt,"tmlbl",'x',&tlblflg);
  OptionAdd(&opt,"time",'x',&tmeflg);
  OptionAdd(&opt,"tmhdr",'x',&tmhdr);

  OptionAdd(&opt,"fontname",'t',&fontname);
  OptionAdd(&opt,"fontsize",'f',&fontsize);
  OptionAdd(&opt,"lnewdt",'f',&width);

  OptionAdd(&opt,"frame",'x',&frmflg);
  OptionAdd(&opt,"over",'x',&ovrflg);

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

  s=SImageFread(fp,&simage);
  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
  fclose(fp);

  num=simage.colors;

  if (inx<0) inx=0;
  if (inx>=num) inx=num-1;


  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);
 
  if (tval==-1) tval= ( (int) simage.st_time % (24*3600));
  if (dval==-1) tval+=simage.st_time - ( (int) simage.st_time % (24*3600));
  else tval+=dval;
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  if (lat>90) {
    if (simage.c.glat<0) lat=-90;
    else lat=90;
  }

  if (magflg) {
    PolygonModify(map,AACGMtransform,NULL);
    PolygonModify(bnd,AACGMtransform,NULL);
  }

  if (tmtick<1) tmtick=1;
  if (tmtick>6) tmtick=6;

  if ((ortho==0) && (stereo==0)) cylind=1;

  if (grdflg) grd=make_grid(grdlon,grdlat,cylind);   
  if (igrdflg) igrd=make_grid(igrdlon,igrdlat,cylind);   

  if (tmkflg) tmk=make_grid(30*tmtick,10,cylind);

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  if (trmflg) {
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
  if (tmerc)  tfunc=MapTransverseMercator;

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
  if (igrdflg) {
       nigrd=PolygonTransform(igrd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
       pigrd=PolygonClip(clip,nigrd);
       PolygonFree(igrd);
       PolygonFree(nigrd);
  }

  if (trmflg) {
     ntrm=PolygonTransform(trm,2*sizeof(float),PolygonXYbbox,
                           tfunc,marg);
     if (cylind) ntrm=wrap(ntrm);
     ptrm=PolygonClip(clip,ntrm); 
  }

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

  if ((min==1e6) && (simage.min !=NULL)) min=simage.min[inx];
  if ((max==-1e6)&& (simage.max !=NULL)) max=simage.max[inx];
  if ((lflg) && (min<=0)) min=1;
  if (lflg) {
    max=log10(max);
    min=(int) log10(min)+max-(int) max;
  }

  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=RplotColorStringRGB(txtcol_txt);
  if (grdcol_txt !=NULL) grdcol=RplotColorStringRGB(grdcol_txt);
  if (igrdcol_txt !=NULL) igrdcol=RplotColorStringRGB(igrdcol_txt);
  if (cstcol_txt !=NULL) cstcol=RplotColorStringRGB(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=RplotColorStringRGB(bndcol_txt);
  if (lndcol_txt !=NULL) lndcol=RplotColorStringRGB(lndcol_txt);
  if (seacol_txt !=NULL) seacol=RplotColorStringRGB(seacol_txt);
  if (trmcol_txt !=NULL) trmcol=RplotColorStringRGB(trmcol_txt);
  if (tmkcol_txt !=NULL) tmkcol=RplotColorStringRGB(tmkcol_txt);

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



  apad=10+fontsize;
  if (pad==-1) {
    if (tlblflg) pad=10+fontsize;
    if (wdt>400) {
     if (tlblflg==0) pad=12;
     else pad=10+fontsize;
     apad=20+fontsize;
    } else pad=6;
  }

  if (wdt>400) {
     khgt=160*wdt/800;
     kwdt=20*wdt/800;
  } else {
    khgt=80*wdt/320;
  }
  
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

 
  image=SImageMap(iwdt,ihgt,magflg,tfunc,
                  marg,&simage,inx);


  if (ftrmflg) {
    if (cylind) {
      marg[1]=0;
      marg[2]=0;
    }
    zenith=SZAMap(yr,mo,dy,hr,mt,sc,-90.0,
                   iwdt,ihgt,magflg,tfunc,marg);   
  }
  
  RplotMakePlot(fbrplot,simage.source,iwdt,ihgt,24);
  if (!ovrflg) RplotRectangle(fbrplot,NULL,0,0,iwdt,ihgt,1,bgcol,0x0f,0,NULL);
  if (fmapflg) {
      if ((cylind) || (sqflg)) RplotRectangle(fbrplot,NULL,0,0,iwdt,ihgt,
                               1,seacol,0x0f,0,NULL);
      RplotEllipse(fbrplot,NULL,iwdt/2,ihgt/2,iwdt/2,ihgt/2,
                  1,seacol,0x0f,0,NULL);
  }

  if ((grdflg) && (grdtop==0)) {
    MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }

  if ((igrdflg) && (igrdtop==0)) {
    MapPlotPolygon(fbrplot,NULL,0,0,iwdt,ihgt,0,
                                igrdcol,0x0f,width,NULL,
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
      if (ftrmflg) {

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
      if ((lflg) && (image[y*iwdt+x]>0)) c=log10(image[y*iwdt+x]);
      else c=image[y*iwdt+x];
      if (c<-1e16) continue;
      if (c>max) c=max;
      if ((sflg) || (lflg)) c=(c-min)/(max-min);
      else c=c/max;

      if (c<0.0) c=0.0;
      if (c>1.0) c=1.0;

      rv=r[y*iwdt+x];
      gv=g[y*iwdt+x];
      bv=b[y*iwdt+x];

      ci=(key.num-1)*c;
     
      if (addflg) {
        if (key.r !=NULL) rv=rv*(1-c*tf)+tf*key.r[ci];
        if (key.g !=NULL) gv=gv*(1-c*tf)+tf*key.g[ci];
        if (key.b !=NULL) bv=bv*(1-c*tf)+tf*key.b[ci];
      } else {
        ratio=(1-tf)+c*tf;
        if (key.r !=NULL) rv=rv*(1-ratio)+ratio*key.r[ci];
        if (key.g !=NULL) gv=gv*(1-ratio)+ratio*key.g[ci];
        if (key.b !=NULL) bv=bv*(1-ratio)+ratio*key.b[ci];
      }
      if (rv<0) rv=0;
      if (gv<0) gv=0;
      if (bv<0) bv=0;
      if (rv>255) rv=255;
      if (gv>255) gv=255;
      if (bv>255) bv=255;
 
      r[y*iwdt+x]=rv;
      g[y*iwdt+x]=gv;
      b[y*iwdt+x]=bv;

    }
  }

  RplotMakePlot(rplot,simage.source,wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);

  if (!ovrflg) RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
  
  if (pflg) {
    struct RplotMatrix m;
    m.a=(wdt-2*pad)/img->wdt;
    m.b=0;
    m.c=0;
    m.d=(hgt-2*pad)/img->hgt;
    RplotImage(rplot,&m,img,0x0f,pad,pad,0);
  } else RplotImage(rplot,NULL,img,0x0f,pad,pad,0);
  
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

  if (bndflg) MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                bndcol,0x0f,width,NULL,
                                pbnd,-1);

  if (trmflg) MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                trmcol,0x0f,width,NULL,
                                ptrm,1);


  if ((grdflg) && (grdtop==1)) {
    MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }

  if ((igrdflg) && (igrdtop==0)) {
    MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,width,NULL,
                                pgrd,1);
  }

  if (tmkflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                tmkcol,0x0f,width,NULL,
                                ptmk,1);
  if (frmflg) {
    if ((cylind) || (sqflg)) 
       RplotRectangle(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
               0,grdcol,0x0f,width,NULL);
    else RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
               0,grdcol,0x0f,width,NULL);
  }
  if (tmeflg) {
       if (tmhdr) plot_time(rplot,5,5,wdt-10,hgt-10,simage.st_time,
                        simage.ed_time,txtcol,0x0f,fontname,fontsize,fontdb);
       else plot_time(rplot,5,5,wdt-10,hgt-10,tval,tval,
                        txtcol,0x0f,fontname,fontsize,fontdb);

  }
  if (keyflg) {
    if (lflg) {
      float lmt[2];
      lmt[0]=min;
      lmt[1]=max;
      if (khgt<80) PlotStdKeyLog(rplot,2,apad,kwdt,khgt,
 	   	          min,max,1,5.0,
                          0,0,2,
                          0,NULL,
                          txtbox,fontdb,label_keylog,key_unit,
			  fontname,fontsize,txtcol,0x0f,width,
                           key.num,key.r,key.g,key.b);

      else  PlotStdKeyLog(rplot,2,apad,kwdt,khgt,
 		   min,max,1,10.0,0,0,2,0,NULL,txtbox,
                   fontdb,label_keylog,key_unit,
		   fontname,fontsize,txtcol,0x0f,width,
                   key.num,key.r,key.g,key.b);
    } else {
      min=min*sflg;
      if (kstp==0) {
        if (khgt<80) kstp=(max-min)/5.0;
        else kstp=(max-min)/10.0;
      }
      if (keyaflg) PlotStdKey(rplot,2,apad,kwdt,khgt,
 		   min,max,kstp,0,0,2,0,NULL,txtbox,
                   fontdb,label_keyall,key_unit,
		   fontname,fontsize,txtcol,0x0f,width,
                   key.num,key.r,key.g,key.b);
      else PlotStdKey(rplot,2,apad,kwdt,khgt,
 		   min,max,kstp,0,0,2,0,NULL,txtbox,
                   fontdb,label_key,key_unit,
		   fontname,fontsize,txtcol,0x0f,width,
                   key.num,key.r,key.g,key.b);
    }
  }

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


