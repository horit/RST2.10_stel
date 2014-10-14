/* map_plot.c
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

#include "rmath.h"
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
#include "rtime.h"
#include "rmap.h"
#include "contour.h"
#include "rplotgraw.h"
#include "rplotgstd.h"
#include "aacgm.h"

#include "rfile.h"
#include "radar.h" 

#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvgrid.h"
#include "cnvmapsolve.h"
#include "crdshft.h"

#include "key.h"
#include "text_box.h"
#include "plot_time.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"

#include "plot_fit.h"
#include "plot_cell.h"
#include "plot_raw.h"
#include "plot_model.h"
#include "plot_maxmin.h"
#include "plot_model_name.h"
#include "plot_imf.h"
#include "plot_imf_delay.h"
#include "plot_excluded.h"
#include "plot_logo.h"
#include "plot_extra.h"
#include "plot_source.h"
#include "plot_chi.h"
#include "plot_time_label.h"
#include "plot_vec.h"
#include "make_pgrid.h"
#include "make_vgrid.h"
#include "make_hmb.h"
#include "make_fov.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: map_plot.c,v $
 Revision 1.11  2006/03/14 21:27:17  barnes
 Modification to deal with Radars with extra beams.

 Revision 1.10  2005/08/18 16:01:50  barnes
 Fixed bug in naming the plot in stdout mode.

 Revision 1.9  2005/08/10 22:51:03  barnes
 Added option to dump the command line options.

 Revision 1.8  2004/10/13 15:38:14  barnes
 Moved to DataMap format.

 Revision 1.7  2004/06/21 22:37:11  barnes
 Added missing header.

 Revision 1.6  2004/06/10 23:53:31  barnes
 Fixed bug in writing to standard output.

 Revision 1.5  2004/06/09 15:02:08  barnes
 Fixed bug in dealing stdout plots.

 Revision 1.4  2004/06/09 14:31:19  barnes
 Added the ability to output just one frame to standard output.

 Revision 1.3  2004/05/18 22:45:09  barnes
 Modifications to give the user finer control over the plot.

 Revision 1.2  2004/05/10 22:10:30  barnes
 Removed redundant code.

 Revision 1.1  2003/03/27 16:18:08  barnes
 Initial revision

*/

char *fsfx[]={"xml","ppm","ps",0};

unsigned char gry[256];

struct key vkey;
struct key xkey;

struct OptionData opt;
struct OptionFile *optf=NULL;

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;
struct PolygonData *rmap; 

struct PolygonData *bnd;
struct PolygonData *nbnd;
struct PolygonData *pbnd;
struct PolygonData *rbnd;


struct PolygonData *grd;
struct PolygonData *ngrd;
struct PolygonData *pgrd;
struct PolygonData *rgrd;

struct PolygonData *igrd;
struct PolygonData *nigrd;
struct PolygonData *pigrd;
struct PolygonData *rigrd;

struct PolygonData *tmk;
struct PolygonData *ntmk;
struct PolygonData *ptmk;
struct PolygonData *rtmk;

struct PolygonData *hmb;
struct PolygonData *nhmb;
struct PolygonData *phmb;

struct PolygonData *fov;
struct PolygonData *nfov;
struct PolygonData *pfov;
struct PolygonData *rfov;

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *clip;

struct FrameBufferClip *fbclp=NULL;

struct GridData rgrid;
struct GridData rgridmrg;
struct GridData rgridavg;

struct CnvMapData  rcmap;

struct CnvGrid pgrid;
struct CnvGrid vgrid;

double *zbuffer=NULL;
int zwdt,zhgt;
struct PolygonData **ctr;
struct PolygonData **nctr;
int cnum=24;
double cval[30]={-3000,-9000,-15000,-21000,-27000,-33000,-39000,-45000,-51000,
		 -57000,-63000,-70000,
	          3000,9000,15000,21000,27000,33000,39000,45000,51000,57000,
                63000,70000};



struct RadarNetwork *network=NULL;


int circle_clip(struct Rplot *rplot,
                float xoff,float yoff,float wdt,float hgt) {
  int i;
  float sf=0.55;
  float px[12];
  float py[12];
  int t[12];

  px[0]=wdt;
  py[0]=0;
  px[1]=wdt;
  py[1]=hgt*sf;
  px[2]=wdt*sf;
  py[2]=hgt;
  px[3]=0;
  py[3]=hgt;
  px[4]=-wdt*sf;
  py[4]=hgt;
  px[5]=-wdt;
  py[5]=hgt*sf;
  px[6]=-wdt;
  py[6]=0;
  px[7]=-wdt;
  py[7]=-hgt*sf;
  px[8]=-wdt*sf;
  py[8]=-hgt;
  px[9]=0;
  py[9]=-hgt;
  px[10]=wdt*sf;
  py[10]=-hgt;
  px[11]=wdt;
  py[11]=-hgt*sf;

  for (i=0;i<12;i++) {
    t[i]=1;
    px[i]+=xoff;
    py[i]+=yoff;
  }
  return RplotClip(rplot,12,px,py,t);
}

int square_clip(struct Rplot *rplot,
                float xoff,float yoff,float wdt,float hgt) {
  
  float px[4];
  float py[4];
  int t[4]={0,0,0,0};

  px[0]=xoff;
  py[0]=yoff;
  px[1]=xoff+wdt;
  py[1]=yoff;
  px[2]=xoff+wdt;
  py[2]=yoff+hgt;
  px[3]=xoff;
  py[3]=yoff+hgt;
  return RplotClip(rplot,4,px,py,t);
}



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
  fwrite(buf,sze,1,fp);
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


