/* rplotg.c
   ========
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
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rplotgraw.h"
#include "rplotgstd.h"
#include "rplotg.h"


/* 
 $Log: rplotg.c,v $
 Revision 1.2  2004/11/10 15:06:22  barnes
 Added ability to offset the axis titles.

 Revision 1.1  2003/03/09 21:36:49  barnes
 Initial revision

 */


struct PlotData *PlotMake(float wdt,float hgt,
                          int xnum,int ynum,float lpad,float rpad,
                          float bpad,float tpad,float xoff,float yoff) {
  struct PlotData *plt;

  plt=malloc(sizeof(struct PlotData));
  if (plt==NULL) return NULL;
 
  plt->rplot=NULL;
  plt->xnum=xnum;
  plt->ynum=ynum;
  plt->xoff=xoff;
  plt->yoff=yoff;
  plt->tpad=tpad;
  plt->bpad=bpad;
  plt->lpad=lpad;
  plt->rpad=rpad;

  plt->major_wdt=5;
  plt->minor_wdt=3;
  plt->major_hgt=5;
  plt->minor_hgt=3;

  plt->ttl_wdt=12;
  plt->ttl_hgt=12;
  plt->ttl_xor=0;
  plt->ttl_yor=1; 

  plt->lbl_wdt=4;
  plt->lbl_hgt=4; 

  plt->lbl_xor=0;
  plt->lbl_yor=0; 

  plt->box_hgt=hgt/plt->ynum-plt->tpad-plt->bpad;
  plt->box_wdt=wdt/plt->xnum-plt->lpad-plt->rpad;
  plt->xoff+=plt->lpad;
  plt->yoff+=plt->tpad;

  plt->text.box=NULL;
  plt->text.data=NULL;

  return plt;
}


void PlotFree(struct PlotData *ptr) {
  if (ptr==NULL) return;
  free(ptr);
}


int PlotSetRplot(struct PlotData *plt,struct Rplot *rplot) {
  if (plt==NULL) return -1;
  plt->rplot=rplot;
  return 0;
}

int PlotSetTextBox(struct PlotData *plt,
                   int  (*text)(char *,float,int,char *,float *,void *),
                   void *data) {
  if (plt==NULL) return -1;
  plt->text.box=text;
  plt->text.data=data;
  return 0;
}


int PlotPanel(struct PlotData *plt,int num,unsigned int color,
              unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawPanel(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                      color,mask,width);

}

int PlotClipPanel(struct PlotData *plt,int num) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  return PlotRawClipPanel(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt);

}




int PlotXaxis(struct PlotData *plt,int num,
              double xmin,double xmax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxis(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
		      xmin,xmax,tick_major,tick_minor,
                      plt->major_hgt,plt->minor_hgt,
                      tick_flg,color,mask,width);

  

}
  
int PlotYaxis(struct PlotData *plt,int num,
	      double ymin,double ymax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYaxis(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
			 ymin,ymax,tick_major,tick_minor,
                         plt->major_wdt,plt->minor_wdt,
                         tick_flg,color,mask,width); 

}

int PlotXaxisLog(struct PlotData *plt,int num,
		 double xmin,double xmax,
                 double tick_major,double tick_minor,
	         int tick_flg,
                 unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxisLog(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
			 xmin,xmax,tick_major,tick_minor,
                         plt->major_hgt,plt->minor_hgt,
                         tick_flg,color,mask,width);

}

int PlotYaxisLog(struct PlotData *plt,int num,
		 double ymin,double ymax,
                 double tick_major,double tick_minor,
	         int tick_flg,
		 unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYaxisLog(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
			 ymin,ymax,tick_major,tick_minor,
                         plt->major_wdt,plt->minor_wdt,
                         tick_flg,color,mask,width);

}



int PlotXaxisValue(struct PlotData *plt,int num,
	           double xmin,double xmax,double value,
	           float tick_hgt,int tick_flg,
		   unsigned int color,unsigned char mask,float width) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxisValue(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
			   xmin,xmax,value,
                           tick_hgt,tick_flg,color,mask,width);

}
 
int PlotYaxisValue(struct PlotData *plt,int num,
		   double ymin,double ymax,double value,
	           float tick_hgt,int tick_flg,
		   unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYaxisValue(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
				ymin,ymax,value,
                                tick_hgt,tick_flg,color,mask,width);

}

int PlotXzero(struct PlotData *plt,int num,
	      double xmin,double xmax,
              unsigned int color,unsigned char mask,float width,
	      struct RplotDash *dash) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXzero(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                      xmin,xmax,color,mask,width,
                      dash);


}

int PlotYzero(struct PlotData *plt,int num,
	      double ymin,double ymax,
              unsigned int color,unsigned char mask,float width,
	      struct RplotDash *dash) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYzero(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                      ymin,ymax,color,mask,width,
                      dash);

}

int PlotXaxisLabel(struct PlotData *plt,int num,
	           double xmin,double xmax,double step,
		   int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxisLabel(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                                xmin,xmax,step,
                                plt->lbl_hgt,txt_flg,plt->lbl_xor,
                                plt->text.box,
                                plt->text.data,make_text,textdata,
                                fntname,fntsze,
                                color,mask);

}

int PlotYaxisLabel(struct PlotData *plt,int num,
	           double ymin,double ymax,double step,
	           int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYaxisLabel(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                                ymin,ymax,step,
                                plt->lbl_wdt,txt_flg,plt->lbl_yor,
                                plt->text.box,
                                plt->text.data,make_text,textdata,
                                fntname,fntsze,
                                color,mask);

}

