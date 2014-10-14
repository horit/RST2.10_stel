/* istp_plot.c
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

/* 
 $Log: istp_plot.c,v $
 Revision 1.34  2005/08/08 15:27:45  barnes
 Removed redundant command line option.

 Revision 1.33  2005/08/05 18:42:41  barnes
 Added command line option to dump the possible command line options.

 Revision 1.32  2004/05/04 19:16:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.31  2004/04/27 18:24:08  barnes
 Audit - Enforced warning set to "all".

 Revision 1.30  2003/08/06 22:19:02  barnes
 Changed library calls.

 Revision 1.29  2003/03/14 20:15:05  barnes
 Included a name for the plot.

 Revision 1.28  2003/03/14 18:23:49  barnes
 Added support for the extended PPM format.

 Revision 1.27  2003/03/13 12:53:01  barnes
 Changed the way information is recorded in the metadata.

 Revision 1.26  2003/03/13 04:25:57  barnes
 Added version information to the metadata.

 Revision 1.25  2003/03/13 03:48:42  barnes
 Record creation date in UT rather than local time.

 Revision 1.24  2003/03/13 03:43:17  barnes
 Added metadata to the plot.

 Revision 1.23  2003/03/09 17:31:05  barnes
 Changed CDF header name.

 Revision 1.22  2003/03/07 03:01:26  barnes
 Changed the command line arguments.

 Revision 1.21  2003/03/06 20:28:20  barnes
 Modification to the headers of the plot library.

 Revision 1.20  2003/02/28 16:21:40  barnes
 Removed the RplotClear command.

 Revision 1.19  2003/02/28 00:26:56  barnes
 Added support for multiple output types: Rplot, PPM, PS, X window, XML image.

 Revision 1.18  2003/02/27 23:47:46  barnes
 Major modifications to use the base libraries and to output as Rplot XML.

 Revision 1.17  2002/12/03 15:10:19  barnes
 Added code to use the h0 IMF data.

 Revision 1.16  2001/12/07 21:26:16  barnes
 Added the ability to select the parameters to plot.

 Revision 1.15  2001/06/27 20:57:09  barnes
 Added license tag

 Revision 1.14  2001/01/29 18:12:10  barnes
 Added Author Name

 Revision 1.13  2000/10/20 16:35:35  barnes
 Fixed bug in labelling date.

 Revision 1.12  2000/08/16 22:08:26  barnes
 Changed return type of main().

 Revision 1.11  2000/08/13 17:14:13  barnes
 Changed some of the labelling.

 Revision 1.10  2000/08/13 14:58:51  barnes
 Fixed bug in setting landscape mode.

 Revision 1.9  2000/08/02 23:39:51  barnes
 Modified the way X is implemented so that the code compiles on systems
 that do not support it.

 Revision 1.8  2000/07/26 18:52:06  barnes
 Added X term support.

 Revision 1.7  2000/07/11 18:54:54  barnes
 Fixed bug in reading the help message.

 Revision 1.6  2000/06/26 15:09:48  barnes
 Added labelling of the position
 and date markers.

 Revision 1.5  2000/06/25 21:02:09  barnes
 Fixed plot labelling.

 Revision 1.4  2000/06/25 20:47:43  barnes
 Fixed bugs in the locate algorithm

 Revision 1.3  2000/06/25 01:10:33  barnes
 Fixed bug in the extent option.

 Revision 1.2  2000/06/24 20:28:22  barnes
 Added the ability to set the data path on the command line.

 Revision 1.1  2000/06/24 20:08:24  barnes
 Initial revision

 */

/* This is a simple test program to demonstrate how to read a CDF file */

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>

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

#include "cdf.h"
#include "rcdf.h"

#include "locate.h"

#include "istp.h"
#include "wind.h"
#include "ace.h"
#include "imp.h"
#include "geotail.h"

#include "graphic.h"

#include "errstr.h"
#include "hlpstr.h"
#include "version.h"

#define ONE_MONTH (28*24*3600)

#define WIDTH (558-58)
#define HEIGHT (734-58)
  
struct RCDFData data[10];

struct imfdata imf;
struct posdata pos;
struct plasmadata plasma;