int rotate(int ssze,void *src,int dsze,void *dst,void *data) {

  float *arg=NULL;
  float *pnt;
  float px,py;
  float rad;
  arg=(float *) data;
  if (arg[0] > 0) rad=arg[1]*PI/180.0;
  else rad=-arg[1]*PI/180;
 
  pnt=(float *) src;
  px=pnt[0];
  py=pnt[1];
  pnt=(float *) dst;
  
  pnt[0]=0.5+(px-0.5)*cos(rad)-(py-0.5)*sin(rad);
  pnt[1]=0.5+(px-0.5)*sin(rad)+(py-0.5)*cos(rad);
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
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return (double) hr*3600L+mn*60L;
}   

double *render_map(struct CnvGrid *ptr,int *wdt,int *hgt) {
  int i;
  double *zbuffer=NULL;

  zbuffer=malloc(sizeof(double)*ptr->num);
  if (zbuffer==NULL) return NULL;
  
  *wdt=ptr->nlon;
  *hgt=ptr->nlat;

  for (i=0;i<ptr->num;i++) {
    zbuffer[i]=ptr->mag[i];
    
  }
  return zbuffer;
}    


int contour_convert(int ssze,void *src,int dsze,void *dst,void *data) {
  double latmin;
  float lat,lon;
  float *spnt;
  float *dpnt;
  spnt=(float *)src;
  dpnt=(float *)dst;
  latmin=*(double *)data;
  
  lon=spnt[0]*360.0;
  if (latmin>0)  lat=spnt[1]*(90.0-latmin)+latmin;
  else lat=-spnt[1]*(90.0+latmin)+latmin;
 
  dpnt[0]=lat;
  dpnt[1]=lon;

  return 0;

}

unsigned int mag_color(double v,void *data) {
  struct key *key;
  int i;

  key=(struct key *) data;
  if (key->num==0) return key->defcol;
  i=key->num*fabs(v)/key->max;
  if (i>=key->num) i=key->num-1;
  return (key->r[i]<<16) | (key->g[i]<<8) | key->b[i];
}

char *label_vel(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g m/s",val); 
  return txt;
}

char *label_wdt(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g m/s (sw)",val); 
  return txt;
}


char *label_pwr(double val,double min,double max,void *data) {
  char *txt=NULL;
  if ((val !=max) && (val !=min)) return NULL;
  txt=malloc(32); 
  if (val==max) sprintf(txt,"%g",val);
  if (val==min) sprintf(txt,"%g dB (pwr)",val); 
   
  return txt;
}



int calc_degfree(struct CnvMapData *mptr,struct GridData *gptr) {
  int degfree=0,i;
  double vel_max=2000;
  double mlat,mlon,tmp; 
  for (i=0;i<gptr->vcnum;i++) {

    if (gptr->data[i].st_id==-1) continue;
    mlat=gptr->data[i].mlat;
    mlon=gptr->data[i].mlon;
    tmp=gptr->data[i].azm;
       
    if ((mptr->lat_shft !=0) || (mptr->lon_shft !=0)) 
    CnvMapCrdShft(&mlat,&mlon,&tmp,mptr->lat_shft,mptr->lon_shft);
       
    if (fabs(mlat) < fabs(mptr->latmin)) continue;
    if (fabs(gptr->data[i].vel.median)>vel_max) continue;


    degfree++;
  }
  return degfree;
}

int calc_degfree_model(struct CnvMapData *mptr,struct GridData *gptr) {
  int degfree=0,i; 
  for (i=0;i<mptr->num_model;i++) {
    if (mptr->model[i].vel.median !=1) degfree+=2;
    else degfree++;
  }
  degfree-=mptr->num_coef;
  return degfree;
}

