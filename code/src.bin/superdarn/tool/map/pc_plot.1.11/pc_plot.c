/* pc_plot.c
   ========= 
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

#include "rplotg.h"

#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "oldcnvmapread.h"


#include "hlpstr.h"
#include "version.h"

/*
 $Log: pc_plot.c,v $
 Revision 1.12  2005/08/17 21:27:36  barnes
 Fixed bug in option types.

 Revision 1.11  2005/08/11 13:52:49  barnes
 Added option dump the command line options.

 Revision 1.10  2004/10/12 23:21:34  barnes
 Moved to DataMap format.

 Revision 1.9  2004/05/10 22:08:42  barnes
 Removed redundant code.

 Revision 1.8  2003/03/15 21:32:49  barnes
 Fixed bug in getting the end time, added a name to
 the plot, and added information (metadata).

 Revision 1.7  2003/03/14 20:26:27  barnes
 Added support for extended PPM files.

 Revision 1.6  2003/03/07 03:15:59  barnes
 Changed command line arguments.

 Revision 1.5  2003/03/01 00:05:47  barnes
 Fixed bug in setting up the command line options.

 Revision 1.4  2003/02/28 23:00:45  barnes
 Major modification to deal with the base libraries and
 the output of Rplot XML documents.

 Revision 1.3  2003/01/24 21:56:49  barnes
 Added code to remove the IMF delay.

 Revision 1.2  2003/01/24 21:32:33  barnes
 Added support for plotting IMF and to vary the start and
 stop times.

 Revision 1.1  2002/08/02 19:30:03  barnes
 Initial revision

 */

#define WIDTH (558-58)
#define HEIGHT (734-58)

struct GridData grd;
struct CnvMapData  map;

struct OptionData opt;
struct OptionFile *optf=NULL;

