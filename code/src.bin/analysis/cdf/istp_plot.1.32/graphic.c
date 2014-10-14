/* graphic.c
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

/* 
 $Log: graphic.c,v $
 Revision 1.5  2004/08/29 21:41:42  barnes
 Fixes for Mac OS X.

 Revision 1.4  2004/04/27 18:24:08  barnes
 Audit - Enforced warning set to "all".

 Revision 1.3  2003/03/06 20:28:20  barnes
 Modification to the headers of the plot library.

 Revision 1.2  2003/02/27 23:55:09  barnes
 Fixed bug in displaying the page title.

 Revision 1.1  2003/02/27 23:47:46  barnes
 Initial revision

 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rfbuffer.h"
#include "fontdb.h"

#include "rtime.h"
#include "rplot.h"
#include "rplotg.h"

#include "istp.h"

#define DAYSC (24*3600)

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



char *mtxt[]={"JAN","FEB","MAR","APR","MAY","JUN","JUL",
                "AUG","SEP","OCT","NOV","DEC",0};

char *mltxt[]={"Jan","Feb","Mar","Apr","May","Jun","Jul",
                "Aug","Sep","Oct","Nov","Dec",0};


char *label_tme(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%.2d",((int) (val/3600)) % 24);
  return txt;
}


char *label_dte(double val,double min,double max,void *data) {

  int yr,mo,dy,hr,mt;
  double sc;
  char *txt=NULL;
  txt=malloc(32);

  /* 
  if ((((int) (val/3600)) % 24) !=0) {
    sprintf(txt,""); 
    return;
  }
  */

  TimeEpochToYMDHMS(val,&yr,&mo,&dy,&hr,&mt,&sc);
  sprintf(txt,"%.2d %s %.4d",dy,mltxt[mo-1],yr);
  return txt;
}



char *label_val(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32); 
  sprintf(txt,"%.2d",(int) val);
  return txt;
}

char *label_km(double val,double min,double max,void *data) {
  char *txt=NULL;
  txt=malloc(32);
  sprintf(txt,"%g",val);
  return txt;
}




void plot_frame(struct PlotData *plt,int pmax,double stime,
                double etime,int dflg,
                unsigned int color,float width,char *fontname,
                float fontsize) {
  int i=0;
  double xtime;

  double xmajor=3600;
  double xminor=600;
 
  xtime=etime-stime;
  while (xtime>DAYSC) {
     xtime=xtime/2;
     xmajor=xmajor*2;
     xminor=xminor*2;
     if (xminor==2400) xminor=1800;
     if (xminor==14400) xminor=DAYSC;
     if (xmajor==14400) xmajor=43200;
     
  }
  if ((dflg) && (xmajor<(43200))) xmajor=43200;

  for (i=0;i<pmax;i++) {
     PlotPanel(plt,i,color,0x0f,width);
     PlotXaxis(plt,i,stime,etime,xmajor,xminor,0x03,color,0x0f,width);
   }
  plt->lbl_hgt=8;
  PlotXaxisLabel(plt,pmax-1,stime,etime,xmajor,0x01,label_tme,NULL,
                 fontname,fontsize,color,0x0f);
  if (dflg) {
    plt->lbl_hgt=18;
    PlotXaxisLabel(plt,pmax-1,stime,etime,xmajor,0x01,label_dte,NULL,
                   fontname,fontsize,color,0x0f);
    plt->lbl_hgt=8;
  }
  PlotXaxisTitle(plt,pmax-1,0x01,strlen("UT (Hours)"),"UT (Hours)",fontname,
                 fontsize,color,0x0f);
}



