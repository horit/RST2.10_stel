/* vec_plot.c
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

#ifdef _XLIB_
#include "xwin.h"
#endif
#include "rtime.h"
#include "rmap.h"
#include "aacgm.h"

#include "radar.h"
#include "calcvector.h"

#include "text_box.h"
#include "plot_circle.h"
#include "plot_radial.h"
#include "plot_time.h"

#include "errstr.h"
#include "hlpstr.h"
#include "version.h"

/*
 $Log: vec_plot.c,v $
 Revision 1.29  2005/08/10 15:38:56  barnes
 Added option to dump the command line options.

 Revision 1.28  2004/05/10 21:58:59  barnes
 Removed redundant code.

 Revision 1.27  2004/04/27 22:49:56  barnes
 Audit - Enforced warning set to "all".

 Revision 1.26  2003/04/10 17:46:16  barnes
 Changed window name.

 Revision 1.25  2003/03/14 20:17:27  barnes
 Included a name for the plot.

 Revision 1.24  2003/03/14 18:45:15  barnes
 Modification to support extended PPM output.

 Revision 1.23  2003/03/13 23:40:49  barnes
 Added version number to metadata.

 Revision 1.22  2003/03/13 14:37:14  barnes
 Record information (metadata) in the Rplot XML document.

 Revision 1.21  2003/03/09 17:37:21  barnes
 Changed AACGM header name.

 Revision 1.20  2003/03/07 03:06:11  barnes
 Changed command line arguments.

 Revision 1.19  2003/03/01 03:20:46  barnes
 Major modifications to support the base libraries and Rplot XML output.

 Revision 1.18  2001/06/27 21:08:56  barnes
 Added license tag

 Revision 1.17  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.16  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.15  2000/08/16 22:22:48  barnes
 Changed return type of main().

 Revision 1.14  2000/08/06 21:35:48  barnes
 Fixed bug in decoding X flag.

 Revision 1.13  2000/08/02 23:39:07  barnes
 Fixed bug in positioning code.

 Revision 1.12  2000/08/02 22:24:53  barnes
 Added the ability to set the plot position.

 Revision 1.11  2000/07/31 13:57:41  barnes
 Changed the way X is implemented so that the code can be compiled on a
 system that does not support it.

 Revision 1.10  2000/07/26 18:44:56  barnes
 Added X term support.

 Revision 1.9  2000/07/24 15:15:15  barnes
 Function name conflict.

 Revision 1.8  1999/10/22 16:53:47  barnes
 Added the ability to read input from standard input.

 Revision 1.7  1999/10/22 15:09:16  barnes
 Modifications to deal with concatenated files.

 Revision 1.6  1999/10/01 14:47:16  barnes
 Modified to use the new gplot library and command line options.

 Revision 1.5  1999/07/28 16:17:39  barnes
 Added the logo.

 Revision 1.4  1999/06/15 18:07:22  barnes
 Changed text_ps_box to float.

 Revision 1.3  1999/04/10 18:58:56  barnes
 Added help message system.

 Revision 1.2  1998/11/19 15:27:40  barnes
 Added the ability to produce multi-panel plots.

 Revision 1.1  1998/11/12 18:10:38  barnes
 Initial revision

 */

struct OptionData opt;
struct OptionFile *optf=NULL;

struct vec {
  float mlon,mlat;
  int type,qflg;
  float Veast,Vpole,VEeast,VEpole;
};

struct vecdata {
  char name[64];
  double st_time;
  double ed_time;
  int npnt;
  struct vec *data;
};
  

double mlon_av_val=0;
int mlon_av_cnt=0;
struct vecdata vec;
int gcnt[360/5][90-60];

struct vec *grid[360/5][90-60];

char *dte_fnt_nme={"Helvetica"};
float dte_fnt_sze=8.0;

char *tme_fnt_nme={"Helvetica"};
float tme_fnt_sze=8.0;