unsigned char mfi=0;
unsigned char swe=0;
unsigned char mag=0;
unsigned char pla=0;
unsigned char mgf=0;
unsigned char lep=0;
unsigned char cpi=0;

char sat[256];
char inst[256];

char dpath[256]={"/data"};
 
double st_time=-1;
double ed_time=-1;

struct file_list *fptr;

struct OptionData opt;
struct OptionFile *optf=NULL;

int plasma_f[4]={1,1,1,1};
int imf_f[4]={1,1,1,1};

int plasma_n=4;
int imf_n=4;

char *imf_s[]={"Bm","Bx","By","Bz",0};
char *plasma_s[]={"P","Vx","Vy","Vz",0};

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
  return hr*3600L+mn*60L;
}  


int load_wind() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"WIND");
  sprintf(path,"%s/%s",dpath,"wind");

  if ((mfi+swe)==0) mfi=1; /* default to imf if not given */

  if (swe==1) {
    strcpy(inst,"SWE");
    fptr=locate_files(path,"swe",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      status=windswe_pos(id,&pos,st_time,ed_time); 
      status=windswe_plasma(id,&plasma,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }


  if (mfi==1) {
    if (swe==0) strcpy(inst,"MFI");
    else strcat(inst,"+MFI");
    fptr=locate_files(path,"mfi",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (swe==0) status=windmfi_pos(id,&pos,st_time,ed_time); 
      status=windmfi_imf(id,&imf,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
}
 
int load_ace() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 
  if ((mfi+swe)==0) mfi=1; /* default to imf if not given */

  sprintf(sat,"ACE");
  sprintf(path,"%s/%s",dpath,"ace");
 

  if (swe==1) strcpy(inst,"SWE");
 
  fptr=locate_files(path,"h0_swe",st_time,ed_time);

  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

    status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
       fprintf(stderr,"Could not open cdf file.\n");
       continue;
    }
    status=aceswe_pos(id,&pos,st_time,ed_time); 
    if (swe==1) status=aceswe_plasma(id,&plasma,st_time,ed_time);
    CDFclose(id);
  }
  free_locate(fptr);
  if (mfi==1) {
    if (swe==0) strcpy(inst,"MFI");
    else strcat(inst,"+MFI");

    fptr=locate_files(path,"h0_mfi",st_time,ed_time);

      if (fptr->cnt !=0) {
       for (i=0;i<fptr->cnt;i++) {
         fprintf(stderr,"%s\n",fptr->fname[i]);
         status=CDFopen(fptr->fname[i],&id);
         if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
        status=acemfi_imf(id,&imf,st_time,ed_time);
    
        CDFclose(id);
      }
      free_locate(fptr);
    } else {
      free_locate(fptr);    
      fptr=locate_files(path,"k1_mfi",st_time,ed_time);

      for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

         status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
    
        status=acemfi_imf(id,&imf,st_time,ed_time);
    
        CDFclose(id);
      }
      free_locate(fptr);
    }
  }
  return 0;
}

