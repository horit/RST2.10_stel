/* raw_simage
   =========== 
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

#ifdef _XLIB_
#include "xwin.h"
#endif
#include "rtime.h"

#include "simage.h"

#include "key.h"
#include "text_box.h"
#include "plot_time.h"

#include "hlpstr.h"
#include "version.h"

/*
 $Log: raw_simage.c,v $
 Revision 1.7  2006/11/22 19:39:28  code
 Changed limits.

 Revision 1.6  2005/08/11 14:44:16  barnes
 Added option to dump command line options.

 Revision 1.5  2004/06/21 22:22:31  barnes
 Added missing header.

 Revision 1.4  2004/05/04 20:08:21  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/04/28 15:31:54  barnes
 Added missing time.h header.

 Revision 1.2  2004/04/27 20:31:55  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/03 18:54:22  barnes
 Initial revision

*/

unsigned char gry[256];

struct key key;

struct OptionData opt;
struct OptionFile *optf=NULL;

struct SImage simage;

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

int main(int argc,char *argv[]) {
  int s=0;

#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="raw_simage";
  int xdoff=-1;
  int ydoff=-1;
#endif

  struct FrameBuffer *simg=NULL;
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


  char *cfname=NULL;
  FILE *fp;


  float wdt=0,hgt=0;

  struct Rplot *rplot=NULL;

  char *fontname=NULL;
  float fontsize=10.0;
  char *dfontname="Helvetica";


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

  int inx=0;
  int num=0;
  float min=1e6,max=-1e6;
  unsigned char sflg=0;
  unsigned char lflg=0;
  unsigned char glflg=0; 

  unsigned char smflg=0;
  unsigned char tmeflg=0;

  int x,y,i;
  float c;
  int ci;
  unsigned char *r,*g,*b;

  for (i=0;i<256;i++) gry[i]=i;
  bgcol=RplotColor(0,0,0,0xff);
  txtcol=RplotColor(0xff,0xff,0xff,0xff);

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

  OptionAdd(&opt,"gl",'x',&glflg);
  OptionAdd(&opt,"log",'x',&lflg);
  OptionAdd(&opt,"stretch",'x',&sflg);

  OptionAdd(&opt,"min",'f',&min);
  OptionAdd(&opt,"max",'f',&max);

  OptionAdd(&opt,"smooth",'x',&smflg);

  OptionAdd(&opt,"inx",'i',&inx);


  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"key",'t',&key_fname);

  OptionAdd(&opt,"time",'x',&tmeflg);

  OptionAdd(&opt,"fontname",'t',&fontname);
  OptionAdd(&opt,"fontsize",'f',&fontsize);

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


  if (arg<argc) {
     fp=fopen(argv[arg],"r");
     if (fp==NULL) {
       fprintf(stderr,"File not found.\n");
       exit(-1);
    }
  } else fp=stdin;

  s=SImageFread(fp,&simage);
  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp); 
 
  num=simage.colors;

  if (inx<0) inx=0;
  if (inx>=num) inx=num-1;
 
  if ((wdt==0) || (hgt==0)) {
    wdt=simage.wdt;
    hgt=simage.hgt;
  }

  if (wdt>2000) wdt=2000;
  if (hgt>2000) hgt=2000;

  if ((min==1e6) && (simage.min !=NULL)) min=simage.min[inx];
  if ((max==-1e6)&& (simage.max !=NULL)) max=simage.max[inx];
  if ((lflg) && (min<=0)) min=1;

  if (lflg) min=log10(min);
  if (lflg) max=log10(max);

  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=RplotColorStringRGB(txtcol_txt);

  if (key_fname !=NULL) {
    fp=fopen(key_fname,"r");
    if (fp !=NULL) {
      load_key(fp,&key);
      fclose(fp);
    }
  }


  if (key.max==0) {
    key.max=255;
    key.r=gry;
    key.g=gry;
    key.b=gry;
  }

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"invalid plot size.\n");
    exit(-1);
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

  RplotMakePlot(rplot,simage.source,wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);

  simg=FrameBufferMake(NULL,simage.wdt,simage.hgt,24);
  FrameBufferClear(simg,bgcol,0x0f);

  r=simg->img;
  g=simg->img+simage.wdt*simage.hgt;
  b=simg->img+simage.wdt*simage.hgt*2;

  for (i=0;i<simage.numpnt;i++) {
    x=simage.pnt[i].x;
    y=simage.pnt[i].y;
    c=0;

    if ((glflg) && (simage.pnt[i].flg)) continue;
   
    if ((lflg) && (simage.data[i*num+inx]>0)) c=log10(simage.data[i*num+inx]);
    else c=simage.data[i*num+inx];
 
    if (c<min) continue;
    if (c>max) c=max;
    if (sflg) c=(c-min)/(max-min);
    else c=c/max;
     
    if (c<0.0) c=0.0;
    if (c>1.0) c=1.0;
    
    ci=(key.max-1)*c;
    
    if (key.r !=NULL) r[y*simage.wdt+x]=key.r[ci];
    if (key.g !=NULL) g[y*simage.wdt+x]=key.g[ci];
    if (key.b !=NULL) b[y*simage.wdt+x]=key.b[ci];
  }  
  if ((wdt !=simg->wdt) || (hgt !=simg->hgt)) {
    struct RplotMatrix m;
    m.a=wdt/simg->wdt;
    m.b=0;
    m.c=0;
    m.d=hgt/simg->hgt;
    RplotImage(rplot,&m,simg,0x0f,0,0,smflg);    
  } else RplotImage(rplot,NULL,simg,0x0f,0,0,smflg);

  if (tmeflg) plot_time(rplot,5,5,wdt-10,hgt-10,simage.st_time,simage.ed_time,
            txtcol,0x0f,fontname,fontsize,fontdb);
  RplotEndPlot(rplot);  


  if (fbplot==NULL) exit(0);
  if (fbplot->img==NULL) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }
  img=fbplot->img;

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-img->wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-img->hgt)/2;


    win=XwinMakeWindow(xdoff,ydoff,img->wdt,img->hgt,0,
                       dp,wname,
                       wname,argv[0],wname,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }

    XwinFrameBufferWindow(img,win);

    XwinShowWindow(win);

    XwinDisplayEvent(dp,1,&win,1,NULL);

    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  } else {
    if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
    else FrameBufferSavePPMX(img,stdout);
  }
  #else 
    if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
    else FrameBufferSavePPMX(img,stdout);
  #endif

    return 0;

}  