char *lgo_fnt_nme={"Helvetica"};
float lgo_fnt_sze=14.0;

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



void label_logo(struct Rplot *rplot,
	        float xoff,float yoff,float wdt,float hgt,
                char *fontname,float fontsize,unsigned int color,
                unsigned char mask,
                void *txtdata) {
  float txbox[3];
  char txt[256]; 
  sprintf(txt,"SuperDARN");

  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  RplotText(rplot,NULL,fontname,fontsize,xoff,yoff+txbox[2],strlen(txt),txt,
               color,mask,1);

}

void label_web(struct Rplot *rplot,
	        float xoff,float yoff,float wdt,float hgt,
                char *fontname,float fontsize,unsigned int color,
	        unsigned char mask,
                void *txtdata) {
  float txbox[3];
  char txt[256]; 
  sprintf(txt,"http://superdarn.jhuapl.edu");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  RplotText(rplot,NULL,fontname,fontsize,xoff+wdt-txbox[0],
            yoff+txbox[2],strlen(txt),txt,
            color,mask,1);
}


int vec_sort_east(const void *a,const void *b) {
  struct vec *x;
  struct vec *y;
  x=(struct vec *) a;
  y=(struct vec *) b;

  if (x->Veast < y->Veast) return -1;
  if (x->Veast > y->Veast) return 1;
  return 0;

}

int vec_sort_pole(const void *a,const void *b) {
  struct vec *x;
  struct vec *y;
  x=(struct vec *) a;
  y=(struct vec *) b;

  if (x->Vpole < y->Vpole) return -1;
  if (x->Vpole > y->Vpole) return 1;
  return 0;

}

int read_record(FILE *fp,struct vecdata *ptr) {
  char line[1024];
  int yr,mo,dy,hr,mt,sc,syrsec,eyrsec;
  int mlat,mlon,type,qflg,Veast,Vpole,VEeast,VEpole;

  if (fgets(line,256,fp)==0) return -1;  

  if (strncmp(line,"vlptm",5)==0) {
    /* header record */
    char *tkn;
    tkn=strtok(line," ");
    tkn=strtok(NULL," ");
    tkn=strtok(NULL," ");
    tkn=strtok(NULL,"\n");
    if (tkn !=NULL) {
      int i;
      for (i=0;(tkn[i]==' ') && (tkn[i] !=0);i++);
      if (tkn[i] !=0) strcpy(ptr->name,tkn+i); 
    }
    if (fgets(line,256,fp)==0) return -1;
    if (fgets(line,256,fp)==0) return -1;
    if (fgets(line,256,fp)==0) return -1;
    if (fgets(line,256,fp)==0) return -1;
    return 1;
  }
  
  if (sscanf(line,"%d %d %d %d %d %d %d %d",&yr,&mo,&dy,&hr,&mt,&sc,
                    &syrsec,&eyrsec) !=8) return -1;

   
  TimeYrsecToYMDHMS(syrsec,yr,&mo,&dy,&hr,&mt,&sc);
  ptr->st_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  TimeYrsecToYMDHMS(eyrsec,yr,&mo,&dy,&hr,&mt,&sc);
  ptr->ed_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  

  if (fgets(line,256,fp)==0) return -1;
  if (fgets(line,256,fp)==0) return -1;
  if (fgets(line,256,fp)==0) return -1;
 
  if (sscanf(line,"%d %d %d %d %d %d %d %d",&mlat,&mlon,&type,&qflg,
	      &Veast,&Vpole,&VEeast,&VEpole) !=8) return -1;
  
  ptr->npnt=0; 
  if (ptr->data !=NULL) {
    free(ptr->data);
    ptr->data=NULL;
  }
  
  while ((Veast !=0) || (Vpole !=0) || (type !=0) || (qflg !=0) || 
	 (mlat !=0) || (mlon !=0)) {
    
   if (ptr->data==NULL) ptr->data=malloc(sizeof(struct vec));
   else ptr->data=realloc(ptr->data,(ptr->npnt+1)*sizeof(struct vec));

   ptr->data[ptr->npnt].mlat=mlat/100.0;
   ptr->data[ptr->npnt].mlon=mlon/100.0;
   ptr->data[ptr->npnt].type=type;
   ptr->data[ptr->npnt].qflg=qflg;
   ptr->data[ptr->npnt].Veast=Veast/10.0;
   ptr->data[ptr->npnt].Vpole=Vpole/10.0;
   ptr->data[ptr->npnt].VEeast=VEeast/10.0;
   ptr->data[ptr->npnt].VEpole=VEpole/10.0;
   ptr->npnt++;
    
   if (fgets(line,256,fp)==0) return -1;

   if (sscanf(line,"%d %d %d %d %d %d %d %d",&mlat,&mlon,&type,&qflg,
	      &Veast,&Vpole,&VEeast,&VEpole) !=8) return -1;
   
  } 
 
  return 0;
}


