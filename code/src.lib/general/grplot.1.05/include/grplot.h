/* grplot.h
   ======= 
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
 $Log: grplot.h,v $
 Revision 1.2  2007/01/16 20:51:10  code
 Added the ability to change the orientation of titles and labels.

 Revision 1.1  2003/03/22 03:21:24  barnes
 Initial revision

*/

#ifndef _GRPLOT_H
#define _GRPLOT_H

struct GrPlotConvert {
  int (*user)(char *type,char *buf,double *val,void *data);
  void *data;
};

struct GrPlotIncludeBlock {
  int (*user)(char *name,
		 int (*user)(char *buf,int sze,void *data),
		 void *udata,
		 void *data);
  void *data;
};

struct GrPlotData {
  struct XMLdata *xml;
  struct Rplot *rplot;
  struct PlotData *plot;
  int num;
  struct {
    int  (*box)(char *fntname,float fntsize,int num,
                char *text,
                float *box,void *data);
    void *data;
  } text;

  struct {
    int  (*size)(char *name,int *x,int *y,void *data);
    void *data;
  } image;



  struct {
    char *(*text)(char *style,double value,
                  double min,double max,void *data);
    void *data;
  } make;

  struct {
    void *(*open)(char *name,char *encode,char *xinx,char *yinx,void *data);
    void (*close)(void *fp,void *data);
    int  (*read)(void *,char *encode,
                 char *xinx,char *yinx,double *xval,double *yval,void *data);
    void *data;
  } file;
  struct GrPlotIncludeBlock inc;
  struct GrPlotConvert cnv;

};

/* Scaling and offset for symbols */

struct GrPlotOffset {
  double x,y;
  double w,h;
};

/* Data value */

struct GrPlotValue {
  struct GrPlotConvert *cnv;
  struct GrPlotIncludeBlock *inc;
  char *type;
  char *tag;
  char *buf;
  double *val;
};

/* Basic types */

struct GrPlotFloat {
  struct GrPlotIncludeBlock *inc;
  char *tag;
  char *buf;
  float *val;
};

struct GrPlotInt {
  struct GrPlotIncludeBlock *inc;
  char *tag;
  char *buf;
  int *val;
};

struct GrPlotString {
  struct GrPlotIncludeBlock *inc;
  char *tag;
  char *buf;
  char **txt;
};

/* Limits max/min */

struct GrPlotLimit {
  int state;
  struct GrPlotConvert *cnv;
  struct GrPlotIncludeBlock *inc;
  double *min, *max;
};

/* Axis index for data file */

struct GrPlotIndex {
  int state;
  struct GrPlotIncludeBlock *inc;
  char **x, **y;
};

/* Extent of a plot */

struct GrPlotExtent {
  int state;
  struct GrPlotIncludeBlock *inc;
  struct GrPlotConvert *cnv;
  double *xmin,*xmax;
  double *ymin,*ymax;
};


/* Point on a plot */

struct GrPlotPoint {
  int state;
  struct GrPlotIncludeBlock *inc;
  struct GrPlotConvert *cnv;
  double *x,*y;
};

/* Point in a polygon */

struct GrPlotPolyPoint {
  int state;
  struct GrPlotIncludeBlock *inc;
  struct GrPlotConvert *cnv;
  double *x,*y;
  int *t;
};

/* A Dimension w/h */

struct GrPlotDimension {
  int state;
  struct GrPlotIncludeBlock *inc;
  struct GrPlotConvert *cnv;
  double *w,*h;
};

/* Tick mark spacing */

struct GrPlotTick {
  struct GrPlotIncludeBlock *inc;
  struct GrPlotConvert *cnv;
  double *major,*minor;
};


/* Axis plotting and labelling */

struct GrPlotBox {
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float width;
};


struct GrPlotAxis {
  int state;
  int flg;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  float major_size;
  float minor_size;
  double min;
  double max;
  double major;
  double minor;  
};

struct GrPlotZero {
  int state;
  int flg;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  struct RplotDash *dash;
  float width;
  double min;
  double max;  
};


