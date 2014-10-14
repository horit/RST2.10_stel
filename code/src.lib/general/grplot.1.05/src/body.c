/* body.c
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "fontdb.h"
#include "rplotg.h"
#include "grplot.h"
#include "block.h"

/*
 $Log: body.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:13:49  barnes
 Initial revision

*/


int GrPlotBodyStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  plt=(struct GrPlotData *) blk->data;
  if (plt==NULL) return -1;
  if (plt->plot==NULL) return -1;
  if (plt->rplot==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    PlotClipPanel(plot,plt->num);
    return 0;
  }

  if (strcmp(name,"line")==0) {
    struct GrPlotLine *lne;
    lne=malloc(sizeof(struct GrPlotLine));
    lne->plt=plt;
    lne->off=NULL;
    lne->state=0;
    blk=GrPlotSave(plt->xml,name,lne);
    XMLSetStart(plt->xml,GrPlotLineStart,blk);
    XMLSetEnd(plt->xml,GrPlotLineEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"bezier")==0) {
    struct GrPlotBezier *bez;
    bez=malloc(sizeof(struct GrPlotBezier));
    bez->plt=plt;
    bez->off=NULL;
    bez->state=0;
    blk=GrPlotSave(plt->xml,name,bez);
    XMLSetStart(plt->xml,GrPlotBezierStart,blk);
    XMLSetEnd(plt->xml,GrPlotBezierEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ellipse")==0) {
    struct GrPlotShape *shp;
    shp=malloc(sizeof(struct GrPlotShape));
    shp->plt=plt;
    shp->off=NULL;
    shp->state=0;
    blk=GrPlotSave(plt->xml,name,shp);
    XMLSetStart(plt->xml,GrPlotShapeStart,blk);
    XMLSetEnd(plt->xml,GrPlotShapeEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"rectangle")==0) {
    struct GrPlotShape *shp;
    shp=malloc(sizeof(struct GrPlotShape));
    shp->plt=plt;
    shp->off=NULL;
    shp->state=0;
    blk=GrPlotSave(plt->xml,name,shp);
    XMLSetStart(plt->xml,GrPlotShapeStart,blk);
    XMLSetEnd(plt->xml,GrPlotShapeEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"polygon")==0) {
    struct GrPlotPolygon *pol;
    pol=malloc(sizeof(struct GrPlotPolygon));
    pol->plt=plt;
    pol->off=NULL;
    pol->state=0;
    blk=GrPlotSave(plt->xml,name,pol);
    XMLSetStart(plt->xml,GrPlotPolygonStart,blk);
    XMLSetEnd(plt->xml,GrPlotPolygonEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

 if (strcmp(name,"text")==0) {
    struct GrPlotText *txt;
    txt=malloc(sizeof(struct GrPlotText));
    txt->plt=plt;
    txt->off=NULL;
    txt->state=0;
    blk=GrPlotSave(plt->xml,name,txt);
    XMLSetStart(plt->xml,GrPlotTextStart,blk);
    XMLSetEnd(plt->xml,GrPlotTextEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

 if (strcmp(name,"image")==0) {
    struct GrPlotImage *img;
    img=malloc(sizeof(struct GrPlotImage));
    img->plt=plt;
    img->off=NULL;
    img->state=0;
    blk=GrPlotSave(plt->xml,name,img);
    XMLSetStart(plt->xml,GrPlotImageStart,blk);
    XMLSetEnd(plt->xml,GrPlotImageEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"imagen")==0) {
    struct GrPlotImageName *img;
    img=malloc(sizeof(struct GrPlotImageName));
    img->plt=plt;
    img->off=NULL;
    img->state=0;
    blk=GrPlotSave(plt->xml,name,img);
    XMLSetStart(plt->xml,GrPlotImageNameStart,blk);
    XMLSetEnd(plt->xml,GrPlotImageNameEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"bar")==0) {
    struct GrPlotBar *bar;
    bar=malloc(sizeof(struct GrPlotBar));
    bar->plt=plt;
    bar->state=0;
    blk=GrPlotSave(plt->xml,name,bar);
    XMLSetStart(plt->xml,GrPlotBarStart,blk);
    XMLSetEnd(plt->xml,GrPlotBarEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

 if (strcmp(name,"fill")==0) {
    struct GrPlotFill *bar;
    bar=malloc(sizeof(struct GrPlotFill));
    bar->plt=plt;
    bar->state=0;
    blk=GrPlotSave(plt->xml,name,bar);
    XMLSetStart(plt->xml,GrPlotFillStart,blk);
    XMLSetEnd(plt->xml,GrPlotFillEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"fimage")==0) {
    struct GrPlotFitImage *img;
    img=malloc(sizeof(struct GrPlotFitImage));
    img->plt=plt;
    blk=GrPlotSave(plt->xml,name,img);
    XMLSetStart(plt->xml,GrPlotFitImageStart,blk);
    XMLSetEnd(plt->xml,GrPlotFitImageEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"fimagen")==0) {
    struct GrPlotFitImageName *img;
    img=malloc(sizeof(struct GrPlotFitImageName));
    img->plt=plt;
    blk=GrPlotSave(plt->xml,name,img);
    XMLSetStart(plt->xml,GrPlotFitImageNameStart,blk);
    XMLSetEnd(plt->xml,GrPlotFitImageNameEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"lineplot")==0) {
    struct GrPlotLinePlot *pl;
    pl=malloc(sizeof(struct GrPlotLinePlot));
    pl->plt=plt;
    pl->state=0;
    blk=GrPlotSave(plt->xml,name,pl);
    XMLSetStart(plt->xml,GrPlotLinePlotStart,blk);
    XMLSetEnd(plt->xml,GrPlotLinePlotEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"fillplot")==0) {
    struct GrPlotFillPlot *pl;
    pl=malloc(sizeof(struct GrPlotFillPlot));
    pl->plt=plt;
    pl->state=0;
    blk=GrPlotSave(plt->xml,name,pl);
    XMLSetStart(plt->xml,GrPlotFillPlotStart,blk);
    XMLSetEnd(plt->xml,GrPlotFillPlotEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"barplot")==0) {
    struct GrPlotBarPlot *pl; 
    pl=malloc(sizeof(struct GrPlotBarPlot));
    pl->plt=plt;
    pl->state=0;
    blk=GrPlotSave(plt->xml,name,pl);
    XMLSetStart(plt->xml,GrPlotBarPlotStart,blk);
    XMLSetEnd(plt->xml,GrPlotBarPlotEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"symbolplot")==0) {
    struct GrPlotSymbolPlot *pl; 
    pl=malloc(sizeof(struct GrPlotSymbolPlot));
    pl->plt=plt;
    pl->state=0;
    blk=GrPlotSave(plt->xml,name,pl);
    XMLSetStart(plt->xml,GrPlotSymbolPlotStart,blk);
    XMLSetEnd(plt->xml,GrPlotSymbolPlotEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"symbol")==0) {
    struct GrPlotSymbol *pl; 
    pl=malloc(sizeof(struct GrPlotSymbol));
    pl->plt=plt;
    pl->state=0;
    blk=GrPlotSave(plt->xml,name,pl);
    XMLSetStart(plt->xml,GrPlotSymbolStart,blk);
    XMLSetEnd(plt->xml,GrPlotSymbolEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"include")==0) {
    struct GrPlotInclude *inc;
    inc=malloc(sizeof(struct GrPlotInclude));
    inc->inc=&plt->inc;
    blk=GrPlotSave(blk->xml,name,inc);
    XMLSetStart(blk->xml,GrPlotIncludeStart,blk);
    XMLSetEnd(blk->xml,GrPlotIncludeEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ignore")==0) {
    blk=GrPlotSave(blk->xml,name,NULL);
    XMLSetStart(blk->xml,GrPlotIgnoreStart,blk);
    XMLSetText(blk->xml,GrPlotIgnoreText,blk);
    XMLSetEnd(blk->xml,GrPlotIgnoreEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  return 0;
}


int GrPlotBodyEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotData *plt;


  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  plt=(struct GrPlotData *) blk->data;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    RplotClip(plot->rplot,0,NULL,NULL,NULL);    
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}