int main(int argc,char *argv[]) {

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  char filename[256];
  FILE *outfp=NULL;
  unsigned char dnflg=0;
  unsigned char tnflg=0;
  unsigned char mnflg=0;
  unsigned char stdioflg=0;
  char *sfx=NULL;
  int cnt=0;

  int s=0;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp=NULL;
  struct XwinWindow *win=NULL;
  char *display_name=NULL;
  char *wname="map_plotxt";
  int xdoff=-1;
  int ydoff=-1;
  struct timeval tmout;
  float delay=0.1;
  int xstat=0;

#endif

  struct RfileIndex *oinx=NULL;
  struct CnvMapIndex *inx=NULL;

  struct FrameBuffer *img=NULL;
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
  char *fname=NULL;
  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;
 
  float wdt=540,hgt=540;
  float pad=-1;

  float khgt=80;
  float kwdt=44;
  float apad=22;
  float px=2;

  float imfx,imfy,imfr;
  float imfmx=10;


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
  char *vkey_fname=NULL;
  char *xkey_fname=NULL;
  FILE *keyfp=NULL;

  MapTransform  tfunc;

  unsigned char sqflg=0;

  unsigned char flip=0;
  unsigned char ortho=0;
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

  int tmtick=3;

  unsigned char lstflg=0;

  unsigned char trmflg=0;
  unsigned char ftrmflg=0;

  unsigned char defflg=0;

  char *grdcol_txt=NULL;
  char *igrdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *lndcol_txt=NULL;
  char *seacol_txt=NULL;
  char *ftrmcol_txt=NULL;
  char *trmcol_txt=NULL;
  char *tmkcol_txt=NULL;

  char *veccol_txt=NULL;
  char *ctrcol_txt=NULL;
  char *hmbcol_txt=NULL;
  char *fovcol_txt=NULL;
  char *ffovcol_txt=NULL;

  unsigned int grdcol;
  unsigned int igrdcol;
  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;
  unsigned int ftrmcol;
  unsigned int tmkcol;


  unsigned int veccol;
  unsigned int ctrcol;
  unsigned int hmbcol;
  unsigned int fovcol;
  unsigned int ffovcol;

  FILE *mapfp;
  float marg[4];
  int i;

  int flg=0;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double sdte=-1;
  double edte=-1;

  double ssec=-1; /* specified start time */
  double esec=-1; /* end time */
  double exsec=0;


  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  double tval=0;
  float tme_shft;

  float grdlat=10;
  float grdlon=15;

  float igrdlat=10;
  float igrdlon=15;

  unsigned char grdtop=0;
  unsigned char igrdtop=0;

  unsigned char vkeyflg=0;
  unsigned char xkeyflg=0;

  unsigned char vecflg=0;  
 
  unsigned char fitflg=0;
  unsigned char mrgflg=0;
  unsigned char rawflg=0;
  unsigned char modflg=0;
  unsigned char ctrflg=0;
  unsigned char hmbflg=0;
  unsigned char fovflg=0;
  unsigned char ffovflg=0;

  unsigned char excflg=0;

  unsigned char pwrflg=0;
  unsigned char wdtflg=0;

  unsigned char tlblflg=0;
  unsigned char modnflg=0;
  unsigned char imfflg=0;
  unsigned char potflg=0;
  unsigned char extflg=0;
  unsigned char logoflg=0;
  unsigned char srcflg=0;
  unsigned char tmeflg=0;

  unsigned char maxflg=0;
  unsigned char minflg=0;
  unsigned char avgflg=0;

  unsigned char avflg=0;
  int aval=0;
 
  unsigned char celflg=0;
  int cprm=0;

  double pmax=30;
  double wmax=500;
  double vmax=1000;
  int degfree=0;
  int degfree_dat=0;

  float vsf=2.0;


  unsigned char poleflg=0;
 
  unsigned char frmflg=0; 
  unsigned char ovrflg=0; 

  float lnewdt=0.5;
 
  char txt[256];
  char tsfx[16];

  struct RplotDash *cdash=NULL;

  char *dfpath=".";
  char *fpath=NULL;
  char *postp=NULL;


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

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

 
  cdash=RplotMakeDashString("12 6");

  for (i=0;i<256;i++) gry[i]=i;

  bgcol=RplotColor(0xff,0xff,0xff,0xff);
  txtcol=RplotColor(0x00,0x00,0x00,0xff);

  grdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  igrdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  cstcol=RplotColor(0xa0,0xa0,0xa0,0xff);
  bndcol=RplotColor(0x80,0x80,0x80,0xff);
  lndcol=RplotColor(0xff,0xff,0xf0,0xff);
  seacol=RplotColor(0xe0,0xf0,0xff,0xff);
  trmcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  ftrmcol=RplotColor(0xe0,0xe0,0xe0,0xff);

  tmkcol=RplotColor(0x00,0x00,0x00,0xff);

  veccol=RplotColor(0x00,0x00,0x00,0xff);
  ctrcol=RplotColor(0x00,0x00,0x00,0xff);
  hmbcol=RplotColor(0x00,0x80,0x00,0xff);
  fovcol=RplotColor(0x00,0x00,0x00,0xff);
  ffovcol=RplotColor(0xc0,0xc0,0xc0,0xff);
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"cf",'t',&cfname);


#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
  OptionAdd(&opt,"delay",'f',&delay);