struct GrPlotAxisValue {
  int state;
  int flg;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  float size;
  double min;
  double max;
  double val;  
};


struct GrPlotTitle {
  int flg;
  int or;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float off;
  char *fname;
  float fsize;
  char *txt;
};


struct GrPlotLabelValue {
  int state;
  int flg;
  int or;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float off;
  char *fname;
  float fsize;
  char *txt;
  double min;
  double max;
  double val;
};

struct GrPlotLabel {
  int state;
  int flg;
  int or;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float off;
  char *fname;
  float fsize;
  char *style;
  double min;
  double max;
  double step;
};


/* Basic Graphic Primitives */

struct GrPlotLine {
  int state;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  double xmin,xmax,ymin,ymax;
  double x1,x2,y1,y2;

};

struct GrPlotBezier {
  int state;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  double xmin,xmax,ymin,ymax;
  double x1,x2,x3,x4;
  double y1,y2,y3,y4;
};


struct GrPlotShape {
  int state;
  int fill;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  struct RplotMatrix *matrix;
  double xmin,xmax,ymin,ymax;
  double x,y;
  double w,h;
};

struct GrPlotPolygon {
  int state;
  int fill;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  struct RplotMatrix *matrix;
  double xmin,xmax,ymin,ymax;
  double x,y;
  int num;
  double *px,*py;
  int *t;
};

struct GrPlotText {
  int state;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned int color;
  unsigned char mask;
  char *fname;
  float fsize;
  float dx,dy;
  struct RplotMatrix *matrix;
  char *txt;
  double xmin,xmax,ymin,ymax;
  double x,y;
};


struct GrPlotImage {
  int state;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned char mask;
  struct RplotMatrix *matrix;
  float dx,dy;
  double xmin,xmax,ymin,ymax;
  struct FrameBuffer *img;
  double x,y;
};

struct GrPlotImageName {
  int state;
  struct GrPlotData *plt;
  struct GrPlotOffset *off;
  unsigned char mask;
  struct RplotMatrix *matrix;
  float dx,dy;
  double xmin,xmax,ymin,ymax;
  char *name;
  double x,y;
};

struct GrPlotInclude {
  struct GrPlotIncludeBlock *inc;
  struct GrPlotOffset *off;
  char *name;
};



/* Plot a bar for a barchart */

struct GrPlotBar {
  int state;
  int or;
  int fill;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  double xmin,xmax,ymin,ymax;
  double x,y,w,o;

};

/* Plot a filled plot to one of the axis */

struct GrPlotFill {
  int state;
  int or;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  double xmin,xmax,ymin,ymax;
  double x1,x2,y1,y2;

};

/* Fit an image to the plot */

struct GrPlotFitImage {
  struct GrPlotData *plt;
  unsigned char mask;
  struct FrameBuffer *img;
};

struct GrPlotFitImageName {
  struct GrPlotData *plt;
  unsigned char mask;
  char *name;
};


/* Plot a symbol */


struct GrPlotSymbol {
  int state;
  struct GrPlotData *plt;
  char *symbol;
  double w,h;
  double xmin,xmax,ymin,ymax; /* plot limits */
  double x,y;
};


/* Line plot */

struct GrPlotLinePlot {
  int state;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  double xmin,xmax,ymin,ymax; /* plot limits */
  double xdmin,xdmax,ydmin,ydmax; /* data limits */
  char *name;
  char *encode;
  char *x;
  char *y;
};

/* Filled plot */

struct GrPlotFillPlot {
  int state;
  int or;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  double xmin,xmax,ymin,ymax; /* plot limits */
  double xdmin,xdmax,ydmin,ydmax; /* data limits */
  char *name;
  char *encode;
  char *x;
  char *y;
};


/* Bar chart */

struct GrPlotBarPlot {
  int state;
  int or;
  int fill;
  struct GrPlotData *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  struct RplotDash *dash;
  double xmin,xmax,ymin,ymax; /* plot limits */
  double xdmin,xdmax,ydmin,ydmax; /* data limits */
  double w,o;
  char *name;
  char *encode;
  char *x;
  char *y;
};