void filter(struct vecdata *vec,int qlevel) {
  int i,j,k,c;
  float lon,lat;
  struct vec *ptr=NULL;
  lon=(360.0*((int) vec->st_time % (24*3600)))/(24*3600);
  j=lon/5;
  if (j<0) return;
  if (j>=360/5) return;

  for (k=0;k<vec->npnt;k++) {
    if (vec->data[k].qflg>qlevel) continue;
    lat=vec->data[k].mlat;
    i=lat-60;
    if (i<0) continue;
    if (i>=40) continue;
    ptr=grid[j][i];
    mlon_av_val+=vec->data[k].mlon;
    mlon_av_cnt++;
    if (ptr==NULL) grid[j][i]=malloc(sizeof(struct vec));
    else grid[j][i]=realloc(grid[j][i],(gcnt[j][i]+1)*sizeof(struct vec));
    ptr=grid[j][i];
    c=gcnt[j][i];
    memcpy(&ptr[c],&(vec->data[k]),sizeof(struct vec));
    gcnt[j][i]++;
  }
}

void free_filter() {
  int i,j;
  for (i=0;i<(360/5);i++) {
    for (j=0;j<(90-60);j++) {
      gcnt[i][j]=0;
      if (grid[i][j] !=NULL) {
        free(grid[i][j]);
        grid[i][j]=NULL;
      }
    }
  }
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
  char *wname="vec_plot";
  int xdoff=-1;
  int ydoff=-1;
#endif

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
  unsigned int grdcol;
  unsigned int txtcol;
  float lne=0.5;

  int arg;
  int fc;
  char *fname=NULL;
  char *cfname=NULL;
  FILE *fp;
  int status=0;
 
  float xoff=0,yoff=0;
  float wdt=600,hgt=600;
  float pad=15;
  float tpad=0;
  float pwdt=0;
  int npanel=0;

  float txbox[3];

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
  unsigned char logo=0;

  char *bgcol_txt=NULL;
  char *txtcol_txt=NULL;
  char *grdcol_txt=NULL;

  float marg[8]={0,0,0,0};
  int c=0;

  int yr,mo,dy,hr,mt;
  double sc;
  double mlt;
  int yrsec;
  double stime=0;


  int cnt=0;
  char str[256];
  char station[256];
  struct vec *ptr=NULL;
  double lat,lon,mag,azm;
  float Vpole,Veast;
  float scale=2.0; 
  int i,j;
  
  float map[2];
  float pnt[2];
  float ax,ay,bx,by;

  bgcol=RplotColor(255,255,255,255);
  txtcol=RplotColor(0,0,0,255);
  grdcol=RplotColor(128,128,128,255);
 
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
 
  OptionAdd(&opt,"logo",'x',&logo);

  OptionAdd(&opt,"p",'i',&npanel);

  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
  OptionAdd(&opt,"txtcol",'t',&txtcol_txt);
  OptionAdd(&opt,"grdcol",'t',&grdcol_txt);

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
    OptionPrintInfo(stdout,errstr);
    exit(0);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }
 

 
  if (bgcol_txt !=NULL) bgcol=RplotColorStringRGB(bgcol_txt);
  if (txtcol_txt !=NULL) txtcol=RplotColorStringRGB(txtcol_txt);
  if (grdcol_txt !=NULL) grdcol=RplotColorStringRGB(grdcol_txt);


  if (npanel==0) npanel=argc-arg; 
  if (logo) tpad=20;

  if (npanel>1) pwdt=wdt/2;
  else pwdt=wdt;
  if (npanel>1) hgt=pwdt*(1+(npanel-1)/2)+tpad;
  else hgt=pwdt;

  if (pwdt>400) {
    pad=30;
    tme_fnt_sze=14.0;
    dte_fnt_sze=20.0;
   } else if (wdt>200) {
    tme_fnt_sze=8.0;
    dte_fnt_sze=10.0;
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

  RplotMakePlot(rplot,"vec.plot",wdt,hgt,24);
  RplotStartInfo(rplot);
  record_info(rplot,argc,argv);
  RplotEndInfo(rplot);
  RplotRectangle(rplot,NULL,0,0,wdt,hgt,1,bgcol,0x0f,0,NULL);
 
  if (logo==1) {
    label_web(rplot,0,0,wdt,tpad,tme_fnt_nme,tme_fnt_sze,txtcol,0x0f,fontdb);
    label_logo(rplot,0,0,wdt,tpad,lgo_fnt_nme,lgo_fnt_sze,txtcol,0x0f,fontdb);
  }

  marg[0]=90.0;
  marg[1]=0;
  marg[2]=0.8*90.0/(90-50.0);

  c=0;
  for (fc=arg;fc<argc;fc++) {
    fname=argv[fc];
    if (strcmp(fname,"stdin")==0) fp=stdin;
    else {
      fp=fopen(fname,"r");
      if (fp==NULL) continue;
      fprintf(stderr,"Plotting:%s\n",fname);
    }

    
    status=read_record(fp,&vec);
    if (status !=1) {
      fprintf(stderr,"Error reading file.\n");
      continue;
    }

    do {

      xoff=(c % 2)*pwdt;
      yoff=(c / 2)*pwdt+tpad;
     
      cnt=0;    
      
      strcpy(station,vec.name);
     
      while ( (status=read_record(fp,&vec))==0) {
        if (cnt==0) stime=vec.st_time;
		
        filter(&vec,5);
	
        cnt++;
      }
      

      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      
      plot_circle(rplot,marg,60.0,90.0,10.0,
                  xoff+pad,yoff+pad,pwdt-2*pad,pwdt-2*pad,
                  grdcol,0x0f,lne,NULL);
      plot_radial(rplot,marg,60,0,359,45,xoff+pad,yoff+pad,pwdt-2*pad,
                  pwdt-2*pad,grdcol,0x0f,lne,NULL);
      
      for (i=0;i<(360/5);i++) {
        for (j=0;j<(90-60);j++) {
          if (gcnt[i][j]==0) continue;
        
          lon=i*5.0;
          lat=j+60;
          ptr=grid[i][j];
          if (gcnt[i][j]>1) { 
            qsort(ptr,gcnt[i][j],sizeof(struct vec),vec_sort_pole);
            Vpole=ptr[gcnt[i][j]/2].Vpole;
            qsort(ptr,gcnt[i][j],sizeof(struct vec),vec_sort_east);
            Veast=ptr[gcnt[i][j]/2].Veast;
          } else {
            Vpole=ptr[0].Vpole;
            Veast=ptr[0].Veast;
         }

	  
          mag=sqrt(Veast*Veast+Vpole*Vpole);
          if (mag>4000) continue;
          if (Vpole !=0) azm=atan2d(Veast,Vpole);
          else azm=0;
	  map[0]=lat;
          map[1]=lon;
	  MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
          ax=xoff+pad+pnt[0]*(pwdt-2*pad);
          ay=yoff+pad+pnt[1]*(pwdt-2*pad);

	  RplotEllipse(rplot,NULL,ax,ay,
                       2,2,1,txtcol,0x0f,lne,NULL);

          RPosCalcVector(lat,lon,scale*mag,azm,&lat,&lon);
	  map[0]=lat;
          map[1]=lon;
          MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
          bx=xoff+pad+pnt[0]*(pwdt-2*pad);
          by=yoff+pad+pnt[1]*(pwdt-2*pad);
	  RplotLine(rplot,ax,ay,bx,by,txtcol,0x0f,lne,NULL);
        }
      }

      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
      if (mlon_av_cnt !=0) {
        mlon_av_val=mlon_av_val/mlon_av_cnt;

        mlt=12.0-AACGMConvertMLT(yr,yrsec,mlon_av_val);
        if (mlt<0) mlt=24+mlt;
        if (mlt>24) mlt=mlt-24;

	map[0]=90.0;
	map[1]=0.0;
	MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
	ax=xoff+pad+pnt[0]*(pwdt-2*pad);
	ay=yoff+pad+pnt[1]*(pwdt-2*pad);

	map[0]=60.0;
        map[1]=mlt*360.0/24;
	MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
        bx=xoff+pad+pnt[0]*(pwdt-2*pad);
        by=yoff+pad+pnt[1]*(pwdt-2*pad);
        RplotLine(rplot,ax,ay,bx,by,txtcol,0x0f,lne,NULL);
      }

      map[0]=90.0;
      map[1]=0.0;
      MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
      ax=pnt[0];
      ay=pnt[1];

      RPosCalcVector(90.0,0.0,scale*1000.0,0.0,&lat,&lon);
      map[0]=lat;
      map[1]=lon;

      MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,marg);
      bx=pnt[0];
      by=pnt[1];

      mag=sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));

      RplotEllipse(rplot,NULL,xoff+pwdt-pwdt*0.2,yoff+pwdt*0.05,
		   2,2,1,txtcol,0x0f,lne,NULL);

      RplotLine(rplot,
		xoff+pwdt-pwdt*0.2,yoff+pwdt*0.05,
	        xoff+pwdt-pwdt*0.2+mag*(pwdt-2*pad),yoff+pwdt*0.05,
                txtcol,0x0f,lne,NULL);

      txtbox(dte_fnt_nme,dte_fnt_sze,strlen("1000m/s"),"1000m/s",txbox,fontdb);
      RplotText(rplot,NULL,dte_fnt_nme,dte_fnt_sze,
                xoff+pwdt-pwdt*0.2,yoff+pwdt*0.05+4+txbox[2],
                strlen("1000m/s"),"1000m/s",
                txtcol,0x0f,1);

      plot_time(rplot,xoff+pad,yoff+pad,pwdt-2*pad,pwdt-2*pad,
                txtcol,0x0f,tme_fnt_nme,tme_fnt_sze,fontdb);



      txtbox(dte_fnt_nme,dte_fnt_sze,strlen("Xy"),"Xy",txbox,fontdb);


      RplotText(rplot,NULL,dte_fnt_nme,dte_fnt_sze,
                xoff+pwdt*0.05,yoff+pwdt*0.05,strlen(station),station,
		txtcol,0x0f,1);
      sprintf(str,"%.2d/%.2d/%.4d",mo,dy,yr);
      RplotText(rplot,NULL,dte_fnt_nme,dte_fnt_sze,
		xoff+pwdt*0.05,yoff+pwdt*0.05+4+(txbox[2]-txbox[1]),
                strlen(str),str,txtcol,0x0f,1);
      free_filter();
      c++;
      if (c>npanel) break;
    } while (status !=-1);
    fclose(fp);
    if (c>npanel) break;
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
