int load_imp() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"IMP8");
  sprintf(path,"%s/%s",dpath,"imp8");

  if ((mag+pla)==0) mag=1; /* default to imf if not given */

  if (pla==1) {

    strcpy(inst,"PLA");
   
    fptr=locate_files(path,"pla",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
  
      status=imppla_pos(id,&pos,st_time,ed_time); 
      status=imppla_plasma(id,&plasma,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  if (mag==1) {
    if (pla==0) strcpy(inst,"MAG");
    else strcat(inst,"+MAG");
    fptr=locate_files(path,"mag",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (pla==0) status=impmag_pos(id,&pos,st_time,ed_time); 
      status=impmag_imf(id,&imf,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  return 0;
}
 
int load_geo() {
  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 

  sprintf(sat,"GEOTAIL");
  sprintf(path,"%s/%s",dpath,"geotail");

  if ((mgf+lep+cpi)==0) mgf=1; /* default to imf if not given */

  if (lep==1) strcpy(inst,"LEP");

  if ((lep==1) || (mgf==0)) {
    fptr=locate_files(path,"lep",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
        fprintf(stderr,"%s\n",fptr->fname[i]);

        status=CDFopen(fptr->fname[i],&id);
        if (status !=CDF_OK) {
          fprintf(stderr,"Could not open cdf file.\n");
          continue;
        }
  
        status=geolep_pos(id,&pos,st_time,ed_time);
        if (lep==1) status=geolep_plasma(id,&plasma,st_time,ed_time);
        CDFclose(id);
      }
      free_locate(fptr);
  }

  if (cpi==1) {

    if (lep==0) strcpy(inst,"CPI");
    else strcat(inst,"+CPI");
    fptr=locate_files(path,"cpi",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
  
      status=geocpi_plasma(id,&plasma,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }

  if (mgf==1) {
    if ((lep+cpi)==0) strcpy(inst,"MGF");
    else strcat(inst,"+MGF");
    fptr=locate_files(path,"mgf",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
   
      if (lep==0) status=geomgf_pos(id,&pos,st_time,ed_time); 
      status=geomgf_imf(id,&imf,st_time,ed_time);
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
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

  char *fntdbfname;
  struct FrameBufferFontDB *fontdb=NULL;
  unsigned int bgcol;
  unsigned int fgcol;
  float lne=0.5;

  int arg;
  char *envstr;
  char *cfname=NULL;
  FILE *fp;

  unsigned char wind=0;
  unsigned char ace=0;
  unsigned char imp=0;
  unsigned char geo=0;
  unsigned char posf=0;
  unsigned char help=0;
  unsigned char option=0;

  char *st_time_txt=NULL;
  char *extime_txt=NULL;
  char *ed_time_txt=NULL;
  char *sdate_txt=NULL;
  char *edate_txt=NULL;
  char *dpath_txt=NULL;

  double extime=-1;
  double sdate=-1;
  double edate=-1;

  char *plot=NULL;
  
  int yr,mo,dy,hr,mt;
  double sc;

  unsigned char mode=0; 
  int pnum=0;
  int p=0;
  int dflg=0;
  
  unsigned char land=0;
  int wdt=WIDTH,hgt=HEIGHT;

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

  char name[256];  

  envstr=getenv("ISTP_PATH");
  if (envstr !=NULL) strcpy(dpath,envstr);


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

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

  OptionAdd(&opt,"w",'i',&wdt);
  OptionAdd(&opt,"h",'i',&hgt);

  OptionAdd(&opt,"st",'t',&st_time_txt);
  OptionAdd(&opt,"et",'t',&ed_time_txt);
  OptionAdd(&opt,"ex",'t',&extime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);

  OptionAdd(&opt,"gse",'x',&mode);

  OptionAdd(&opt,"pos",'x',&posf);
  OptionAdd(&opt,"mfi",'x',&mfi);
  OptionAdd(&opt,"swe",'x',&swe);
  OptionAdd(&opt,"mag",'x',&mag);
  OptionAdd(&opt,"pla",'x',&pla);
  OptionAdd(&opt,"mgf",'x',&mgf);
  OptionAdd(&opt,"lep",'x',&lep);
  OptionAdd(&opt,"cpi",'x',&cpi);

  OptionAdd(&opt,"plot",'t',&plot);
  OptionAdd(&opt,"path",'t',&dpath_txt);

  OptionAdd(&opt,"ace",'x',&ace);
  OptionAdd(&opt,"wind",'x',&wind);
  OptionAdd(&opt,"imp8",'x',&imp);
  OptionAdd(&opt,"geotail",'x',&geo);
  OptionAdd(&opt,"cf",'t',&cfname);

  if (argc>1) { 
    arg=OptionProcess(1,argc,argv,&opt,NULL);   
    if (cfname !=NULL) { /* load the configuration file */
      do {
        fp=fopen(cfname,"r");
        if (fp==NULL) break;
        free(cfname);
        cfname=NULL;
        optf=OptionProcessFile(fp);
        if (optf !=NULL) {
          arg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
          OptionFreeFile(optf);
	}   
        fclose(fp);
      } while (cfname !=NULL);
    }
  } else {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (plot !=NULL) {
    char *tok;
    int i,j;
    for (i=0;i<4;i++) {
       plasma_f[i]=0;
       imf_f[i]=0;
    }
    plasma_n=0;
    imf_n=0;
    tok=strtok(plot,",");
    do {
      for (j=0;plasma_s[j] !=NULL;j++) 
        if (strcasecmp(plasma_s[j],tok)==0) break;
      if (plasma_s[j] !=NULL) {
        plasma_f[j]=1;
        plasma_n++;
      }
     

      for (j=0;imf_s[j] !=NULL;j++) 
        if (strcasecmp(imf_s[j],tok)==0) break;
      if (imf_s[j] !=NULL) {
        imf_f[j]=1;
        imf_n++;
      }
    } while ((tok=strtok(NULL,",")) !=NULL);
  } 
    


  if ((ace+wind+imp+geo)==0) ace=1;

  if ((ace+wind+imp+geo)>1) { 
    fprintf(stderr,"Only one satellite can be specified.\n");
    exit(-1);
  }

  if (dpath_txt !=NULL) strcpy(dpath,dpath_txt);


  if (st_time_txt !=NULL) st_time=strtime(st_time_txt);
  if (ed_time_txt !=NULL) ed_time=strtime(ed_time_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL) sdate=strdate(sdate_txt);
  if (edate_txt !=NULL) edate=strdate(edate_txt);

  if (sdate==-1) sdate=TimeYMDHMSToEpoch(1997,1,1,0,0,0);
  if (st_time==-1) st_time=0;

  st_time+=sdate;

  if (edate==-1) edate=sdate; 
  if (ed_time==-1) ed_time=0;  

  ed_time+=edate;
  if (extime !=-1) ed_time=st_time+extime;


  if (ed_time<=st_time) ed_time=st_time+(24*3600); /* plot a day */
    
  TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(name,"%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",yr,mo,dy,hr,mt,(int) sc,sat);
  fprintf(stderr,"Start time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
  TimeEpochToYMDHMS(ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stderr,"End time:%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);


 if ((ed_time-st_time)>=(48*3600)) dflg=1;
 
 
  if (ace==1) load_ace();
  else if (wind==1)load_wind();
  else if (imp==1) load_imp();
  else if (geo==1) load_geo();
 

  if (posf==0) pos.cnt=0;
  
  pnum=(pos.cnt>0)+imf_n*(imf.cnt>0)+plasma_n*(plasma.cnt>0);
  
  if (pnum==0) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }
  
  TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(name,"%.4d%.2d%.2d.%.2d%.2d.%.2d.%s",yr,mo,dy,hr,mt,(int) sc,sat);
 

  if ((land==1) && (wdt== WIDTH) && (hgt==HEIGHT)) {
    int t;
    t=hgt;
    hgt=wdt;
    wdt=t;
  } 

  fntdbfname=getenv("FONTDB");
  fp=fopen(fntdbfname,"r");
  if (fp !=NULL) {
   fontdb=FrameBufferFontDBLoad(fp);
   fclose(fp);
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

  plt=PlotMake(wdt,hgt-60-(10*dflg),1,pnum,40,40,5,5,0,30);
  PlotSetRplot(plt,rplot);
  PlotSetTextBox(plt,txtbox,fontdb);  

 
  plt->major_hgt=4;
  plt->minor_hgt=2;
  plt->lbl_hgt=8;
  plt->ttl_hgt=20+(10*dflg);
 
  plt->ttl_wdt=30;
  plt->lbl_wdt=10;
 
  
  plot_frame(plt,pnum,st_time,ed_time,dflg,fgcol,lne,"Helvetica",10.0); 
  
  if (pos.cnt !=0) {
    plot_pos(plt,p,&pos,st_time,ed_time,mode,fgcol,lne,"Helvetica",10.0);   
    p++;
  } 
  
  if (plasma.cnt !=0) {
     plot_plasma(plt,p,&plasma,st_time,ed_time,mode,plasma_f,fgcol,lne,
                 "Helvetica",10.0);  
     p+=plasma_n;;
  }
  
  if (imf.cnt !=0) {
     plot_imf(plt,p,&imf,st_time,ed_time,mode,imf_f,fgcol,lne,
              "Helvetica",10.0); 
     p+=imf_n;
  }
  
  plt->ttl_hgt=6;
  plot_title(plt,st_time,ed_time,sat,inst,mode,fgcol,lne,"Helvetica",12.0);  
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
   


