/* Symbol plot */

struct GrPlotSymbolPlot {
  int state;
  struct GrPlotData *plt;
  char *symbol;
  double w,h;
  double xmin,xmax,ymin,ymax; /* plot limits */
  double xdmin,xdmax,ydmin,ydmax; /* data limits */
  char *name;
  char *encode;
  char *x;
  char *y;
};

/* Symbol plotting data structure */

struct GrPlotSymbolDraw {
  struct GrPlotData *plt;
  struct GrPlotOffset off;
  double xmin,xmax,ymin,ymax;
};


int GrPlotPointString(char *str,float *x,float *y);
int GrPlotPadString(char *str,float *l,float *r,
                    float *b,float *t);


int GrPlotSymbolStoreText(char *buf,int sze,void *data);
int GrPlotSymbolStoreStart(char *name,int atnum,char **atname,
                           char **atval,char *buf,int sze,void *data);
int GrPlotSymbolStoreEnd(char *name,char *buf,int sze,void *data);



int GrPlotStringText(char *buf,int sze,void *data);
int GrPlotStringStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int GrPlotStringEnd(char *name,char *buf,int sze,void *data);


int GrPlotFloatText(char *buf,int sze,void *data);
int GrPlotFloatStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int GrPlotFloatEnd(char *name,char *buf,int sze,void *data);


int GrPlotIntText(char *buf,int sze,void *data);
int GrPlotIntStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int GrPlotIntEnd(char *name,char *buf,int sze,void *data);

int GrPlotValueText(char *buf,int sze,void *data);
int GrPlotValueStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int GrPlotValueEnd(char *name,char *buf,int sze,void *data);

int GrPlotLimitStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotLimitEnd(char *name,char *buf,int sze,void *data);

int GrPlotIndexStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotIndexEnd(char *name,char *buf,int sze,void *data);

int GrPlotExtentStart(char *name,int atnum,char **atname,char **atval,
                     char *buf,int sze,void *data);
int GrPlotExtentEnd(char *name,char *buf,int sze,void *data);

int GrPlotPointStart(char *name,int atnum,char **atname,char **atval,
                     char *buf,int sze,void *data);
int GrPlotPointEnd(char *name,char *buf,int sze,void *data);

int GrPlotDimensionStart(char *name,int atnum,char **atname,char **atval,
                     char *buf,int sze,void *data);
int GrPlotDimensionEnd(char *name,char *buf,int sze,void *data);

int GrPlotTickStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotTickEnd(char *name,char *buf,int sze,void *data);


int GrPlotPanelStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotPanelEnd(char *name,char *buf,int sze,void *data);

int GrPlotBodyStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotBodyEnd(char *name,char *buf,int sze,void *data);

int GrPlotBoxStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotBoxEnd(char *name,char *buf,int sze,void *data);

int GrPlotZeroStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotZeroEnd(char *name,char *buf,int sze,void *data);

int GrPlotAxisStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotAxisEnd(char *name,char *buf,int sze,void *data);

int GrPlotAxisValueStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotAxisValueEnd(char *name,char *buf,int sze,void *data);

int GrPlotTitleStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotTitleEnd(char *name,char *buf,int sze,void *data);

int GrPlotLabelValueStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotLabelValueEnd(char *name,char *buf,int sze,void *data);

int GrPlotLabelStart(char *name,int atnum,char **atname,char **atval,
			  char *buf,int sze,void *data);
int GrPlotLabelEnd(char *name,char *buf,int sze,void *data);


int GrPlotLineStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int GrPlotLineEnd(char *name,char *buf,int sze,void *data);


int GrPlotBezierStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotBezierEnd(char *name,char *buf,int sze,void *data);

int GrPlotShapeStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotShapeEnd(char *name,char *buf,int sze,void *data);

int GrPlotPolygonStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotPolygonEnd(char *name,char *buf,int sze,void *data);

int GrPlotTextStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotTextEnd(char *name,char *buf,int sze,void *data);

int GrPlotBarStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotBarEnd(char *name,char *buf,int sze,void *data);

int GrPlotFillStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotFillEnd(char *name,char *buf,int sze,void *data);


int GrPlotImageStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotImageEnd(char *name,char *buf,int sze,void *data);

int GrPlotImageNameStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotImageNameEnd(char *name,char *buf,int sze,void *data);

int GrPlotFitImageStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotFitImageEnd(char *name,char *buf,int sze,void *data);

int GrPlotFitImageNameStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data);
int GrPlotFitImageNameEnd(char *name,char *buf,int sze,void *data);

int GrPlotIncludeStart(char *name,int atnum,char **atname,char **atval,
			    char *buf,int sze,void *data);
int GrPlotIncludeEnd(char *name,char *buf,int sze,void *data);

int GrPlotIgnoreText(char *buf,int sze,void *data);

int GrPlotIgnoreStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data);
int GrPlotIgnoreEnd(char *name,char *buf,int sze,void *data);

int GrPlotSymbolStart(char *name,int atnum,char **atname,char **atval,
			    char *buf,int sze,void *data);
int GrPlotSymbolEnd(char *name,char *buf,int sze,void *data);

int GrPlotLinePlotStart(char *name,int atnum,char **atname,char **atval,
			    char *buf,int sze,void *data);
int GrPlotLinePlotEnd(char *name,char *buf,int sze,void *data);

int GrPlotFillPlotStart(char *name,int atnum,char **atname,char **atval,
			char *buf,int sze,void *data);
int GrPlotFillPlotEnd(char *name,char *buf,int sze,void *data);

int GrPlotBarPlotStart(char *name,int atnum,char **atname,char **atval,
                        char *buf,int sze,void *data);
int GrPlotBarPlotEnd(char *name,char *buf,int sze,void *data);

int GrPlotSymbolPlotStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data);
int GrPlotSymbolPlotEnd(char *name,char *buf,int sze,void *data);

int GrPlotSymbolDrawStart(char *name,int atnum,char **atname,char **atval,
			    char *buf,int sze,void *data);
int GrPlotSymbolDrawEnd(char *name,char *buf,int sze,void *data);




void GrPlotFree(struct GrPlotData *ptr);
struct GrPlotData *GrPlotMake(struct XMLdata *xml);
int GrPlotSetTextBox(struct GrPlotData *plt,
                   int  (*text)(char *,float,int,char *,float *,void *),
		     void *data);

int GrPlotSetMakeText(struct GrPlotData *plt,
		      char *(*text)(char *,double,double,double,void *data),
                     void *data);

int GrPlotSetConvert(struct GrPlotData *plt,
		     int (*convert)(char *,char *,double *,void *),
                     void *data);



int GrPlotSetRplot(struct GrPlotData *plt,struct Rplot *rplot);
int GrPlotStart(char *name,int atnum,char **atname,char **atval,
		char *buf,int sze,void *data);
int GrPlotEnd(char *name,char *buf,int sze,void *data);
int GrPlotText(char *buf,int sze,void *data);

int GrPlotSetInclude(struct GrPlotData *plt,
                   int (*text)(char *,int (*)(char *,int,void *),void *,
			       void *),
                   void *data);

int GrPlotSetImageSize(struct GrPlotData *plt,
		       int (*size)(char *,int *,int *,void *),
		       void *data);


int GrPlotSetOpenFile(struct GrPlotData *plt,
		      void *(*open)(char *,char *,char *,char *,void *));

int GrPlotSetCloseFile(struct GrPlotData *plt,
		       void (*close)(void *,void *));
 
int GrPlotSetReadFile(struct GrPlotData *plt,
		      int (*read)(void *,char *,char *,char *,
                                  double *,double *,void *));
 
int GrPlotSetFileData(struct GrPlotData *plt,void *data);



#endif