int count=0;
double tval[10000];
double polar[10000];
int scatter[10000];
int delayval[10000];
double bx[10000];
double by[10000];
double bz[10000];


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
 
  char *st_time_txt=NULL;
  char *extime_txt=NULL;
  char *ed_time_txt=NULL;
  char *sdate_txt=NULL;
  char *edate_txt=NULL;
  
  char title[256];  
  char name[256];
  int yr,mo,dy,hr,mt;
  double sc;

  int status;

  unsigned char nodelay=0;

  double extime=0;
  double stime=-1;
  double etime=-1;
  double mtime=0;
  double sdate=-1;
  double edate=-1;

  int i,j;
  int xnum=1,ynum=1;
  float pwdt,phgt;
  float lpad=40,rpad=40,tpad=30,bpad=30;
  float xoff=0,yoff=40;

  double xmin=0,xmax=0;
  double xmajor=3*3600,xminor=30*60;
  int xtick_flg=0x03;
  int xlbl_flg=0x03;
 
  char *xtitle="Time";
  int xttl_flg=0x03;
  
  unsigned char sflg=0;
  unsigned char  bflg=0;

  double ypmin=0,ypmax=100;
  double ypmajor=10,ypminor=5;
  double ysmin=0,ysmax=1200;
  double ysmajor=100,ysminor=20;

  double bxmin=-20,bxmax=20;
  double bxmajor=4,bxminor=4;
  double bymin=-20,bymax=20;
  double bymajor=4,byminor=4;
  double bzmin=-20,bzmax=20;
  double bzmajor=4,bzminor=4;

  int ytick_flg=0x03;
  int ylbl_flg=0x03;
 
  char *yptitle="Cross Polar Cap Potential";
  char *ystitle="Data Points";
  char *bxtitle="Bx (nT)";
  char *bytitle="By (nT)";
  char *bztitle="Bz (nT)";
  int yttl_flg=0x03;

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

  OptionAdd(&opt,"st",'t',&st_time_txt);
  OptionAdd(&opt,"et",'t',&ed_time_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);

  OptionAdd(&opt,"pmin",'d',&ypmin);
  OptionAdd(&opt,"pmax",'d',&ypmax);

  OptionAdd(&opt,"smin",'d',&ysmin);
  OptionAdd(&opt,"smax",'d',&ysmax);

  OptionAdd(&opt,"bxmin",'d',&bxmin);
  OptionAdd(&opt,"bxmax",'d',&bxmax);
  OptionAdd(&opt,"bymin",'d',&bymin);
  OptionAdd(&opt,"bymax",'d',&bymax);
  OptionAdd(&opt,"bzmin",'d',&bzmin);
  OptionAdd(&opt,"bzmax",'d',&bzmax);

  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"b",'x',&bflg);
  OptionAdd(&opt,"nodelay",'x',&nodelay);

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

  if (st_time_txt !=NULL) stime=strtime(st_time_txt);
  if (ed_time_txt !=NULL) etime=strtime(ed_time_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL) sdate=strdate(sdate_txt);
  if (edate_txt !=NULL) edate=strdate(edate_txt);

  if ((etime==-1) && (extime==0)) extime=24*3600;

  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"Map file not found.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (old) status=OldCnvMapFread(fp,&map,&grd);
  else  status=CnvMapFread(fp,&map,&grd);

  if (stime !=-1) { /* we must skip the start of the files */
    int yr,mo,dy,hr,mt;
    double sc;  

    if (stime==-1) stime= ( (int) map.st_time % (24*3600));
    if (sdate==-1) stime+=map.st_time - ( (int) map.st_time % (24*3600));
    else stime+=sdate;

    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (old) status=OldCnvMapFseek(fp,yr,mo,dy,hr,mt,sc,NULL,NULL);
    else status=CnvMapFseek(fp,yr,mo,dy,hr,mt,sc,NULL,NULL);
    if (status ==-1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (old) status=OldCnvMapFread(fp,&map,&grd);
    else  status=CnvMapFread(fp,&map,&grd);
  } else stime=grd.st_time;



  if (etime !=-1) {
    if (edate==-1) etime+=grd.st_time - ( (int) grd.st_time % (24*3600));
    else etime+=edate;
  } 
  if (extime !=0)  etime=stime+extime;

  if ((land==1) && (wdt== WIDTH) && (hgt==HEIGHT)) {
    int t;
    t=hgt;
    hgt=wdt;
    wdt=t;
  } 

  phgt=hgt-yoff;
  pwdt=wdt;

  if (sflg==1) {
     ynum++;  
     tpad=15;
     bpad=15;
     phgt-=20;
     xttl_flg=0x02;
     xlbl_flg=0x02;
  } 

  if (bflg==1) {
     ynum+=3;  
     tpad=8;
     bpad=8;
     xttl_flg=0x02;
     xlbl_flg=0x02;
     if (sflg==0) phgt-=25;
     else phgt-=5;
     yoff+=5;
  } 

  xmax=etime-stime;
 
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

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(name,"%.4d%.2d%.2d.pc",yr,mo,dy);

  RplotMakePlot(rplot,name,wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);
  RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);

  plt=PlotMake(pwdt,phgt,xnum,ynum,lpad,rpad,bpad,tpad,xoff,yoff);
  PlotSetRplot(plt,rplot);
  PlotSetTextBox(plt,txtbox,fontdb);  

  plt->major_hgt=8;
  plt->minor_hgt=5;
  plt->ttl_hgt=20;
  plt->lbl_hgt=10;

  plt->ttl_wdt=30;
  plt->lbl_wdt=10;
  

  PlotPanel(plt,0,fgcol,0x0f,lne);
  PlotXaxis(plt,0,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);

  PlotXaxisLabel(plt,0,xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  PlotXaxisTitle(plt,0,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);

  PlotYaxis(plt,0,ypmin,ypmax,ypmajor,ypminor,ytick_flg,fgcol,0x0f,lne);

  PlotYaxisLabel(plt,0,ypmin,ypmax,ypmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
  PlotYaxisTitle(plt,0,yttl_flg,strlen(yptitle),yptitle,"Helvetica",
                 10.0,fgcol,0x0f);

  if (ynum==2) {
    xttl_flg=0x01; 
    xlbl_flg=0x01;

    PlotPanel(plt,1,fgcol,0x0f,lne);
    PlotXaxis(plt,1,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);

    PlotXaxisLabel(plt,1,xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    PlotXaxisTitle(plt,1,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);

    PlotYaxis(plt,1,ysmin,ysmax,ysmajor,ysminor,ytick_flg,fgcol,0x0f,lne);

    PlotYaxisLabel(plt,1,ysmin,ysmax,ysmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    PlotYaxisTitle(plt,1,yttl_flg,strlen(ystitle),ystitle,"Helvetica",
                 10.0,fgcol,0x0f);

  }
  if (ynum>2) {
    xttl_flg=0x01;
    xlbl_flg=0x01;
    for (i=1;i<ynum;i++) {
      PlotPanel(plt,i,fgcol,0x0f,lne);
      PlotXaxis(plt,i,xmin,xmax,xmajor,xminor,xtick_flg,fgcol,0x0f,lne);
    }
    PlotXaxisLabel(plt,ynum-1,
                   xmin+stime,xmax+stime,xmajor,xlbl_flg,xlabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    PlotXaxisTitle(plt,ynum-1,xttl_flg,strlen(xtitle),xtitle,"Helvetica",
                 10.0,fgcol,0x0f);
    i=1;
    if (sflg) {
      PlotYaxis(plt,i,ysmin,ysmax,ysmajor,ysminor,ytick_flg,fgcol,0x0f,lne);

      PlotYaxisLabel(plt,i,ysmin,ysmax,ysmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
      PlotYaxisTitle(plt,i,yttl_flg,strlen(ystitle),ystitle,"Helvetica",
                 10.0,fgcol,0x0f);
      i++;
    }
    PlotYaxis(plt,i,bxmin,bxmax,bxmajor,bxminor,ytick_flg,fgcol,0x0f,lne);
    PlotYzero(plt,i,bxmin,bxmax,fgcol,0x0f,lne,NULL);
    PlotYaxisLabel(plt,i,bxmin,bxmax,bxmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    PlotYaxisTitle(plt,i,yttl_flg,strlen(bxtitle),bxtitle,"Helvetica",
                 10.0,fgcol,0x0f);

    PlotYaxis(plt,i+1,bymin,bymax,bymajor,byminor,ytick_flg,fgcol,0x0f,lne);
    PlotYzero(plt,i+1,bymin,bymax,fgcol,0x0f,lne,NULL);
    PlotYaxisLabel(plt,i+1,bymin,bymax,bymajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    PlotYaxisTitle(plt,i+1,yttl_flg,strlen(bytitle),bytitle,"Helvetica",
                 10.0,fgcol,0x0f);

    PlotYaxis(plt,i+2,bzmin,bzmax,bzmajor,bzminor,ytick_flg,fgcol,0x0f,lne);
    PlotYzero(plt,i+2,bzmin,bzmax,fgcol,0x0f,lne,NULL);
    PlotYaxisLabel(plt,i+2,bzmin,bzmax,bzmajor,ylbl_flg,ylabel,NULL,
                 "Helvetica",10.0,fgcol,0x0f);
    PlotYaxisTitle(plt,i+2,yttl_flg,strlen(bztitle),bztitle,"Helvetica",
                 10.0,fgcol,0x0f);


  }

  plt->ttl_hgt=40;

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(title,"%.4d%.2d%.2d",yr,mo,dy);
  PlotXaxisTitle(plt,0,0x02,strlen(title),title,"Helvetica",
                 20.0,fgcol,0x0f);
  do {
    tval[count]=map.st_time;
    mtime=map.st_time;
    TimeEpochToYMDHMS(mtime,&yr,&mo,&dy,&hr,&mt,&sc);
    
    if (vb==1) 
      fprintf(stderr,"%.2d-%.2d-%.4d %.2d:%.2d\n",dy,mo,dy,hr,mt);

    polar[count]=map.pot_drop;
    scatter[count]=grd.vcnum;
    bx[count]=map.Bx;
    by[count]=map.By;
    bz[count]=map.Bz;
    delayval[count]=map.imf_delay;

    count++;
    if (mtime>etime) break;
    if (old) status=OldCnvMapFread(fp,&map,&grd);
    else  status=CnvMapFread(fp,&map,&grd);

  } while (status !=-1);


  ox=0;
  oy=polar[0]/1000;
  for (j=1;j<count;j++) {
    px=tval[j]-stime;
    py=polar[j]/1000;
    PlotLine(plt,0,ox,oy,px,py,xmin,xmax,ypmin,ypmax,fgcol,0x0f,lne,NULL);
    ox=px;
    oy=py;

  }
  i=1;
  if (sflg==1) {
    ox=0;
    oy=scatter[0];
    for (j=1;j<count;j++) {
      px=tval[j]-stime;
      py=scatter[j];
      PlotLine(plt,1,ox,oy,px,py,xmin,xmax,ysmin,ysmax,fgcol,0x0f,lne,NULL);
      ox=px;
      oy=py;
    }
    i=2;
  } 

  if (bflg==1) {
    if (nodelay==0) {
      ox=0;
      oy=bx[0];
      for (j=1;j<count;j++) {
        px=tval[j]-stime;
        py=bx[j];
        PlotLine(plt,i,ox,oy,px,py,xmin,xmax,bxmin,bxmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=by[0];
      for (j=1;j<count;j++) {
        px=tval[j]-stime;
        py=by[j];
        PlotLine(plt,i+1,ox,oy,px,py,xmin,xmax,
                 bymin,bymax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=bz[0];
      for (j=1;j<count;j++) {
        px=tval[j]-stime;
        py=bz[j];
        PlotLine(plt,i+2,ox,oy,px,py,xmin,xmax,
                 bzmin,bzmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
    } else {
      int off=0;
      for (j=0;j<count;j++) if (((tval[j]-stime)/60)>delayval[0]) break;
      off=j;
      ox=0;
      oy=bx[off];
      for (j=off+1;j<count;j++) {
        px=tval[j]-60*delayval[j]-stime;
        py=bx[j];
        PlotLine(plt,i,ox,oy,px,py,xmin,xmax,
                 bxmin,bxmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=by[off];
      for (j=off+1;j<count;j++) {
        px=tval[j]-60*delayval[j]-stime;
        py=by[j];
        PlotLine(plt,i+1,ox,oy,px,py,xmin,xmax,
                 bymin,bymax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }
      ox=0;
      oy=bz[off];
      for (j=off+1;j<count;j++) {
        px=tval[j]-60*delayval[j]-stime;
        py=bz[j];
        PlotLine(plt,i+2,ox,oy,px,py,xmin,xmax,
                 bzmin,bzmax,fgcol,0x0f,lne,NULL);
        ox=px;
        oy=py;
      }

    }
  }

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