#endif

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);
  OptionAdd(&opt,"ps",'x',&psflg); 

  OptionAdd(&opt,"path",'t',&fpath);
  OptionAdd(&opt,"post",'t',&postp);
  OptionAdd(&opt,"tn",'x',&tnflg);
  OptionAdd(&opt,"dn",'x',&dnflg); 
  OptionAdd(&opt,"mn",'x',&mnflg); 

  OptionAdd(&opt,"stdout",'x',&stdioflg); 

  
  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"pad",'f',&pad);
  OptionAdd(&opt,"lnewdt",'f',&lnewdt);


  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

  OptionAdd(&opt,"t",'t',&stmestr);
  OptionAdd(&opt,"d",'t',&sdtestr);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"vkey",'t',&vkey_fname);
  OptionAdd(&opt,"xkey",'t',&xkey_fname);

   OptionAdd(&opt,"square",'x',&sqflg);

  OptionAdd(&opt,"ortho",'x',&ortho);
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

  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"igrdcol",'t',&igrdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"lndcol",'t',&lndcol_txt);
  OptionAdd(&opt,"seacol",'t',&seacol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
  OptionAdd(&opt,"ftrmcol",'t',&ftrmcol_txt);


  OptionAdd(&opt,"tmkcol",'t',&tmkcol_txt);

  OptionAdd(&opt,"veccol",'t',&veccol_txt);
  OptionAdd(&opt,"ctrcol",'t',&ctrcol_txt);
  OptionAdd(&opt,"hmbcol",'t',&hmbcol_txt);
  OptionAdd(&opt,"fovcol",'t',&fovcol_txt);
  OptionAdd(&opt,"ffovcol",'t',&ffovcol_txt);


  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"mrg",'x',&mrgflg);
  OptionAdd(&opt,"raw",'x',&rawflg);
  OptionAdd(&opt,"mod",'x',&modflg);
  OptionAdd(&opt,"ctr",'x',&ctrflg);
  OptionAdd(&opt,"hmb",'x',&hmbflg);
  OptionAdd(&opt,"fov",'x',&fovflg);
  OptionAdd(&opt,"ffov",'x',&ffovflg);


  OptionAdd(&opt,"exc",'x',&excflg);

  OptionAdd(&opt,"pwr",'x',&pwrflg);
  OptionAdd(&opt,"swd",'x',&wdtflg);
  
  OptionAdd(&opt,"avg",'x',&avgflg);
  OptionAdd(&opt,"max",'x',&maxflg);
  OptionAdd(&opt,"min",'x',&minflg);
 
  OptionAdd(&opt,"vkeyp",'x',&vkeyflg);
  OptionAdd(&opt,"xkeyp",'x',&xkeyflg);

  OptionAdd(&opt,"vecp",'x',&vecflg);
  OptionAdd(&opt,"vsf",'f',&vsf);
 
  OptionAdd(&opt,"tmlbl",'x',&tlblflg);
  OptionAdd(&opt,"modn",'x',&modnflg);
  OptionAdd(&opt,"imf",'x',&imfflg);
  OptionAdd(&opt,"pot",'x',&potflg);

  OptionAdd(&opt,"extra",'x',&extflg);
  OptionAdd(&opt,"source",'x',&srcflg);
  OptionAdd(&opt,"logo",'x',&logoflg);
  OptionAdd(&opt,"time",'x',&tmeflg);

  OptionAdd(&opt,"frame",'x',&frmflg);
  OptionAdd(&opt,"over",'x',&ovrflg);


  OptionAdd(&opt,"vmax",'d',&vmax);
  OptionAdd(&opt,"pmax",'d',&pmax);
  OptionAdd(&opt,"wmax",'d',&wmax);

  OptionAdd(&opt,"def",'x',&defflg);

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

  old=!new;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  fname=argv[arg];

  if (old) {
    if (argc-arg>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp !=NULL) { 
         oinx=RfileLoadIndex(fp);
         fclose(fp);
      }
    }
  } else {
     if (argc-arg>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp !=NULL) { 
         inx=CnvMapIndexFload(fp);
         fclose(fp);
      }
    }
  }

  if (defflg) { /* set default plot */
    magflg=1;
    rotflg=1;
    fitflg=1;
    ctrflg=1;
    imfflg=1;
    potflg=1;
    hmbflg=1;
    modnflg=1;
    tmkflg=1;
    vkeyflg=1;
    vecflg=1;
    tmeflg=1;
  }

  if (exstr !=NULL) exsec=strtime(exstr);
  if (stmestr !=NULL) ssec=strtime(stmestr);
  if (etmestr !=NULL) esec=strtime(etmestr);
  if (sdtestr !=NULL) sdte=strdate(sdtestr);
  if (edtestr !=NULL) edte=strdate(edtestr);

  fp=fopen(fname,"r");

  if (old) s=OldCnvMapFread(fp,&rcmap,&rgrid);
  else s=CnvMapFread(fp,&rcmap,&rgrid);

  if (s !=-1) {
    if (ssec==-1) ssec=((int) rcmap.st_time % (24*3600));
    if (sdte==-1) ssec+=rcmap.st_time - ((int) rcmap.st_time % (24*3600));
    else ssec+=sdte;
    if (esec !=-1) {
      if (edte==-1) esec+=rcmap.st_time - ((int) rcmap.st_time % (24*3600));
      else esec+=edte;
    }
    if (exsec !=0) esec=ssec+exsec;
  }
  TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
  if (old) {
    if (rcmap.st_time<ssec) {
      s=OldCnvMapFseek(fp,yr,mo,dy,hr,mt,(int) sc,oinx,NULL);
      s=OldCnvMapFread(fp,&rcmap,&rgrid);
    }
  } else {
    if (rcmap.st_time<ssec) {
      s=CnvMapFseek(fp,yr,mo,dy,hr,mt,(int) sc,NULL,inx);
      s=CnvMapFread(fp,&rcmap,&rgrid);
    }
  }

  if (!sqflg) clip=MapCircleClip(10);
  else clip=MapSquareClip();

  if (lat>90) lat=90*rcmap.hemisphere;
  if (fovflg || ffovflg) fov=make_fov(rgrid.st_time,network); 
  if ((fovflg || ffovflg) && !magflg) PolygonModify(fov,AACGMtransform,&flg);

  if (grdflg) grd=make_grid(grdlon,grdlat);   
  if (igrdflg) igrd=make_grid(igrdlon,igrdlat);   

  if (tmtick<1) tmtick=1;
  if (tmtick>6) tmtick=6;
 
  if (tmkflg) tmk=make_grid(30*tmtick,10);

  if (magflg) {
    PolygonModify(map,AACGMtransform,NULL);
    PolygonModify(bnd,AACGMtransform,NULL);
    if (igrdflg) PolygonModify(igrd,AACGMtransform,NULL);
  } else {
    if (igrdflg) PolygonModify(igrd,AACGMtransform,marg);
  }

 
  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  marg[0]=lat;
  marg[1]=0;
  if (ortho) marg[2]=sf;
  else marg[2]=1.25*0.5*sf*90.0/(90-fabs(latmin));
  marg[3]=flip;

  tfunc=MapStereographic;
  if (ortho) tfunc=MapOrthographic;

  strcpy(tsfx,"LT");
  if (magflg) strcpy(tsfx,"MLT");
  else if (lstflg) strcpy(tsfx,"LST");

  if (lat==90) poleflg=1;
  if (lat==-90) poleflg=1;

 if (poleflg) {
    if (mapflg || fmapflg) {
      nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
                             tfunc,marg);
      pmap=PolygonClip(clip,nmap); 
      PolygonFree(map);
      PolygonFree(nmap);
    }
    if (bndflg) {
       nbnd=PolygonTransform(bnd,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
       pbnd=PolygonClip(clip,nbnd);
       PolygonFree(bnd);
       PolygonFree(nbnd);
    }
    if (grdflg) {
       ngrd=PolygonTransform(grd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
       pgrd=PolygonClip(clip,ngrd);
       PolygonFree(grd);
       PolygonFree(ngrd);
    }
    if (igrdflg) {
       nigrd=PolygonTransform(igrd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
       pigrd=PolygonClip(clip,nigrd);
       PolygonFree(igrd);
       PolygonFree(nigrd);
    }
    if (fovflg || ffovflg) {
       nfov=PolygonTransform(fov,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
       pfov=PolygonClip(clip,nfov);
       PolygonFree(fov);
       PolygonFree(nfov);
    }
    if (tmkflg) {
       ntmk=PolygonTransform(tmk,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
       ptmk=PolygonClip(clip,ntmk);
       PolygonFree(tmk);
       PolygonFree(ntmk);
    }
  }

  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=RplotColorStringRGB(txtcol_txt);

  if (grdcol_txt !=NULL) grdcol=RplotColorStringRGB(grdcol_txt);
  if (igrdcol_txt !=NULL) igrdcol=RplotColorStringRGB(igrdcol_txt);
  if (cstcol_txt !=NULL) cstcol=RplotColorStringRGB(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=RplotColorStringRGB(bndcol_txt);
  if (lndcol_txt !=NULL) lndcol=RplotColorStringRGB(lndcol_txt);
  if (seacol_txt !=NULL) seacol=RplotColorStringRGB(seacol_txt);
  if (trmcol_txt !=NULL) seacol=RplotColorStringRGB(trmcol_txt);
  if (trmcol_txt !=NULL) trmcol=RplotColorStringRGB(trmcol_txt);
  if (ftrmcol_txt !=NULL) ftrmcol=RplotColorStringRGB(ftrmcol_txt);


  if (tmkcol_txt !=NULL) tmkcol=RplotColorStringRGB(tmkcol_txt);

  if (veccol_txt !=NULL) veccol=RplotColorStringRGB(veccol_txt);
  if (ctrcol_txt !=NULL) ctrcol=RplotColorStringRGB(ctrcol_txt);
  if (hmbcol_txt !=NULL) hmbcol=RplotColorStringRGB(hmbcol_txt);
  if (fovcol_txt !=NULL) fovcol=RplotColorStringRGB(fovcol_txt);
  if (ffovcol_txt !=NULL) ffovcol=RplotColorStringRGB(ffovcol_txt);

  if (vkey_fname !=NULL) {
    keyfp=fopen(vkey_fname,"r");
    if (keyfp !=NULL) {
      load_key(keyfp,&vkey);
      fclose(keyfp);
    }
  }
  vkey.max=vmax;
  vkey.defcol=veccol;

  if (xkey_fname !=NULL) {
    keyfp=fopen(xkey_fname,"r");
    if (keyfp !=NULL) {
      load_key(keyfp,&xkey);
      fclose(keyfp);
    }
  }

  if (pwrflg) xkey.max=pmax;
  else xkey.max=wmax;

  if (pwrflg || wdtflg) {
     celflg=1;
     if (pwrflg) cprm=0;
     else cprm=2;
  }

  if (avgflg || maxflg || minflg) {
     avflg=1;
     if (maxflg==1) aval=1;
     if (minflg==1) aval=4;
  }

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }

  if (pad==-1) {
    if (tlblflg) pad=22;
    if (wdt>400) {
       if (tlblflg==0) pad=12;
       else pad=20;
       apad=30;
    } else pad=6;
  }

  if (wdt<400) khgt=80*wdt/320;

  imfr=wdt/16;
  imfx=wdt-imfr-16;
  imfy=imfr+apad;
  
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

  sfx=fsfx[0];
  if (gflg) sfx=fsfx[1];
  if (pflg) sfx=fsfx[2];
  

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-hgt)/2;


    win=XwinMakeWindow(xdoff,ydoff,wdt,hgt,0,
                       dp,wname,
                       wname,argv[0],wname,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }
    XwinShowWindow(win);
  }
  #endif

  if (fpath==NULL) fpath=dfpath;

  do {

    if (mnflg) tval=(rcmap.st_time+rcmap.ed_time)/2;
    else tval=rcmap.st_time;
    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

    if (stdioflg) {
      sprintf(filename,"%.4d%.2d%.2d.%.2d%.2d.%.2d.rp.xml",
                       yr,mo,dy,hr,mt,(int) sc);
      if (pflg) PostScriptSetText(psdata,stream,stdout);
      if (!rflg) RplotSetText(rplot,stream,stdout);
    } else {
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",dy,mo,yr,hr,mt,(int) sc);    
      if (tnflg) sprintf(filename,"%s/%.2d%.2d.%.2d.%s",
                       fpath,hr,mt,(int) sc,sfx); 
      else if (dnflg) sprintf(filename,"%s/%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",
                       fpath,yr,mo,dy,hr,mt,(int) sc,sfx);
      else sprintf(filename,"%s/%.4d.%s",fpath,cnt,sfx);

      if (pflg) {
        outfp=fopen(filename,"w");
        PostScriptSetText(psdata,stream,outfp);
      }

      if (!rflg) {
        outfp=fopen(filename,"w");
        RplotSetText(rplot,stream,outfp);
      }
    }

    if (mrgflg) GridMerge(&rgrid,&rgridmrg);
    if (avflg) GridAverage(&rgrid,&rgridavg,aval+cprm*(aval !=0)); 
    
    /* do plotting here */
    if (rcmap.num_coef !=0) {
      make_pgrid(rcmap.latmin,&pgrid);
      make_vgrid(&rgrid,&vgrid); 
      CnvMapSolve(&rcmap,&pgrid);
      CnvMapSolve(&rcmap,&vgrid);
      zbuffer=render_map(&pgrid,&zwdt,&zhgt);
      ctr=Contour(zbuffer,contour_DOUBLE,zwdt,zhgt,cnum,cval,3,1,1,0x04);
      free(zbuffer);
    }
    if (hmbflg) hmb=make_hmb(&rcmap);
    if ((hmbflg) && (!magflg)) PolygonModify(hmb,AACGMtransform,&flg);
 
    if (trmflg || ftrmflg) {
        if (lat>0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,magflg,
                            1.0,90.0);
        if (lat<0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,-1,magflg,
                            1.0,90.0);
    }
    
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

  if (rotflg) marg[1]=lon+tme_shft; 
  else marg[1]=lon;
    if (poleflg) {
      if ((rotflg) && (flip)) marg[1]=-lon-tme_shft;
      if (pmap !=NULL) 
        rmap=PolygonTransform(pmap,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pbnd !=NULL) 
        rbnd=PolygonTransform(pbnd,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pgrd !=NULL) 
        rgrd=PolygonTransform(pgrd,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pigrd !=NULL) 
       rigrd=PolygonTransform(pigrd,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (pfov !=NULL) 
        rfov=PolygonTransform(pfov,2*sizeof(float),PolygonXYbbox,rotate,marg);
      if (ptmk !=NULL) {
        if (rotflg) marg[1]=0;
        else marg[1]=lon-tme_shft;
        rtmk=PolygonTransform(ptmk,2*sizeof(float),PolygonXYbbox,rotate,marg);
        if (rotflg) marg[1]=lon+tme_shft;
        else marg[1]=lon;
      }
      if ((rotflg) && (flip)) marg[1]=lon+tme_shft;
    } else {
 if (mapflg || fmapflg) {
        nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
				tfunc,marg);
	rmap=PolygonClip(clip,nmap);
	PolygonFree(nmap);
      }
      if (bndflg) {
        nbnd=PolygonTransform(bnd,2*sizeof(float),PolygonXYbbox,
	       		tfunc,marg);
	rbnd=PolygonClip(clip,nbnd);
        PolygonFree(nbnd);
      }
      if (grdflg) {
        ngrd=PolygonTransform(grd,2*sizeof(float),PolygonXYbbox,
	      		tfunc,marg);
	rgrd=PolygonClip(clip,ngrd);
	PolygonFree(ngrd);
      }
      if (igrdflg) {
        nigrd=PolygonTransform(igrd,2*sizeof(float),PolygonXYbbox,
	      		tfunc,marg);
	rigrd=PolygonClip(clip,nigrd);
	PolygonFree(nigrd);
      }
      if (fovflg || ffovflg) {
        nfov=PolygonTransform(fov,2*sizeof(float),PolygonXYbbox,
	      		tfunc,marg);
	rfov=PolygonClip(clip,nfov);
        PolygonFree(nfov);
      }
      if (tmkflg) {
        if (rotflg) marg[1]=0;
        else marg[1]=lon-tme_shft;
        ntmk=PolygonTransform(tmk,2*sizeof(float),PolygonXYbbox,
	      		tfunc,marg);
	rtmk=PolygonClip(clip,ntmk);
	PolygonFree(ntmk);
        if (rotflg) marg[1]=lon+tme_shft;
        else marg[1]=lon;
      }
      
    }
  


    ContourModify(cnum,ctr,contour_convert,&rcmap.latmin);
    if (!magflg) ContourModify(cnum,ctr,AACGMtransform,&flg);

    nctr=ContourTransform(cnum,ctr,tfunc,marg);
   
    if (hmbflg) {
       nhmb=PolygonTransform(hmb,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
       phmb=PolygonClip(clip,nhmb);
       PolygonFree(nhmb);
       PolygonFree(hmb);
       hmb=NULL;
       nhmb=NULL;
    }

    if (trmflg || ftrmflg) {
       ntrm=PolygonTransform(trm,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
       ptrm=PolygonClip(clip,ntrm);
       PolygonFree(ntrm);
       PolygonFree(trm);
       trm=NULL;
       ntrm=NULL;
       
    }

    RplotMakePlot(rplot,filename,wdt,hgt,24);
    RplotStartInfo(rplot);
    record_info(rplot,argc,argv);
    RplotEndInfo(rplot);

    if (!ovrflg) RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
    if (!sqflg) circle_clip(rplot,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad);
    else square_clip(rplot,pad,pad,wdt-2*pad,hgt-2*pad);

    if (fmapflg) {
      if (!sqflg) RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                 1,seacol,0x0f,0,NULL);
      else RplotRectangle(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                 1,seacol,0x0f,0,NULL);
    }

    if ((grdflg) && (grdtop==0)) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,0.5,NULL,
                                rgrd,1);
    }

    if ((igrdflg) && (igrdtop==0)) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,0.5,NULL,
                                rigrd,1);
    }

  
    if (fmapflg) {
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                     1,lndcol,0x0f,0,NULL,rmap,1);

      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                     1,lndcol,0x0f,0,NULL,rmap,3);
  
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                     1,seacol,0x0f,0,NULL,rmap,0);
    }
     
    if (ftrmflg) 
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
                               trmcol,0x0f,0.5,NULL,
                               ptrm,1);

    if (ffovflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,
			       ffovcol,0x0f,0.5,NULL,
			       rfov,1);


    if (celflg) {
      if (avflg) 
          plot_cell(rplot,&rgridavg,rcmap.latmin,magflg,pad,pad,wdt-2*pad,
                    hgt-2*pad,tfunc,marg,mag_color,&xkey,cprm);
      else 
         plot_cell(rplot,&rgrid,rcmap.latmin,magflg,pad,pad,wdt-2*pad,
                    hgt-2*pad,tfunc,marg,mag_color,&xkey,cprm);
        
    }

    if (mapflg) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    lnewdt,NULL,
                    rmap,1);
  
       MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,cstcol,0x0f,
                    lnewdt,NULL,
                    rmap,0);
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,cstcol,0x0f,
                    lnewdt,NULL,
                    rmap,2);

    }

    if (bndflg) MapPlotOpenPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                                bndcol,0x0f,lnewdt,NULL,
                                rbnd,-1);

    if (hmbflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                             hmbcol,0x0f,lnewdt,NULL,
                             phmb,1);
  
    if (fovflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                             fovcol,0x0f,0.5,NULL,
                             rfov,1);

    if (trmflg)
      MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
		     trmcol,0x0f,0.5,NULL,
		     ptrm,1);


    if ((grdflg) && (grdtop==1)) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,0.5,NULL,
                                rgrd,1);
    }

    if ((igrdflg) && (igrdtop==1)) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,0.5,NULL,
                                rigrd,1);
    }

    if (tmkflg) MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                               tmkcol,0x0f,0.5,NULL,
                               rtmk,1);

    
    if (fitflg) 
       plot_fit(rplot,&vgrid,rcmap.latmin,magflg,pad,pad,wdt-2*pad,hgt-2*pad,
               vsf,tfunc,marg,mag_color,&vkey,0.5);
     

   if (mrgflg) plot_raw(rplot,&rgridmrg,rcmap.latmin,magflg,
                 pad,pad,wdt-2*pad,hgt-2*pad,
                 vsf,tfunc,marg,mag_color,&vkey,lnewdt);
    
   if (rawflg) 
      plot_raw(rplot,&rgrid,rcmap.latmin,magflg,pad,pad,wdt-2*pad,hgt-2*pad,
               vsf,tfunc,marg,mag_color,&vkey,lnewdt);
    

    if (modflg) 
      plot_model(rplot,&rcmap,rcmap.latmin,magflg,pad,pad,wdt-2*pad,
                  hgt-2*pad,vsf,tfunc,marg,mag_color,&vkey,lnewdt);
     

    if (excflg) 
      plot_excluded(rplot,&vgrid,rcmap.latmin,magflg,pad,pad,wdt-2*pad,
                   hgt-2*pad,vsf,tfunc,marg,grdcol,lnewdt);
    

    if (ctrflg) {
      for (i=0;i<cnum/2;i++) {
        sprintf(txt,"%.2d",(int) cval[i]/1000);
        PlotRawContour(rplot,nctr[i],pad,pad,wdt-2*pad,hgt-2*pad,0.1,0.5,
                20,i,20,strlen(txt),txt,ctrcol,0x0f,lnewdt,
		NULL,"Helvetica",8.0,
                txtbox,fontdb);
               
      }

      for (i=cnum/2;i<cnum;i++) {
        sprintf(txt,"%.2d",(int) cval[i]/1000);
        PlotRawContour(rplot,nctr[i],pad,pad,wdt-2*pad,hgt-2*pad,0.1,0.5,
                20,i,20,strlen(txt),txt,ctrcol,0x0f,lnewdt,
		cdash,"Helvetica",8.0,
                txtbox,fontdb);
               
      }
      plot_maxmin(rplot,&pgrid,magflg,pad,pad,wdt-2*pad,hgt-2*pad,6.0,
               tfunc,marg,ctrcol,0x0f,2,NULL);
      
    }
 
    RplotClip(rplot,0,NULL,NULL,NULL);

    if (frmflg) {
      if (! sqflg) RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                              0,grdcol,0x0f,0.5,NULL);
      else RplotRectangle(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,
                              0,grdcol,0x0f,0.5,NULL);
    }
    if (tmeflg) plot_time(rplot,5,5,wdt-10,hgt-10,rcmap.st_time,rcmap.ed_time,
                           txtcol,0x0f,"Helvetica",12.0,fontdb);
    px=2;
    if ((vkeyflg) && (vkey.num !=0)) {
      double kstp;
      if (khgt<80) kstp=vmax/5.0;
      else kstp=vmax/10.0;
      PlotStdKey(rplot,px,apad,8,khgt,
 		 0,vmax,kstp,
                 0,0,2,
                 0,NULL,
                 txtbox,fontdb,label_vel,NULL,
		 "Helvetica",10.0,txtcol,0x0f,0.5,
                 vkey.num,vkey.r,vkey.g,vkey.b);
      px+=kwdt;
    }
    if ((xkeyflg) && (xkey.num !=0)) {
      double kstp,max;
      if (cprm==0) max=pmax;
      else max=wmax;
      if (khgt<80) kstp=max/5.0;
      else kstp=max/10.0;
      if (cprm==0) PlotStdKey(rplot,px,apad,8,khgt,
 		               0,pmax,kstp,
                               0,0,2,
                               0,NULL,
                               txtbox,fontdb,label_pwr,NULL,
			       "Helvetica",10.0,txtcol,0x0f,0.5,
                                xkey.num,xkey.r,xkey.g,xkey.b);
      else PlotStdKey(rplot,px,apad,8,khgt,
 		      0,pmax,kstp,
                      0,0,2,
                      0,NULL,
                      txtbox,fontdb,label_wdt,NULL,
		      "Helvetica",10.0,txtcol,0x0f,0.5,
                       xkey.num,xkey.r,xkey.g,xkey.b);

      px+=kwdt;
    }


    if (vecflg) {
      if (px==2) px+=10;
      plot_vec(rplot,px,1.8*apad,0,vmax,magflg,
                        pad,pad,wdt-2*pad,hgt-2*pad,
                        2.0,tfunc,marg,txtcol,0x0f,0.5,
                        "Helvetica",10.0,fontdb);
    }


    if (modnflg) plot_model_name(rplot,
                  imfx-imfr*0.1,
                  wdt-imfy,rcmap.imf_model[0],rcmap.imf_model[1],
                  txtcol,0x0f,"Helvetica",12.0,fontdb);


    if ((imfflg) && (rcmap.Bx !=0) && (rcmap.By !=0) && (rcmap.Bz !=0)) {
       RplotLine(rplot,imfx-imfr,imfy,imfx+imfr,imfy,grdcol,0x0f,0.5,NULL);
       RplotLine(rplot,imfx,imfy-imfr,imfx,imfy+imfr,grdcol,0x0f,0.5,NULL);
       RplotEllipse(rplot,NULL,imfx,imfy,imfr,imfr,0,grdcol,0x0f,0.5,NULL);
       plot_imf(rplot,imfx,imfy,imfr,rcmap.Bx,rcmap.By,rcmap.Bz,imfmx,txtcol,
              0x0f,0.5,"Helvetica",10.0,fontdb); 
       if (wdt>400) {
         plot_imf_delay(rplot,
                      imfx,imfy,imfr,rcmap.imf_delay,txtcol,0x0f,"Helvetica",
                      8.0,fontdb);
     
         RplotText(rplot,NULL,"Helvetica",10.0,imfx+imfr+2,imfy+4,     
               strlen("+Y"),"+Y",txtcol,0x0f,1);
         RplotText(rplot,NULL,"Helvetica",10.0,imfx-8,imfy-imfr-4,     
               strlen("+Z"),"+Z",txtcol,0x0f,1);
       }
    }
  

    if ((potflg) && (rcmap.coef !=NULL)) {
      char txt[256];
      sprintf(txt,"%.2d kV",(int) floor(rcmap.pot_drop/1000.0)); 
       if (wdt>400) RplotText(rplot,NULL,"Helvetica",14.0,
                            3*apad,apad,     
                            strlen(txt),txt,txtcol,0x0f,1);
       else RplotText(rplot,NULL,"Helvetica",10.0,
                            3.5*apad,apad,     
                            strlen(txt),txt,txtcol,0x0f,1);

    } 

    if ((extflg) && (rcmap.coef !=NULL)) {	
         if (wdt>400) {
           plot_extra(rplot,4,hgt-1.6*imfy,&rcmap,txtcol,0x0f,"Helvetica",10.0,
                    fontdb);
	 
          degfree_dat=calc_degfree(&rcmap,&rgrid);
          degfree=degfree_dat+calc_degfree_model(&rcmap,&rgrid);
     
          plot_chi(rplot,4,wdt-0.7*imfy,&rcmap,degfree,degfree_dat,
                 txtcol,0x0f,"Helvetica","Symbol",10.0,fontdb);
            
	 
	 
        }
      }
  
    if (srcflg==1) {
      if (wdt>400) plot_source(rplot,1.2*apad,hgt-0.6*imfy,
                               rcmap.source,rcmap.major_rev,
                               rcmap.minor_rev,txtcol,0x0f,"Helvetica",10.0,
                               fontdb);
      else plot_source(rplot,2,hgt-0.4*imfy,rcmap.source,
                       rcmap.major_rev,
                       rcmap.minor_rev,txtcol,0x0f,"Helvetica",10.0,fontdb);
    }
     
    if (logoflg==1) {
      plot_logo(rplot,4,4,wdt-8,wdt-18,txtcol,0x0f,"Helvetica",10.0,fontdb);
      plot_web(rplot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",10.0,fontdb);
      plot_credit(rplot,4,4,wdt-8,wdt-8,txtcol,0x0f,"Helvetica",8.0,fontdb);
    }

    if (tlblflg) plot_time_label(rplot,pad,pad,wdt-2*pad,hgt-2*pad,
				 90*lat,flip,tsfx,
                                 lon-tme_shft*(! rotflg),
                                 (wdt/2)-pad,6,
                                 txtcol,0x0f,"Helvetica",10.0,fontdb);
    RplotEndPlot(rplot);  

    if (!stdioflg) {
      if ((pflg) || (!rflg)) fclose(outfp);
    }
    if (fbplot !=NULL) { /* image file */
       img=fbplot->img;

#ifdef _XLIB_
       if (xd !=0) {
         XwinFrameBufferWindow(img,win);
         tmout.tv_sec=(int) delay;
         tmout.tv_usec=(delay-(int) delay)*1e6;
         if (delay!=0) xstat=XwinDisplayEvent(dp,1,&win,1,&tmout);
         else xstat=XwinDisplayEvent(dp,1,&win,1,NULL);
         if (xstat==1) break;
       } else {

         if (stdioflg) {
           if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
           else if (ppmflg) FrameBufferSavePPM(img,stdout);
           else FrameBufferSavePPMX(img,stdout);
         } else {
           outfp=fopen(filename,"w");
           if (xmlflg) FrameBufferSaveXML(img,stream,outfp);
           else if (ppmflg) FrameBufferSavePPM(img,outfp);
           else FrameBufferSavePPMX(img,outfp);
           fclose(outfp);
	 }
       }
#else
       if (stdioflg) {
         if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
         else if (ppmflg) FrameBufferSavePPM(img,stdout);
         else FrameBufferSavePPMX(img,stdout);
       } else {
         outfp=fopen(filename,"w");
         if (xmlflg) FrameBufferSaveXML(img,stream,outfp);
         else if (ppmflg) FrameBufferSavePPM(img,outfp);
         else FrameBufferSavePPMX(img,outfp);
         fclose(outfp);
       } 
#endif
       FrameBufferFree(fbplot->img);
       fbplot->img=NULL;
    }
    if (rmap !=NULL) PolygonFree(rmap);
    if (rbnd !=NULL) PolygonFree(rbnd);
    if (rgrd !=NULL) PolygonFree(rgrd);
    if (rfov !=NULL) PolygonFree(rfov);
    if (rtmk !=NULL) PolygonFree(rtmk);
    if (phmb !=NULL) PolygonFree(phmb);
    if (ptrm !=NULL) PolygonFree(ptrm);
    ptrm=NULL;
    phmb=NULL;
    ContourFree(cnum,ctr);
    ctr=NULL;
    if (stdioflg) break;
    if (old) s=OldCnvMapFread(fp,&rcmap,&rgrid);
    else s=CnvMapFread(fp,&rcmap,&rgrid);
    if ((esec !=-1) && (rcmap.ed_time>esec)) break;
    cnt++;
  } while (s !=-1);
  fclose(fp);
 

#ifdef _XLIB_
  if (xd !=0) {
    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  }
#endif

  return 0;
  
}  


