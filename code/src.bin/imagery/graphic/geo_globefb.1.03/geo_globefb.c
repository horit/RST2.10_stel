/* geo_globefb.c
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"

#include "rfbuffer.h"
#include "fontdb.h"
#include "rplot.h"

#include "rplotin.h"
#include "imagedb.h"
#include "fbrplot.h"

#include "polygon.h"
#include "raster.h"
#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"
#include "clip.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: geo_globefb.c,v $
 Revision 1.6  2005/08/11 14:29:40  barnes
 Added option to dump the command line options.

 Revision 1.5  2004/05/17 21:36:54  barnes
 Added code for a time grid and inverse grid (eg. magnetic grid for
 geographic plot and vice versa).

 Revision 1.4  2004/05/05 19:01:47  barnes
 Added missing header.

 Revision 1.3  2004/05/04 19:21:54  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 20:21:09  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/24 18:07:17  barnes
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

struct PolygonData *igrd;
struct PolygonData *nigrd;
struct PolygonData *pigrd;

struct PolygonData *tgrd;
struct PolygonData *ntgrd;
struct PolygonData *ptgrd;

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *clip;

struct FrameBufferClip *fbclp=NULL;

float *zenith;

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


  char *name=NULL;
  char *dname={"bg"};

  struct FrameBuffer *img=NULL;
  struct FrameBufferRplot *fbplot=NULL;
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;

  int arg;

  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;


  int wdt=540,hgt=540;
  int pad=0;
  float width=0.5;

  struct Rplot *rplot=NULL;

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

  unsigned char bgflg=0;
  unsigned char mapflg=0;
  unsigned char fmapflg=0;
  unsigned char bndflg=0;
  unsigned char grdflg=0;
  unsigned char igrdflg=0;
  unsigned char tgrdflg=0;



  unsigned char lstflg=0;
  unsigned char trmflg=0;
  unsigned char ftrmflg=0;
  float tmin=0.5;
  float tscl=10.0;

  char *bgcol_txt=NULL;
  char *grdcol_txt=NULL;
  char *igrdcol_txt=NULL;
  char *tgrdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *lndcol_txt=NULL;
  char *seacol_txt=NULL;
  char *trmcol_txt=NULL;

  
  unsigned int bgcol;
  unsigned int grdcol;
  unsigned int igrdcol;
  unsigned int tgrdcol;

  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int lndcol;
  unsigned int seacol;
  unsigned int trmcol;


  FILE *mapfp;
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

  float grdlat=10;
  float grdlon=15;

  float igrdlat=10;
  float igrdlon=15;

  float tgrdlat=10;
  float tgrdlon=15*6;

  unsigned char grdtop=0;
  unsigned char igrdtop=0;


  double LsoT,LT,Hangle,dec,eqt,mlon;
 

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

  bgcol=RplotColor(0xff,0xff,0xff,0xff);
  grdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  igrdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  tgrdcol=RplotColor(0xc0,0xc0,0xc0,0xff);
  cstcol=RplotColor(0xa0,0xa0,0xa0,0xff);
  bndcol=RplotColor(0x80,0x80,0x80,0xff);
  lndcol=RplotColor(0xff,0xff,0xf0,0xff);
  seacol=RplotColor(0xe0,0xf0,0xff,0xff);
  trmcol=RplotColor(0xc0,0xc0,0xc0,0xff);


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);
  OptionAdd(&opt,"pad",'i',&pad);

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

  OptionAdd(&opt,"bg",'x',&bgflg);
  OptionAdd(&opt,"coast",'x',&mapflg);
  OptionAdd(&opt,"fcoast",'x',&fmapflg);
  OptionAdd(&opt,"bnd",'x',&bndflg);
  OptionAdd(&opt,"grd",'x',&grdflg);
  OptionAdd(&opt,"grdlat",'f',&grdlat);
  OptionAdd(&opt,"grdlon",'f',&grdlon);
  OptionAdd(&opt,"igrd",'x',&igrdflg);
  OptionAdd(&opt,"igrdlat",'f',&igrdlat);
  OptionAdd(&opt,"igrdlon",'f',&igrdlon);
  OptionAdd(&opt,"tgrd",'x',&tgrdflg);
  OptionAdd(&opt,"tgrdlat",'f',&tgrdlat);
  OptionAdd(&opt,"tgrdlon",'f',&tgrdlon);

  OptionAdd(&opt,"grdontop",'x',&grdtop);
  OptionAdd(&opt,"igrdontop",'x',&igrdtop);

  OptionAdd(&opt,"lst",'x',&lstflg);
  OptionAdd(&opt,"term",'x',&trmflg);
  OptionAdd(&opt,"fterm",'x',&ftrmflg);
  OptionAdd(&opt,"tmin",'f',&tmin);
  OptionAdd(&opt,"tscl",'f',&tscl);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"igrdcol",'t',&igrdcol_txt);
  OptionAdd(&opt,"tgrdcol",'t',&tgrdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"lndcol",'t',&lndcol_txt);
  OptionAdd(&opt,"seacol",'t',&seacol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"lnewdt",'f',&width);

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


  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) {
    dval=strdate(dtetxt);
    tval+=dval;
  }
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

 

 
  if ((ortho==0) && (stereo==0) && (gvp==0)) cylind=1;

  if (grdflg) grd=make_grid(grdlon,grdlat,cylind);   
  if (igrdflg) igrd=make_grid(igrdlon,igrdlat,cylind);   
  if (tgrdflg) tgrd=make_grid(tgrdlon,tgrdlat,cylind);   

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;


  if (magflg) {
    PolygonModify(map,AACGMtransform,NULL);
    PolygonModify(bnd,AACGMtransform,NULL);
    if (igrdflg) PolygonModify(igrd,AACGMtransform,NULL);
  } else {
    if (igrdflg) PolygonModify(igrd,AACGMtransform,marg);
  }

  if (trmflg) {
    if ((cylind) || (ortho))
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
       if (cylind) nigrd=wrap(nigrd);
       pigrd=PolygonClip(clip,nigrd);
  }
  if (trmflg) {
     ntrm=PolygonTransform(trm,2*sizeof(float),PolygonXYbbox,
                           tfunc,marg);
     if (cylind) ntrm=wrap(ntrm);
     ptrm=PolygonClip(clip,ntrm); 
  }

  if (tgrdflg) {
    if (rotflg) marg[1]=0;
    else marg[1]=lon-tme_shft;
    ntgrd=PolygonTransform(tgrd,2*sizeof(float),PolygonXYbbox,
	      		   tfunc,marg);
    ptgrd=PolygonClip(clip,ntgrd);
    if (rotflg) marg[1]=lon+tme_shft;
    else marg[1]=lon;
  }

  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (grdcol_txt !=NULL) grdcol=RplotColorStringRGB(grdcol_txt);
  if (igrdcol_txt !=NULL) igrdcol=RplotColorStringRGB(igrdcol_txt);
  if (tgrdcol_txt !=NULL) igrdcol=RplotColorStringRGB(tgrdcol_txt);
  if (cstcol_txt !=NULL) cstcol=RplotColorStringRGB(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=RplotColorStringRGB(bndcol_txt);
  if (lndcol_txt !=NULL) lndcol=RplotColorStringRGB(lndcol_txt);
  if (seacol_txt !=NULL) seacol=RplotColorStringRGB(seacol_txt);
  if (trmcol_txt !=NULL) trmcol=RplotColorStringRGB(trmcol_txt);


  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
  }
  
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
  
  if (name==NULL) name=dname;

  xmldata=XMLMake();
  rplotin=RplotInMake(xmldata);  
  fbplot=FrameBufferRplotMake(fontdb,NULL);
  FrameBufferRplotSetAll(rplotin,fbplot);
  XMLSetStart(xmldata,RplotInStart,rplotin);
  XMLSetEnd(xmldata,RplotInEnd,rplotin);
  rplot=RplotMake();
  RplotSetText(rplot,xmldecode,xmldata);
  
  if (ftrmflg) {
    if (cylind) {
      marg[1]=0;
      marg[2]=0;
    }

    zenith=SZAMap(yr,mo,dy,hr,mt,sc,-90.0,
                   wdt-2*pad,hgt-2*pad,magflg,tfunc,marg);   
  }
  


  RplotMakePlot(rplot,name,wdt,hgt,24);
 
  RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,0,0x0f,0,NULL);
  if (bgflg) RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
  if (fmapflg) {
    if ((cylind) || (sqflg)) RplotRectangle(rplot,NULL,pad,pad,wdt-2*pad,
			       hgt-2*pad,1,seacol,0x0f,0,NULL);
     RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2-pad,hgt/2-pad,
                  1,seacol,0x0f,0,NULL);
  }


  if ((grdflg) && (grdtop==0)) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }

  if ((igrdflg) && (igrdtop==0)) {
     MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,width,NULL,
                                pigrd,1);
  }

  if (fmapflg) {
    MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,lndcol,0x0f,0,NULL,
                   pmap,1);
  
    MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,lndcol,0x0f,0,NULL,
                   pmap,3);
  
    MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,1,seacol,0x0f,0,NULL,
                   pmap,0);
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

  if ((igrdflg) && (igrdtop==1)) {
     MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                igrdcol,0x0f,width,NULL,
                                pigrd,1);
  }

  if (tgrdflg) {
       MapPlotPolygon(rplot,NULL,pad,pad,wdt-2*pad,hgt-2*pad,0,
                                tgrdcol,0x0f,width,NULL,
                                ptgrd,1);
  }

  RplotEndPlot(rplot);
  if ((cylind) || (sqflg)) clip=NULL;
  else fbclp=fbclip(wdt-2*pad,hgt-2*pad);
  img=fbplot->img; 

  if (ftrmflg) {
    float Z=0; 
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

        r[(y+pad)*wdt+x+pad]=r[(y+pad)*wdt+x+pad]*Z;
        g[(y+pad)*wdt+x+pad]=g[(y+pad)*wdt+x+pad]*Z;
        b[(y+pad)*wdt+x+pad]=b[(y+pad)*wdt+x+pad]*Z;
      }
    }
  }
  
  if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0;
  
}  


