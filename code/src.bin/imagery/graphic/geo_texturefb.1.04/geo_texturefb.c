/* geo_texturefb.c
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
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "math.h"
#include "rtypes.h"
#include "rxml.h"
#include "option.h"

#include "rfbuffer.h"
#include "fontdb.h"
#include "rplot.h"

#include "raster.h"

#include "rplotin.h"
#include "imagedb.h"
#include "fbrplot.h"

#include "polygon.h"
#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"
#include "make_grid.h"
#include "sza.h"
#include "szamap.h"
#include "clip.h"
#include "gmap.h"

#include "hlpstr.h"
#include "errstr.h"
#include "version.h"

/*
 $Log: geo_texturefb.c,v $
 Revision 1.6  2005/08/11 14:40:32  barnes
 Added option to dump command line options.

 Revision 1.5  2004/06/21 22:20:30  barnes
 Added missing headers.

 Revision 1.4  2004/05/14 21:44:33  barnes
 Added closing return.

 Revision 1.3  2004/05/14 21:44:00  barnes
 Re-implemented the GeoMap library.

 Revision 1.2  2004/05/14 19:36:33  barnes
 Modifications to deal with changes to the libraries.

 Revision 1.1  2003/04/11 23:39:33  barnes
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

struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *clip;

struct FrameBufferClip *fbclp=NULL;

float *ilat=NULL;
float *ilon=NULL;

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

  int s=0;

  char *name=NULL;
  char *dname={"bg"};

  struct FrameBuffer *day=NULL;
  struct FrameBuffer *night=NULL;

  struct FrameBuffer *img=NULL;
  struct FrameBufferRplot *fbplot=NULL;
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  FILE *fontfp=NULL;
  char *fntdbfname=NULL;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;

  int arg;
  
  char *envstr=NULL;
  char *cfname=NULL;
  FILE *fp;


  int wdt=540,hgt=540;
  float width=0.5;

  struct Rplot *rplot=NULL;

  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char dppmflg=0;
  unsigned char dppmxflg=0;
  unsigned char dxmlflg=0;

  unsigned char nppmflg=0;
  unsigned char nppmxflg=0;
  unsigned char nxmlflg=0;

  unsigned char help=0; 
  unsigned char option=0; 

  char *bgcol_txt=NULL;

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

  unsigned char mapflg=0;
  unsigned char bndflg=0;
  unsigned char grdflg=0;

  unsigned char lstflg=0;
  unsigned char trmflg=0;
  unsigned char ftrmflg=0;
  float tmin=0.5;
  float tscl=10.0;

  char *grdcol_txt=NULL;
  char *cstcol_txt=NULL;
  char *bndcol_txt=NULL;
  char *trmcol_txt=NULL;

  unsigned int grdcol;
  unsigned int cstcol;
  unsigned int bndcol;
  unsigned int trmcol;

  unsigned char smflg=0;

  FILE *mapfp;
  float marg[4];


  int x,y;
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;

  unsigned char *dr,*dg,*db,*da;
  unsigned char *nr=NULL,*ng=NULL,*nb=NULL,*na=NULL;
  int dwdt,dhgt,nwdt=0,nhgt=0;


  char *tmetxt=NULL;
  char *dtetxt=NULL;

  double tval=-1;
  double dval=-1;

  int yr,mo,dy,hr,mt;
  double sc;
  int yrsec;
  float tme_shft;
 
  float plat,plon;
  float fx,fy;
  int px,py;
  float a;

  double LsoT,LT,Hangle,dec,eqt,mlon;
  
  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

  clip=MapCircleClip(10);

  bgcol=RplotColor(0x00,0x00,0x40,0xff);

  grdcol=RplotColor(0x40,0x40,0x40,0xff);
  cstcol=RplotColor(0x80,0x80,0x80,0xff);
  bndcol=RplotColor(0x80,0x80,0x80,0xff);
  trmcol=RplotColor(0xc0,0xc0,0xc0,0xff);


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);

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

  OptionAdd(&opt,"coast",'x',&mapflg);
  OptionAdd(&opt,"bnd",'x',&bndflg);
  OptionAdd(&opt,"grd",'x',&grdflg);

  OptionAdd(&opt,"lst",'x',&lstflg);
  OptionAdd(&opt,"term",'x',&trmflg);
  OptionAdd(&opt,"fterm",'x',&ftrmflg);
  OptionAdd(&opt,"tmin",'f',&tmin);
  OptionAdd(&opt,"tscl",'f',&tscl);

  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);
  OptionAdd(&opt,"cstcol",'t',&cstcol_txt);
  OptionAdd(&opt,"bndcol",'t',&bndcol_txt);
  OptionAdd(&opt,"trmcol",'t',&trmcol_txt);
 
  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);
  
  OptionAdd(&opt,"lnewdt",'f',&width);

  OptionAdd(&opt,"name",'t',&name);

  OptionAdd(&opt,"dppm",'x',&dppmflg);
  OptionAdd(&opt,"dppmx",'x',&dppmxflg);
  OptionAdd(&opt,"dxml",'x',&dxmlflg);

  OptionAdd(&opt,"nppm",'x',&nppmflg);
  OptionAdd(&opt,"nppmx",'x',&nppmxflg);
  OptionAdd(&opt,"nxml",'x',&nxmlflg);

  OptionAdd(&opt,"smooth",'x',&smflg);

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


  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (dppmflg) day=FrameBufferLoadPPM(fp,"image");
  else if (dppmxflg) day=FrameBufferLoadPPMX(fp);
  else day=loadxml(fp);
  fclose(fp);

  if (day==NULL) {
    fprintf(stderr,"Error loading image.\n");
    exit(-1);
  }

  if (argc-arg>1) {

    fp=fopen(argv[arg+1],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }

    if (nppmflg) night=FrameBufferLoadPPM(fp,"image");
    else if (nppmxflg) night=FrameBufferLoadPPMX(fp);
    else night=loadxml(fp);
    fclose(fp);

    if (night==NULL) {
      fprintf(stderr,"Error loading image.\n");
      exit(-1);
    }
  }  


  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) {
    dval=strdate(dtetxt);
    tval+=dval;
  }
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  if (magflg) {
    PolygonModify(map,AACGMtransform,NULL);
    PolygonModify(bnd,AACGMtransform,NULL);
  }

  if ((ortho==0) && (stereo==0) && (gvp==0)) cylind=1;

  if ((lat<0) && (latmin>0)) latmin=-latmin;
  if ((lat>0) && (latmin<0)) latmin=-latmin;

  if (grdflg) grd=make_grid(15,10,cylind);   
 

 if (trmflg) {
    if ((cylind) || (ortho) || (gvp))
       trm=SZATerminator(yr,mo,dy,hr,mt,sc,0,magflg,
                           1.0,90.0);
     else if (lat>0) trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,magflg,
                                     1.0,90.0);
     else trm=SZATerminator(yr,mo,dy,hr,mt,sc,-1,magflg,1.0,90.0);
  }

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

  if ((cylind) || (sqflg)) clip=MapSquareClip(); 
  else clip=MapCircleClip(10);

  if (mapflg) {
    nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,tfunc,marg);
    if (cylind) nmap=wrap(nmap);
    pmap=PolygonClip(clip,nmap);
  }
  if (bndflg) {
     nbnd=PolygonTransform(bnd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
     if (cylind) nbnd=wrap(nbnd);
     pbnd=PolygonClip(clip,nbnd);
  }
  if (grdflg) {
     ngrd=PolygonTransform(grd,2*sizeof(float),PolygonXYbbox,tfunc,marg);
     if (cylind) ngrd=wrap(ngrd);
     pgrd=PolygonClip(clip,ngrd);
  }
  if (trmflg) {
     ntrm=PolygonTransform(trm,2*sizeof(float),PolygonXYbbox,tfunc,marg);
     if (cylind) ntrm=wrap(ntrm);
     ptrm=PolygonClip(clip,ntrm);
  }

  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (grdcol_txt !=NULL) grdcol=RplotColorStringRGB(grdcol_txt);
  if (cstcol_txt !=NULL) cstcol=RplotColorStringRGB(cstcol_txt);
  if (bndcol_txt !=NULL) bndcol=RplotColorStringRGB(bndcol_txt);
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
  
  if (cylind) {
    marg[1]=0;
    marg[2]=0;
  }

  GeoMap(wdt,hgt,tfunc,marg,&ilat,&ilon);
  
  RplotMakePlot(rplot,name,wdt,hgt,24);
 
  RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,0,0x0f,0,NULL);

  if ((cylind) || (sqflg)) RplotRectangle(rplot,NULL,0,0,wdt,hgt,
			     1,bgcol,0x0f,0,NULL);
  else RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2,hgt/2,
                 1,bgcol,0x0f,0,NULL);

  if (grdflg) {
    MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,0,
                                grdcol,0x0f,width,NULL,
                                pgrd,1);
  }

  if (cylind) fbclp=NULL;
  else if (!sqflg) fbclp=fbclip(wdt,hgt);
  img=fbplot->img; 

  r=img->img;
  g=img->img+img->wdt*img->hgt;
  b=img->img+img->wdt*img->hgt*2;

  dr=day->img;
  dg=day->img+day->wdt*day->hgt;
  db=day->img+day->wdt*day->hgt*2;
  da=day->msk;
  dwdt=day->wdt;
  dhgt=day->hgt;

  if (night !=NULL) {
    nr=night->img;
    ng=night->img+night->wdt*night->hgt;
    nb=night->img+night->wdt*night->hgt*2;
    na=night->msk;
    nwdt=night->wdt;
    nhgt=night->hgt;
  }

  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
      if ((fbclp !=NULL) && (fbclp->clp[y*wdt+x]==0)) continue;
      
      if (ilat[y*wdt+x]<-90) continue;
      if (ilat[y*wdt+x]>90) continue;
      if (ilon[y*wdt+x]<0) continue;
      if (ilon[y*wdt+x]>360) continue;
      

      plat=ilat[y*wdt+x];
      plon=ilon[y*wdt+x];
 
      if (magflg) {
	double mlat,mlon,rho;
        s=AACGMConvert(plat,plon,150.0,&mlat,&mlon,&rho,1);
        plat=mlat;
        plon=mlon;
      }

      if (cylind) {
        if (rotflg) lon+=lon+tme_shft;
        else lon+=lon;
      }
     
     

      fy=0.5-plat/180.0;
      fx=0.5+plon/360.0;
      if (fx>1) fx-=1;
      if (fx<0) fx+=1;    

      px=day->wdt*fx;
      py=day->hgt*fy;

      if (smflg) {
        int rl,gl,bl,al;
        int rr,gr,br,ar;
        int rb,gb,bb,ab;
        int rt,gt,bt,at;

        rl=dr[py*dwdt+px];
        gl=dg[py*dwdt+px];
        bl=db[py*dwdt+px]; 
        al=da[py*dwdt+px]; 
	
        if (px<dwdt-1) {
          rr=dr[py*dwdt+px+1];
          gr=dg[py*dwdt+px+1];
          br=db[py*dwdt+px+1];
          ar=da[py*dwdt+px+1]; 
	} else {
          rr=rl;
          gr=gl;
          br=bl;
          ar=bl;
	}
        
        rb=rl+(rr-rl)*(dwdt*fx-px);
        gb=gl+(gr-gl)*(dwdt*fx-px);
        bb=bl+(br-bl)*(dwdt*fx-px);
        ab=al+(ar-al)*(dwdt*fx-px);
	
        if (py<dhgt-1) {
          rl=dr[(py+1)*dwdt+px];
          gl=dg[(py+1)*dwdt+px];
          bl=db[(py+1)*dwdt+px]; 
          al=da[(py+1)*dwdt+px]; 
          if (px<dwdt-1) {
            rr=dr[(py+1)*dwdt+px+1];
            gr=dg[(py+1)*dwdt+px+1];
            br=db[(py+1)*dwdt+px+1];
            ar=da[(py+1)*dwdt+px+1];
	  } else {
            rr=rl;
            gr=gl;
            br=bl;
            ar=al;
	  }
          rt=rl+(rr-rl)*(dwdt*fx-px);
          gt=gl+(gr-gl)*(dwdt*fx-px);
          bt=bl+(br-bl)*(dwdt*fx-px);
          at=al+(ar-al)*(dwdt*fx-px);
	} else {
          rt=rb;
          gt=gb;
          bt=bb;
          at=ab;
	}
	
        rb=rb+(rt-rb)*(dhgt*fy-py);
        gb=gb+(gt-gb)*(dhgt*fy-py);
        bb=bb+(bt-bb)*(dhgt*fy-py);
        ab=ab+(at-ab)*(dhgt*fy-py);
	
        a=ab/255.0;
        r[y*wdt+x]=(1-a)*r[y*wdt+x]+a*rb;
        g[y*wdt+x]=(1-a)*g[y*wdt+x]+a*gb;
        b[y*wdt+x]=(1-a)*b[y*wdt+x]+a*bb;

      } else {
        a=da[py*dwdt+px]/255.0;
        r[y*wdt+x]=(1-a)*r[y*wdt+x]+a*dr[py*dwdt+px];
        g[y*wdt+x]=(1-a)*g[y*wdt+x]+a*dg[py*dwdt+px];
        b[y*wdt+x]=(1-a)*b[y*wdt+x]+a*db[py*dwdt+px];
      }

      if (ftrmflg) {
        float Z;
        LsoT=(hr*3600+mt*60+sc)+(plon*4*60)+eqt;
        Hangle=15*((LsoT/3600)-12);
        Z=SZAAngle(plon,plat,dec,Hangle);
        if (Z>90) {
          if (tscl !=0) Z=(1-(Z-90)/tscl); 
          else Z=0.0;
        } else Z=1.0;
        if (Z>1.0) Z=1.0;
        if (Z<0) Z=0;
        if (Z==1) continue;
        if (night==NULL) {
           if (Z<tmin) Z=tmin;
           r[y*wdt+x]=r[y*wdt+x]*Z;
           g[y*wdt+x]=g[y*wdt+x]*Z;
           b[y*wdt+x]=b[y*wdt+x]*Z;
	} else {
          if (smflg) {
            int rl,gl,bl,al;
            int rr,gr,br,ar;
            int rb,gb,bb,ab;
            int rt,gt,bt,at;

            rl=nr[py*nwdt+px];
            gl=ng[py*nwdt+px];
            bl=nb[py*nwdt+px]; 
            al=na[py*nwdt+px]; 
	
            if (px<nwdt-1) {
              rr=nr[py*nwdt+px+1];
              gr=ng[py*nwdt+px+1];
              br=nb[py*nwdt+px+1];
              ar=na[py*nwdt+px+1]; 
	    } else {
              rr=rl;
              gr=gl;
              br=bl;
              ar=bl;
	    }
        
            rb=rl+(rr-rl)*(nwdt*fx-px);
            gb=gl+(gr-gl)*(nwdt*fx-px);
            bb=bl+(br-bl)*(nwdt*fx-px);
            ab=al+(ar-al)*(nwdt*fx-px);
	
            if (py<nhgt-1) {
             rl=nr[(py+1)*nwdt+px];
             gl=ng[(py+1)*nwdt+px];
             bl=nb[(py+1)*nwdt+px]; 
             al=na[(py+1)*nwdt+px]; 
             if (px<nwdt-1) {
               rr=nr[(py+1)*nwdt+px+1];
               gr=ng[(py+1)*nwdt+px+1];
               br=nb[(py+1)*nwdt+px+1];
               ar=na[(py+1)*nwdt+px+1];
	     } else {
                rr=rl;
                gr=gl;
                br=bl;
                ar=al;
	     }
             rt=rl+(rr-rl)*(nwdt*fx-px);
             gt=gl+(gr-gl)*(nwdt*fx-px);
             bt=bl+(br-bl)*(nwdt*fx-px);
             at=al+(ar-al)*(nwdt*fx-px);
	   }  else {
              rt=rb;
              gt=gb;
              bt=bb;
              at=ab;
  	    }
	
            rb=rb+(rt-rb)*(nhgt*fy-py);
            gb=gb+(gt-gb)*(nhgt*fy-py);
            bb=bb+(bt-bb)*(nhgt*fy-py);
            ab=ab+(at-ab)*(nhgt*fy-py);
	
            a=ab/255.0;
            if (a !=0) {
              a=(1-Z)*a;
              r[y*wdt+x]=(1-a)*r[y*wdt+x]+a*rb;
              g[y*wdt+x]=(1-a)*g[y*wdt+x]+a*gb;
              b[y*wdt+x]=(1-a)*b[y*wdt+x]+a*bb;
	    } else {
              r[y*wdt+x]=r[y*wdt+x]*Z;
              g[y*wdt+x]=g[y*wdt+x]*Z;
              b[y*wdt+x]=b[y*wdt+x]*Z;
	    }
          } else {
            a=na[py*nwdt+px]/255.0;
            if (a !=0) { 
              a=(1-Z)*a;
              r[y*wdt+x]=(1-a)*r[y*wdt+x]+a*nr[py*dwdt+px];
              g[y*wdt+x]=(1-a)*g[y*wdt+x]+a*ng[py*dwdt+px];
              b[y*wdt+x]=(1-a)*b[y*wdt+x]+a*nb[py*dwdt+px];
	    } else {
              r[y*wdt+x]=r[y*wdt+x]*Z;
              g[y*wdt+x]=g[y*wdt+x]*Z;
              b[y*wdt+x]=b[y*wdt+x]*Z;
	    }
	  }
	}
	
      }
    }
  }
  
  if (mapflg) {
     MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,0,cstcol,0x0f,
                    width,NULL,
                    pmap,1);
     
     MapPlotOpenPolygon(rplot,NULL,0,0,wdt,hgt,cstcol,0x0f,
                    width,NULL,
                    pmap,2);
     
     MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,0,cstcol,0x0f,
                    width,NULL,
                    pmap,0);
  }

  if (bndflg) MapPlotOpenPolygon(rplot,NULL,0,0,wdt,hgt,
                                bndcol,0x0f,width,NULL,
                                pbnd,-1);

  if (trmflg) MapPlotOpenPolygon(rplot,NULL,0,0,wdt,hgt,
                                trmcol,0x0f,width,NULL,
                                ptrm,1);

  RplotEndPlot(rplot);

  if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0;  
  
}  