void plot_pos(struct PlotData *plt,int p,
              struct posdata *pos,double stime,double etime,int mode,
              unsigned int color,float width,char *fontname,
              float fontsize) {
 float *P;
 int i;
 int flg;
 float ylim=0;  
 double ymajor=100;
 double yminor=100/5;
 double y;   
 int km=0;
 float mult=1;
 float ox=0,oy=0,px=0,py=0;
 float tx,ty;
 unsigned int gry;
 unsigned int red;
 unsigned int blue;

 struct RplotDash *dashA;
 struct RplotDash *dashB;

 dashA=RplotMakeDashString("4 4");
 dashB=RplotMakeDashString("4 2 1 2");

 gry=RplotColor(128,128,128,255);
 red=RplotColor(255,0,0,255);
 blue=RplotColor(0,0,255,255);

 PlotYzero(plt,p,-1,1,gry,0x0f,width,NULL); 
 
 for (i=0;i<pos->cnt;i++) {
     if (mode==0) P=&pos->PGSM[3*i]; 
     else P=&pos->PGSE[3*i]; 
  
     if (fabs(P[0])>500) continue;
     if (fabs(P[1])>500) continue;
     if (fabs(P[2])>500) continue;

     if (fabs(P[0])>ylim) ylim=fabs(P[0]);
     if (fabs(P[1])>ylim) ylim=fabs(P[1]);
     if (fabs(P[2])>ylim) ylim=fabs(P[2]);
   }
   if (ylim<2) { /* plot in km */
     km=1;
     mult=6370.0;
     ylim=ylim*6370;
     y=0;
     while (y<ylim) y+=1000;
     ylim=y;
     ymajor=1000;
     yminor=1000/5;
     
   } else if (ylim<60) {
     y=0;
     while (y<ylim) y=y+20;
     ylim=y;
     ymajor=20;
     yminor=20/4;
   } else {
     y=0;
     while (y<ylim) y=y+50;
     ylim=y;
     ymajor=100;
     yminor=100/5;
   }

   PlotYaxis(plt,p,-ylim,ylim,ymajor,yminor,0x03,color,0x0f,width);
   if (km==0) {
      PlotYaxisLabel(plt,p,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
      PlotYaxisTitle(plt,p,0x03,strlen("Pos (RE)"),"Pos (RE)",
                     fontname,fontsize,color,0x0f);
   } else {
      PlotYaxisLabel(plt,p,-ylim,ylim,ymajor,0x03,label_km,NULL,fontname,
                     fontsize,color,0x0f);
      PlotYaxisTitle(plt,p,0x03,strlen("Pos (km)"),"Pos (km)",
                     fontname,fontsize,color,0x0f);
   }
   PlotGetPoint(plt,p,0,1,0,1,0,1,&tx,&ty);
   RplotLine(plt->rplot,tx+5,ty+5,tx+20,ty+5,color,0x0f,width,NULL);
   RplotLine(plt->rplot,tx+35,ty+5,tx+50,ty+5,red,0x0f,width,dashA);
   RplotLine(plt->rplot,tx+65,ty+5,tx+80,ty+5,blue,0x0f,width,dashB);

   RplotText(plt->rplot,NULL,fontname,fontsize,
             tx+22,ty+10,strlen("X"),"X",blue,0x0f,1);
   RplotText(plt->rplot,NULL,fontname,fontsize,
             tx+52,ty+10,strlen("Y"),"Y",blue,0x0f,1);
   RplotText(plt->rplot,NULL,fontname,fontsize,
             tx+82,ty+10,strlen("Z"),"Z",blue,0x0f,1);


  flg=0;
  for (i=0;i<pos->cnt;i++) { 
    if (mode==0) P=&pos->PGSM[3*i]; 
    else P=&pos->PGSE[3*i];  
    if (fabs(P[0])<500) {
      px=pos->time[i]-stime;
      py=P[0]*mult;
      if (flg==1) PlotLine(plt,p,ox,oy,px,py,
                           0,etime-stime,-ylim,ylim,
                           color,0x0f,width,NULL);
      ox=px;
      oy=py;
      flg=1;
    } else flg=0;
  }

  flg=0;
  for (i=0;i<pos->cnt;i++) { 
    if (mode==0) P=&pos->PGSM[3*i]; 
    else P=&pos->PGSE[3*i];  
    if (fabs(P[1])<500) {
      px=pos->time[i]-stime;
      py=P[1]*mult;
      if (flg==1) PlotLine(plt,p,ox,oy,px,py,
                           0,etime-stime,-ylim,ylim,
                           red,0x0f,width,dashA);
      ox=px;
      oy=py;
      flg=1;
    } else flg=0;
  }

  flg=0;
  for (i=0;i<pos->cnt;i++) { 
    if (mode==0) P=&pos->PGSM[3*i]; 
    else P=&pos->PGSE[3*i];  
    if (fabs(P[2])<500) {
      px=pos->time[i]-stime;
      py=P[2]*mult;
      if (flg==1) PlotLine(plt,p,ox,oy,px,py,
                           0,etime-stime,-ylim,ylim,
                           blue,0x0f,width,dashB);
      ox=px;
      oy=py;
      flg=1;
    } else flg=0;
  }
  RplotFreeDash(dashA);
  RplotFreeDash(dashB);

}



void plot_plasma(struct PlotData *plt,int p,struct plasmadata *plasma,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
              float fontsize) {

 int i;
 int flg=0;
 float *V;
 float ylim=50;
 float yxlim=1000;
 float ymajor=20;
 float yxmajor=200;
 float yplim=10.0;
 float ypmajor=2;
 int o=0;
 float ox=0,oy=0,px,py;

 unsigned int gry;
 gry=RplotColor(128,128,128,255);

 if (strcmp(plasma->sat,"ge")==0) {
   ylim=150;
   ymajor=50;
 }

 if (pflg[0]) o++;
 if (pflg[1]) o++;
 if (pflg[2]) {
   PlotYzero(plt,p+o,-1,1,gry,0x0f,width,NULL); 
   o++;
 }
 if (pflg[3]) {
   PlotYzero(plt,p+o,-1,1,gry,0x0f,width,NULL); 
   o++;
 }

 o=0;

 if (pflg[0]) {
   char *title="Pressure (nPa)";
   PlotYaxis(plt,p,0,yplim,ypmajor,0,0x03,color,0x0f,width);
   PlotYaxisLabel(plt,p,0,yplim,ypmajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   PlotYaxisTitle(plt,p,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
   o++;
 }
 if (pflg[1]) {
   char *title="Vx (km/s)";
   PlotYaxis(plt,p+o,0,yxlim,yxmajor,0,0x03,color,0x0f,width);
   PlotYaxisLabel(plt,p+o,0,yxlim,yxmajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   PlotYaxisTitle(plt,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
   o++;
 }
 if (pflg[2]) {
   char *title="Vy (km/s)";
   PlotYaxis(plt,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
   PlotYaxisLabel(plt,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   PlotYaxisTitle(plt,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);

   o++;
 }
 if (pflg[3]) {
   char *title="Vz (km/s)";
   PlotYaxis(plt,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
   PlotYaxisLabel(plt,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
   PlotYaxisTitle(plt,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
   o++;
 }


 o=0;

 if (pflg[0]) {  
   flg=0;
   for (i=0;i<plasma->cnt;i++) {
       
      if (fabs(plasma->pre[i])<1.e6) {
        px=plasma->time[i]-stime;
        py=plasma->pre[i];
        if (py>yplim) py=yplim;
        if (flg==1) PlotLine(plt,p,ox,oy,px,py,
                               0,etime-stime,0,yplim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
   o++;
 }

 if (pflg[1]) {
   flg=0;
   for (i=0;i<plasma->cnt;i++) {
      if (mode==0) V=&plasma->VGSM[3*i]; 
      else V=&plasma->VGSE[3*i]; 

      if (fabs(V[0])<1.e6) {
        px=plasma->time[i]-stime;
        py=fabs(V[0]);
        if (py>yxlim) py=yxlim;
        if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,0,yxlim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
   o++;
  }
  if (pflg[2]) {
    flg=0;
    for (i=0;i<plasma->cnt;i++) {
      if (mode==0) V=&plasma->VGSM[3*i]; 
      else V=&plasma->VGSE[3*i];   

      if (fabs(V[1])<1e6) {
        px=plasma->time[i]-stime;
        py=V[1];
        if (py>ylim) py=ylim;
        if (py<-ylim) py=-ylim;
        if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;
        flg=1;  
      } else flg=0;
    }
    o++;
  }
  if (pflg[2]) {
    flg=0;
    for (i=0;i<plasma->cnt;i++) {
      if (mode==0) V=&plasma->VGSM[3*i]; 
      else V=&plasma->VGSE[3*i];   

      if (fabs(V[2])<1e6) {
        px=plasma->time[i]-stime;
        py=V[2];
        if (py>ylim) py=ylim;
        if (py<-ylim) py=-ylim;
        if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
    o++;
  }

}

void plot_imf(struct PlotData *plt,int p,struct imfdata *imf,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
              float fontsize) {
   int i;
   int flg=0;  
   float *B;
   float mod;
   float ylim=0;
   double ymajor=10;  
   double y;   
   int o=0;

   float ox=0,oy=0,px=0,py=0;

   unsigned int gry;
   gry=RplotColor(128,128,128,255);

   if (pflg[0]) o++;
   if (pflg[1]) {
     PlotYzero(plt,p+o,-1,1,gry,0x0f,width,NULL); 
     o++;
   }
   if (pflg[2]) {
     PlotYzero(plt,p+o,-1,1,gry,0x0f,width,NULL); 
     o++;
   }
   if (pflg[3]) {
     PlotYzero(plt,p+o,-1,1,gry,0x0f,width,NULL); 
     o++;
   }

   for (i=0;i<imf->cnt;i++) {
     if (mode==0) B=&imf->BGSMc[3*i]; 
     else B=&imf->BGSEc[3*i]; 
     if (fabs(B[0])>500) continue;
     if (fabs(B[1])>500) continue;
     if (fabs(B[2])>500) continue;

     if (fabs(B[0])>ylim) ylim=fabs(B[0]);
     if (fabs(B[1])>ylim) ylim=fabs(B[1]);
     if (fabs(B[2])>ylim) ylim=fabs(B[2]);
   }

   y=0;
   while (y<ylim) y+=5;
   ylim=y;
   if (ylim>50) ylim=50; 

   o=0;

   if (pflg[0]) {
     char *title="|B| (nT)";
     PlotYaxis(plt,p,0,1.5*ylim,ymajor,0,0x03,color,0x0f,width);
     PlotYaxisLabel(plt,p,0,1.5*ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     PlotYaxisTitle(plt,p,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }
   if (pflg[1]) {
     char *title="Bx (nT)";
     PlotYaxis(plt,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
     PlotYaxisLabel(plt,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     PlotYaxisTitle(plt,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }
   if (pflg[2]) {
     char *title="By (nT)";
     PlotYaxis(plt,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
     PlotYaxisLabel(plt,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     PlotYaxisTitle(plt,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }
   if (pflg[3]) {
     char *title="Bz (nT)";
     PlotYaxis(plt,p+o,-ylim,ylim,ymajor,0,0x03,color,0x0f,width);
     PlotYaxisLabel(plt,p+o,-ylim,ylim,ymajor,0x03,label_val,NULL,fontname,
                     fontsize,color,0x0f);
     PlotYaxisTitle(plt,p+o,0x03,strlen(title),title,
                     fontname,fontsize,color,0x0f);
     o++;
   }



 
  o=0; 
  if (pflg[0]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
   
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i];  

      mod=sqrt(B[0]*B[0]+B[1]*B[1]+B[2]*B[2]);
      if (mod<100) {
        px=imf->time[i]-stime;
        py=mod;
        if (py>1.5*ylim) py=1.5*ylim;
        if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,0,1.5*ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;   
    }
    o++;
  }
  if (pflg[1]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i];   
      if (fabs(B[0])<500) {
         px=imf->time[i]-stime;
         py=B[0];
         if (py>ylim) py=ylim;
         if (py<-ylim) py=-ylim;
         if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
    o++;
  }
  if (pflg[2]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i]; 

      if (fabs(B[1])<100) {
         px=imf->time[i]-stime;
         py=B[1];
         if (py>ylim) py=ylim;
         if (py<-ylim) py=-ylim;
         if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;

    }
    o++;
  }
  if (pflg[3]) {
    flg=0;
    for (i=0;i<imf->cnt;i++) {
      if (mode==0) B=&imf->BGSMc[3*i]; 
      else B=&imf->BGSEc[3*i];

      if (fabs(B[2])<100) {
        px=imf->time[i]-stime;
        py=B[2];
        if (py>ylim) py=ylim;
        if (py<-ylim) py=-ylim;
        if (flg==1) PlotLine(plt,p+o,ox,oy,px,py,
                               0,etime-stime,-ylim,ylim,
                               color,0x0f,width,NULL);
        ox=px;
        oy=py;  
        flg=1;
      } else flg=0;
    }
    o++;
  }

}

void plot_title(struct PlotData *plt,double stime,double etime,
                char *sat,char *plot,int mode,
                unsigned int color,float width,char *fontname,
                float fontsize) {
 char title[256];


 char *motxt[]={"GSM","GSE",0};
 int yr,mo,dy,hr,mt;
 int dayno;
 double sc;
 double yrsec;
 TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
 
 yrsec=stime-TimeYMDHMSToEpoch(yr,1,1,0,0,0);
 dayno=1+yrsec/(24*3600);
 
 sprintf(title,"%s %s (%s) %s %.2d %.4d (Dayno=%.2d)",
         sat,plot,motxt[mode],mtxt[mo-1],dy,yr,dayno);
 
 PlotXaxisTitle(plt,0,0x02,strlen(title),title,
                fontname,fontsize,color,0x0f);


}



   


















