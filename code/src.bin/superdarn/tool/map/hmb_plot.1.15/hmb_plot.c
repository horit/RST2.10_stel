/* hmb_plot.c
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
#include <sys/types.h>
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

#include "rplotg.h"

#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"

#include "hlpstr.h"
#include "version.h"


/*
 $Log: hmb_plot.c,v $
 Revision 1.16  2005/08/17 21:43:09  barnes
 Fixed bug in setting the name of the plot.

 Revision 1.15  2005/08/10 21:57:02  barnes
 Added option to dump the command line options.

 Revision 1.14  2004/10/12 23:12:45  barnes
 Moved to DataMap format.

 Revision 1.13  2004/05/10 22:03:39  barnes
 Removed redundant code.

 Revision 1.12  2003/03/15 21:29:22  barnes
 Added information (metadata) to plot.

 Revision 1.11  2003/03/14 20:28:08  barnes
 Added support for extended PPM files.

 Revision 1.10  2003/03/14 20:21:35  barnes
 Added a name to the plot.

 Revision 1.9  2003/03/07 03:13:32  barnes
 Modification to command line arguments.

 Revision 1.8  2003/03/01 00:06:33  barnes
 Fixed bug in setting up the command line options.

 Revision 1.7  2003/02/28 22:25:01  barnes
 Fixed minor bug in decoding options.

 Revision 1.6  2003/02/28 21:28:28  barnes
 Major modifications to suppor the base libraries and the
 Rplot output type.

 Revision 1.5  2002/04/10 19:16:44  barnes
 Modified code to deal with Southern Hemisphere.

 Revision 1.4  2001/07/09 18:16:59  barnes
 Modified code that draws axis.

 Revision 1.3  2001/06/27 21:09:15  barnes
 Added license tag

 Revision 1.2  2001/06/08 15:56:16  barnes
 Fixed bug in reporting help.

 Revision 1.1  2001/06/05 14:10:45  barnes
 Initial revision

 */

#define WIDTH (558-58)
#define HEIGHT (734-58)

struct GridData grd;
struct CnvMapData  map;

struct OptionData opt;
struct OptionFile *optf=NULL;