int PlotXaxisLabelValue(struct PlotData *plt,int num,
	                double xmin,double xmax,double value,
			int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxisLabelValue(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
				xmin,xmax,value,
                                plt->lbl_hgt,txt_flg,plt->lbl_xor,
                                plt->text.box,
                                plt->text.data,sze,text,fntname,fntsze,
                                color,mask);

}

int PlotYaxisLabelValue(struct PlotData *plt,int num,
	                double ymin,double ymax,double value,
		        int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYaxisLabelValue(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                                ymin,ymax,value,
                                plt->lbl_wdt,txt_flg,plt->lbl_yor,
                                plt->text.box,
                                plt->text.data,sze,text,fntname,fntsze,
                                color,mask);

}

int PlotXaxisTitle(struct PlotData *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxisTitle(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_hgt,txt_flg,plt->ttl_xor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int PlotXaxisTitleOffset(struct PlotData *plt,int num,
		   int txt_flg,
		   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad)+offset;
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawXaxisTitle(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_hgt,txt_flg,plt->ttl_xor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int PlotYaxisTitle(struct PlotData *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawYaxisTitle(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_wdt,txt_flg,plt->ttl_yor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int PlotYaxisTitleOffset(struct PlotData *plt,int num,
		   int txt_flg,
                   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad)+offset;

  return PlotRawYaxisTitle(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_wdt,txt_flg,plt->ttl_yor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int PlotFitImage(struct PlotData *plt,int num,
                 struct FrameBuffer *img,
		 unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  return PlotRawFitImage(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                      img,mask); 

}

int PlotFitImageName(struct PlotData *plt,int num,
		  char *name,int iwdt,int ihgt,
                  unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  return PlotRawFitImageName(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                      name,iwdt,ihgt,mask); 

}

int PlotLine(struct PlotData *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask,float width,
             struct RplotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawLine(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                     ax,ay,bx,by,xmin,xmax,ymin,ymax,color,mask,width,
                     dash);
}

int PlotRectangle(struct PlotData *plt,int num,
		  struct RplotMatrix *matrix,
                  double x,double y,double w,double h,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  int fill,unsigned int color,unsigned char mask,float width,
                  struct RplotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawRectangle(plt->rplot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     x,y,w,h,xmin,xmax,ymin,ymax,fill,color,mask,width,
                     dash);
}




int PlotEllipse(struct PlotData *plt,int num,
		  struct RplotMatrix *matrix,
                  double x,double y,double w,double h,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  int fill,unsigned int color,unsigned char mask,float width,
                  struct RplotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawEllipse(plt->rplot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     x,y,w,h,xmin,xmax,ymin,ymax,fill,color,mask,width,
                     dash);
}


int PlotPolygon(struct PlotData *plt,int num,
		struct RplotMatrix *matrix,
		double x,double y,int n,double *px,double *py,int *t,
	        double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct RplotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawPolygon(plt->rplot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                       x,y,n,px,py,t,
                       xmin,xmax,ymin,ymax,fill,color,mask,width,
                       dash);
}


int PlotText(struct PlotData *plt,int num,
	     struct RplotMatrix *matrix,char *fontname,
             float fontsize,double x,double y,
             int n,char *txt,float dx,float dy,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawText(plt->rplot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     fontname,fontsize,x,y,n,txt,dx,dy,
                     xmin,xmax,ymin,ymax,color,mask);
}


int PlotImage(struct PlotData *plt,int num,
	     struct RplotMatrix *matrix,
	      struct FrameBuffer *img,float dx,float dy,
             double x,double y,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawImage(plt->rplot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     img,dx,dy,x,y,
                     xmin,xmax,ymin,ymax,mask);
}



int PlotImageName(struct PlotData *plt,int num,
	          struct RplotMatrix *matrix,
                  char *name,float dx,float dy,
                  double x,double y,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawImageName(plt->rplot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                          name,dx,dy,x,y,
                          xmin,xmax,ymin,ymax,mask);
}






int PlotBezier(struct PlotData *plt,int num,
             double ax,double ay,double bx,double by,
	     double cx,double cy,double dx,double dy,
             double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask,float width,
             struct RplotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawBezier(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                     ax,ay,bx,by,cx,cy,dx,dy,xmin,xmax,ymin,ymax,
                     color,mask,width,
                     dash);
}





int PlotFill(struct PlotData *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,int or,
	     unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawFill(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                     ax,ay,bx,by,xmin,xmax,ymin,ymax,or,color,mask);


}


int PlotBar(struct PlotData *plt,int num,
	    double ax,double ay,double w,double o,
	    double xmin,double xmax,
	    double ymin,double ymax,int or,int fill,
	    unsigned int color,unsigned char mask,float width,
            struct RplotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawBar(plt->rplot,ox,oy,plt->box_wdt,plt->box_hgt,
                    ax,ay,w,o,xmin,xmax,ymin,ymax,or,fill,color,mask,
                    width,dash);


}


int PlotGetPoint(struct PlotData *plt,int num,
             double x,double y,
	     double xmin,double xmax,
             double ymin,double ymax,float *px,float *py) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return PlotRawGetPoint(ox,oy,plt->box_wdt,plt->box_hgt,
                         x,y,xmin,xmax,ymin,ymax,px,py);
}
