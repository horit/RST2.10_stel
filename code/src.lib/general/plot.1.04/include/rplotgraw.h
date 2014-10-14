/* rplotgraw.h
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
 $Log: rplotgraw.h,v $
 Revision 1.1  2003/03/09 21:31:17  barnes
 Initial revision

*/

#ifndef _PLOTRAW_H
#define _PLOTRAW_H


int PlotRawGetPoint(float ox,float oy,float wdt,float hgt,
	            double x,double y,double xmin,double xmax,double ymin,
	            double ymax,float *px,float *py);

int PlotRawPanel(struct Rplot *ptr,float x,float y,float wdt,float hgt,
                 unsigned int color,unsigned  char mask,
                 float width);

int PlotRawClipPanel(struct Rplot *ptr,float x,float y,float wdt,float hgt);



int PlotRawXaxis(struct Rplot *ptr,
                 float ox,float oy,float wdt,float hgt,
		 double xmin,double xmax,
                 double tick_major,double tick_minor,
	         float major_hgt,float minor_hgt,int tick_flg,
		 unsigned int color,unsigned char mask,float width);
  
int PlotRawYaxis(struct Rplot *ptr,
                 float ox,float oy,float wdt,float hgt,
		 double ymin,double ymax,
                 double tick_major,double tick_minor,
	         float major_wdt,float minor_wdt,int tick_flg,
		 unsigned int color,unsigned char mask,float width);

int PlotRawXaxisLog(struct Rplot *ptr,
                    float ox,float oy,float wdt,float hgt,
		    double xmin,double xmax,
                    double tick_major,double tick_minor,
	            float major_hgt,float minor_hgt,int tick_flg,
                    unsigned int color,unsigned char mask,float width);

int PlotRawYaxisLog(struct Rplot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     double ymin,double ymax,
                     double tick_major,double tick_minor,
	             float major_wdt,float minor_wdt,int tick_flg,
		    unsigned int color,unsigned char mask,float width);



int PlotRawXaxisValue(struct Rplot *ptr,
                    float ox,float oy,float wdt,float hgt,
	            double xmin,double xmax,double value,
	            float tick_hgt,int tick_flg,
		    unsigned int color,unsigned char mask,float width);
 
int PlotRawYaxisValue(struct Rplot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     double ymin,double ymax,double value,
	             float tick_wdt,int tick_flg,
		     unsigned int color,unsigned char mask,float width);

int PlotRawXzero(struct Rplot *ptr,
                  float ox,float oy,float wdt,float hgt,
		  double xmin,double xmax,
                  unsigned int color,unsigned char mask,float width,
		  struct RplotDash *dash);

int PlotRawYzero(struct Rplot *ptr,
                  float ox,float oy,float wdt,float hgt,
		  double ymin,double ymax,
                  unsigned int color,unsigned char mask,float width,
		  struct RplotDash *dash);

int PlotRawXaxisLabel(struct Rplot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double xmin,double xmax,double step,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      char * (*make_text)(double,double,double,void *),
                      void *textdata,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);

int PlotRawYaxisLabel(struct Rplot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double ymin,double ymax,double step,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      char * (*make_text)(double,double,double,void *),
                      void *textdata,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);

int PlotRawXaxisLabelValue(struct Rplot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double xmin,double xmax,double value,
			   float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
			   unsigned int color,unsigned char mask);

int PlotRawYaxisLabelValue(struct Rplot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double ymin,double ymax,double value,
			   float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
			   unsigned int color,unsigned char mask);

int PlotRawXaxisTitle(struct Rplot *ptr,
                      float ox,float oy,float wdt,float hgt,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);



int PlotRawYaxisTitle(struct Rplot *ptr,
                      float ox,float oy,float wdt,float hgt,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);

int PlotRawFitImage(struct Rplot *ptr,
                       float ox,float oy,float wdt,float hgt,
                       struct FrameBuffer *img,
		 unsigned char mask);

int PlotRawFitImageName(struct Rplot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     char *name,int iwdt,int ihgt,
                     unsigned char mask);

int PlotRawLine(struct Rplot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask,float width,
                struct RplotDash *dash);

int PlotRawRectangle(struct Rplot *ptr,
                struct RplotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                double x,double y,double w,double h,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
	        struct RplotDash *dash);

int PlotRawEllipse(struct Rplot *ptr,
                struct RplotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                double x,double y,double w,double h,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
		   struct RplotDash *dash);

int PlotRawPolygon(struct Rplot *ptr,
                struct RplotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
		 double x,double y,int num,double *px,double *py,
                int *t,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
		   struct RplotDash *dash);

int PlotRawBezier(struct Rplot *ptr,
                  float ox,float oy,float wdt,float hgt,
                  double ax,double ay,double bx,double by,
                  double cx,double cy,double dx,double dy,
                  double xmin,double xmax,
                  double ymin,double ymax,
                  unsigned int color,unsigned char mask,float width,
                  struct RplotDash *dash);

int PlotRawText(struct Rplot *ptr,
                struct RplotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                char *fontname,float fontsize,
                double x,double y,int num,char *txt,
                float dx,float dy,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask);

int PlotRawImage(struct Rplot *ptr,
                struct RplotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
		struct FrameBuffer *img,
		 float dx,float dy,
	        double x,double y,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned char mask);


int PlotRawImageName(struct Rplot *ptr,
                struct RplotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
	        char *name,float dx,float dy,double x,double y,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned char mask);



int PlotRawFill(struct Rplot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
		double xmin,double xmax,
                double ymin,double ymax,int or,
                unsigned int color,unsigned char mask);


int PlotRawBar(struct Rplot *ptr,
                float ox,float oy,float wdt,float hgt,
	       double ax,double ay,double w,double o,
		double xmin,double xmax,
	        double ymin,double ymax,int or,int fill,
	        unsigned int color,unsigned char mask,
                float width,struct RplotDash *dash);


int PlotRawKey(struct Rplot *ptr,
               float ox,float oy,float wdt,float hgt,
               double min,double max,
               double tick_major,double tick_minor,
               float major_size,float minor_size,
               float txt_off,float ttl_off,
               int tick_flg,int txt_flg,int ttl_flg,
               int txt_or,int ttl_or,
               int or,
               int ttl_num,char *ttl_text,
               int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *r,unsigned char *g,unsigned char *b);

int PlotRawKeyLog(struct Rplot *ptr,
		  float ox,float oy,float wdt,float hgt,
		  double min,double max,
		  double tick_major,double tick_minor,
		  float major_size,float minor_size,
		  float txt_off,float ttl_off,
		  int tick_flg,int txt_flg,int ttl_flg,
		  int txt_or,int ttl_or,
		  int or,
		  int ttl_num,char *ttl_text,
		  int  (*text_box)(char *,float,int,char *,float *,void *),
		  void *boxdata,
		  char * (*make_text)(double,double,double,void *),
		  void *textdata,
		  char *fntname,float fntsze,
		  unsigned int color,unsigned char mask,float width,
		  int ksze,unsigned char *r,unsigned char *g,unsigned char *b);

int PlotRawContour(struct Rplot *rplot,struct PolygonData *ptr,
                  float xoff,float yoff,float wdt,float hgt,
                  float step,float smooth,float lwdt,
                  int lstart,int lstep,
                  int sze,char *text,
                  unsigned int color,char mask,float width,
                  struct RplotDash *dash,
                  char *fntname,float fntsze,
                  int  (*text_box)(char *,float,int,char *,float *,void *),
		   void *boxdata);




#endif