int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data) {
 
  struct FrameBufferFontDB *fontdb;
  struct FrameBufferFont *fnt;
  int tbox[3];

  fontdb=(struct FrameBufferFontDB *)data;
  if (fontdb==NULL) return -1;
  fnt=FrameBufferFontDBFind(fontdb,fntname,sze);
  
  FrameBufferTextBox(fnt,num,txt,tbox);
  
  box[0]=tbox[0];
  box[1]=tbox[1];
  box[2]=tbox[2];
 
  return 0;
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


char *xlabel(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d:%.2d",((int) val/3600) % 24, ((int) val % 3600)/60);
  return txt;
}


char *ylabel(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d",(int) val);

  return txt;
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


#ifdef _XLIB_
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  char *wname="istp_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

  char name[256]="hmb";

  struct FrameBuffer *img=NULL;
  struct FrameBufferRplot *fbplot=NULL;
  struct PostScriptRplot *psplot=NULL;
  struct PostScript *psdata=NULL;
  struct XMLdata *xmldata=NULL;
  struct RplotInPlot *rplotin=NULL;
  FILE *fontfp;
  char *fntdbfname;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int fgcol;
  float lne=0.5;

  float ox,oy,px,py;

  int arg;
 
  char *cfname=NULL;
  FILE *fp;

  unsigned char land=0;
  float wdt=WIDTH,hgt=HEIGHT;
 
  struct Rplot *rplot;
  struct PlotData *plt;

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

  unsigned char vb=0;

  char *fname=NULL;
  char *extime_txt=NULL;

  char title[256];  
  int yr,mo,dy,hr,mt;
  double sc;

  double extime=24*3600;
  double stime=-1;
  double mtime=0;

  int xnum=1,ynum=1;
  float lpad=40,rpad=40,tpad=80,bpad=40;
  float xoff=0,yoff=0;

  double xmin=0,xmax=24*3600;
  double xmajor=3*3600,xminor=15*60;
  int xtick_flg=0x03;
  int xlbl_flg=0x03;
 
  char *xtitle="Time";
  int xttl_flg=0x03;
 
  double ymin=40,ymax=80;
  double ymajor=5,yminor=1;
  int ytick_flg=0x03;
  int ylbl_flg=0x03;
 
  char *ytitle="Latitude";
  int yttl_flg=0x03;

  int status=0;
  

 OptionAdd(&opt,"-help",'x',&help);
 OptionAdd(&opt,"-option",'x',&option);

 OptionAdd(&opt,"new",'x',&new);
 OptionAdd(&opt,"vb",'x',&vb);

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
 
  OptionAdd(&opt,"l",'x',&land); 

  OptionAdd(&opt,"w",'f',&wdt);
  OptionAdd(&opt,"h",'f',&hgt);

  OptionAdd(&opt,"ex",'t',&extime_txt);

  OptionAdd(&opt,"cf",'t',&cfname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  

  old=!new;

  if (arg<argc) fname=argv[arg];
  if (cfname !=NULL) { /* load the configuration file */
    do {
      fp=fopen(cfname,"r");
      if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        arg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
        if (arg<optf->argc) {
	  fname=malloc(strlen(optf->argv[arg]+1));
	  strcpy(fname,optf->argv[arg]);
        }
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



 
  if (extime_txt !=NULL) extime=strtime(extime_txt);

  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"Map file not found.\n");
      exit(-1);
    }
  } else fp=stdin;


  xmax=extime;
 
  if (xmax>24*3600) {
    xmajor=12*3600;
    xminor=3*3600;
  } else if (xmax<12*3600) {
    xmajor=3600;
    xminor=5*60;
  } else if (xmax<3600) {
    xmajor=15*60;
    xminor=60;
  }


  if (old) status=OldCnvMapFread(fp,&map,&grd);
    else status=CnvMapFread(fp,&map,&grd);
  stime=map.st_time;

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(title,"%.4d%.2d%.2d",yr,mo,dy);
  sprintf(name,"%.4d%.2d%.2d.hmb",yr,mo,dy);



 if ((land==1) && (wdt== WIDTH) && (hgt==HEIGHT)) {
    int t;
    t=hgt;
    hgt=wdt;
    wdt=t;
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

  /* now determing our output type */

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
       psplot->land=land;
       psplot->xoff=xpoff;
       psplot->yoff=ypoff;
     }
     XMLSetStart(xmldata,RplotInStart,rplotin);
     XMLSetEnd(xmldata,RplotInEnd,rplotin);
  }


  bgcol=RplotColor(255,255,255,255);
  fgcol=RplotColor(0,0,0,255);
 
  rplot=RplotMake();
  if (rflg) RplotSetText(rplot,xmldecode,xmldata);
  else RplotSetText(rplot,stream,stdout);

  RplotMakePlot(rplot,name,wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);
  RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);

  plt=PlotMake(wdt,hgt,xnum,ynum,lpad,rpad,bpad,tpad,xoff,yoff);
  PlotSetRplot(plt,rplot);
  PlotSetTextBox(plt,txtbox,fontdb);  

  plt->major_hgt=8;
  plt->minor_hgt=5;
  plt->ttl_hgt=20;
  plt->lbl_hgt=10;

  plt->ttl_wdt=20;
  plt->lbl_wdt=10;

 
  if (map.hemisphere==-1) {
    ymin=-80;
    ymax=-40;
  }

  PlotPanel(plt,0,fgcol,0x0f,lne);
  PlotXaxis(plt,0,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);

  PlotXaxisLabel(plt,0,xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  PlotXaxisTitle(plt,0,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);

  PlotYaxis(plt,0,ymin,ymax,ymajor,yminor,ytick_flg,fgcol,0x0f,lne);

  PlotYaxisLabel(plt,0,ymin,ymax,ymajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  PlotYaxisTitle(plt,0,yttl_flg,strlen(ytitle),ytitle,"Helvetica",
                 10.0,fgcol,0x0f);


  ox=0;
  oy=map.latmin;

  if (old) status=OldCnvMapFread(fp,&map,&grd);
  else status=CnvMapFread(fp,&map,&grd);

  while (status !=-1) {          
    mtime=map.st_time;
    TimeEpochToYMDHMS(mtime,&yr,&mo,&dy,&hr,&mt,&sc);
    
    if (vb==1) 
      fprintf(stderr,"%.2d-%.2d-%.4d %.2d:%.2d\n",dy,mo,dy,hr,mt);

    px=mtime-stime;
    py=map.latmin;
    PlotLine(plt,0,ox,oy,px,py,xmin,xmax,ymin,ymax,fgcol,0x0f,lne,NULL);

    ox=px;
    oy=py;
    
    if ((mtime-stime)>extime) break;
    if (old) status=OldCnvMapFread(fp,&map,&grd);
    else status=CnvMapFread(fp,&map,&grd);

  }  

  plt->ttl_hgt=40;
  PlotXaxisTitle(plt,0,0x02,strlen(title),title,
                "Helvetica",20.0,fgcol,0x0f);

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


