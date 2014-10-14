/* rplotg.h
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

/*
 $Log: rplotg.h,v $
 Revision 1.2  2004/11/10 15:06:04  barnes
 Added ability to offset the axis titles.

 Revision 1.1  2003/03/05 20:26:31  barnes
 Initial revision

*/

#ifndef _PLOTGRAPH_H
#define _PLOTGRAPH_H

struct PlotData {
  struct Rplot *rplot;
  int xnum;
  int ynum;
  float xoff;
  float yoff;
  float tpad;
  float bpad;  
  float lpad;
  float rpad;

  float major_wdt;
  float minor_wdt;
  float major_hgt;
  float minor_hgt;

  float ttl_wdt;
  float ttl_hgt;
  int ttl_xor;
  int ttl_yor;
  
  float lbl_wdt;
  float lbl_hgt;
  int lbl_xor;
  int lbl_yor;
 
  float box_wdt;
  float box_hgt;

  struct {
    int  (*box)(char *fntname,float fntsize,int num,char *text,
                 float *box,void *data);
    void *data;
  } text;
};


struct PlotData *PlotMake(float wdt,float hgt,
                          int xnum,int ynum,float lpad,float rpad,
                          float bpad,float tpad,float xoff,float yoff);
void PlotFree(struct PlotData *ptr);
int PlotSetRplot(struct PlotData *plt,struct Rplot *rplot);
int PlotSetTextBox(struct PlotData *plt,
                   int  (*text)(char *,float,int,char *,float *,void *),
                   void *data);

int PlotPanel(struct PlotData *plt,int num,unsigned int color,
              unsigned char mask,float width);

int PlotClipPanel(struct PlotData *plt,int num);

int PlotXaxis(struct PlotData *plt,int num,
              double xmin,double xmax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width);


int PlotYaxis(struct PlotData *plt,int num,
	      double ymin,double ymax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width);

int PlotXaxisLog(struct PlotData *plt,int num,
		 double xmin,double xmax,
                 double tick_major,double tick_minor,
	         int tick_flg,
                 unsigned int color,unsigned char mask,float width);

int PlotYaxisLog(struct PlotData *plt,int num,
		 double ymin,double ymax,
                 double tick_major,double tick_minor,
	         int tick_flg,
		 unsigned int color,unsigned char mask,float width);

int PlotXaxisValue(struct PlotData *plt,int num,
	           double xmin,double xmax,double value,
	           float tick_hgt,int tick_flg,
		   unsigned int color,unsigned char mask,float width);

int PlotYaxisValue(struct PlotData *plt,int num,
		   double ymin,double ymax,double value,
	           float tick_wdt,int tick_flg,
		   unsigned int color,unsigned char mask,float width);



int PlotXzero(struct PlotData *plt,int num,
	      double xmin,double xmax,
              unsigned int color,unsigned char mask,float width,
	      struct RplotDash *dash);

int PlotYzero(struct PlotData *plt,int num,
	      double ymin,double ymax,
              unsigned int color,unsigned char mask,float width,
	      struct RplotDash *dash);

int PlotXaxisLabel(struct PlotData *plt,int num,
	           double xmin,double xmax,double step,
		   int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask);


int PlotYaxisLabel(struct PlotData *plt,int num,
	           double ymin,double ymax,double step,
	           int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask);


int PlotXaxisLabelValue(struct PlotData *plt,int num,
	                double xmin,double xmax,double value,
			int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask);

int PlotYaxisLabelValue(struct PlotData *plt,int num,
	                double ymin,double ymax,double value,
		        int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask);


int PlotXaxisTitle(struct PlotData *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);


int PlotXaxisTitleOffset(struct PlotData *plt,int num,
		   int txt_flg,
		   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);


int PlotYaxisTitle(struct PlotData *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);

int PlotYaxisTitleOffset(struct PlotData *plt,int num,
		   int txt_flg,
                   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);


int PlotFitImage(struct PlotData *plt,int num,
              struct FrameBuffer *img,
	      unsigned char mask);

int PlotFitImageName(struct PlotData *plt,int num,
		  char *name,int iwdt,int ihgt,
                  unsigned char mask);

int PlotLine(struct PlotData *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask,float width,
             struct RplotDash *dash);

int PlotRectangle(struct PlotData *plt,int num,
		  struct RplotMatrix *matrix,
                  double x,double y,double w,double h,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  int fill,unsigned int color,unsigned char mask,float width,
                  struct RplotDash *dash);

int PlotEllipse(struct PlotData *plt,int num,
	        struct RplotMatrix *matrix,
                double x,double y,double w,double h,
	        double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
		struct RplotDash *dash);

int PlotBezier(struct PlotData *plt,int num,
               double ax,double ay,double bx,double by,
               double cx,double cy,double dx,double dy,
               double xmin,double xmax,
               double ymin,double ymax,
               unsigned int color,unsigned char mask,float width,
               struct RplotDash *dash);

int PlotPolygon(struct PlotData *plt,int num,
		struct RplotMatrix *matrix,
		double x,double y,int n,double *px,double *py,int *t,
	        double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct RplotDash *dash);

int PlotFill(struct PlotData *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,int or,
	     unsigned int color,unsigned char mask);

int PlotText(struct PlotData *plt,int num,
	     struct RplotMatrix *matrix,char *fontname,
             float fontsize,double x,double y,
             int n,char *txt,float dx,float dy,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask);

int PlotImageName(struct PlotData *plt,int num,
	          struct RplotMatrix *matrix,
                  char *name,float dx,float dy,
                  double x,double y,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  unsigned char mask);

int PlotImage(struct PlotData *plt,int num,
	     struct RplotMatrix *matrix,
	      struct FrameBuffer *img,float dx,float dy,
             double x,double y,
	     double xmin,double xmax,
             double ymin,double ymax,
	     unsigned char mask);

int PlotBar(struct PlotData *plt,int num,
	    double ax,double ay,double w,double o,
	    double xmin,double xmax,
	    double ymin,double ymax,int or,int fill,
	    unsigned int color,unsigned char mask,
            float width,struct RplotDash *dash);

int PlotGetPoint(struct PlotData *plt,int num,
             double x,double y,
	     double xmin,double xmax,
	     double ymin,double ymax,float *px,float *py);


int PlotKey(struct PlotData *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double step,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
            int ksze,unsigned char *r,unsigned char *g,unsigned char *b);

int PlotKeyLog(struct PlotData *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double tick_major,double tick_minor,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
	    int ksze,unsigned char *r,unsigned char *g,unsigned char *b);


#endif

