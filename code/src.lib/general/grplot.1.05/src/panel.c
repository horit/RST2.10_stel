/* panel.c
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
 $Log: panel.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/05/01 19:38:21  barnes
 Initial revision

*/


int GrPlotPanelStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
  int i;
  int num=0;
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
    for (i=0;i<atnum;i++) {
      if ((strcmp(atname[i],"num")==0) && (atval[i] !=NULL))
      num=atof(atval[i]);
    }
    plt->num=num;
    if (num>=plot->xnum*plot->ynum) return -1;   
    return 0;
  }

  if (strcmp(name,"box")==0) {
    struct GrPlotBox *box;
    box=malloc(sizeof(struct GrPlotBox));
    box->plt=plt;
    blk=GrPlotSave(plt->xml,"box",box);
    XMLSetStart(plt->xml,GrPlotBoxStart,blk);
    XMLSetEnd(plt->xml,GrPlotBoxEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"xaxis")==0) {
    struct GrPlotAxis *axe;
    axe=malloc(sizeof(struct GrPlotAxis));
    axe->plt=plt;
    blk=GrPlotSave(plt->xml,name,axe);
    XMLSetStart(plt->xml,GrPlotAxisStart,blk);
    XMLSetEnd(plt->xml,GrPlotAxisEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"yaxis")==0) {
    struct GrPlotAxis *axe;
    axe=malloc(sizeof(struct GrPlotAxis));
    axe->plt=plt;
    blk=GrPlotSave(plt->xml,name,axe);
    XMLSetStart(plt->xml,GrPlotAxisStart,blk);
    XMLSetEnd(plt->xml,GrPlotAxisEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

 if (strcmp(name,"xaxislog")==0) {
    struct GrPlotAxis *axe;
    axe=malloc(sizeof(struct GrPlotAxis));
    axe->plt=plt;
    blk=GrPlotSave(plt->xml,name,axe);
    XMLSetStart(plt->xml,GrPlotAxisStart,blk);
    XMLSetEnd(plt->xml,GrPlotAxisEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"yaxislog")==0) {
    struct GrPlotAxis *axe;
    axe=malloc(sizeof(struct GrPlotAxis));
    axe->plt=plt;
    blk=GrPlotSave(plt->xml,name,axe);
    XMLSetStart(plt->xml,GrPlotAxisStart,blk);
    XMLSetEnd(plt->xml,GrPlotAxisEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

 if (strcmp(name,"xaxisvalue")==0) {
    struct GrPlotAxisValue *axe;
    axe=malloc(sizeof(struct GrPlotAxisValue));
    axe->plt=plt;
    blk=GrPlotSave(plt->xml,name,axe);
    XMLSetStart(plt->xml,GrPlotAxisValueStart,blk);
    XMLSetEnd(plt->xml,GrPlotAxisValueEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"yaxisvalue")==0) {
    struct GrPlotAxisValue *axe;
    axe=malloc(sizeof(struct GrPlotAxisValue));
    axe->plt=plt;
    blk=GrPlotSave(plt->xml,name,axe);
    XMLSetStart(plt->xml,GrPlotAxisValueStart,blk);
    XMLSetEnd(plt->xml,GrPlotAxisValueEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"xtitle")==0) {
    struct GrPlotTitle *ttl;
    ttl=malloc(sizeof(struct GrPlotTitle));
    ttl->plt=plt;
    blk=GrPlotSave(plt->xml,name,ttl);
    XMLSetStart(plt->xml,GrPlotTitleStart,blk);
    XMLSetEnd(plt->xml,GrPlotTitleEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"ytitle")==0) {
    struct GrPlotTitle *ttl;
    ttl=malloc(sizeof(struct GrPlotTitle));
    ttl->plt=plt;
    blk=GrPlotSave(plt->xml,name,ttl);
    XMLSetStart(plt->xml,GrPlotTitleStart,blk);
    XMLSetEnd(plt->xml,GrPlotTitleEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"xlabel")==0) {
    struct GrPlotLabelValue *lbl;
    lbl=malloc(sizeof(struct GrPlotLabel));
    lbl->plt=plt;
    blk=GrPlotSave(plt->xml,name,lbl);
    XMLSetStart(plt->xml,GrPlotLabelStart,blk);
    XMLSetEnd(plt->xml,GrPlotLabelEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ylabel")==0) {
    struct GrPlotLabel *lbl;
    lbl=malloc(sizeof(struct GrPlotLabel));
    lbl->plt=plt;
    blk=GrPlotSave(plt->xml,name,lbl);
    XMLSetStart(plt->xml,GrPlotLabelStart,blk);
    XMLSetEnd(plt->xml,GrPlotLabelEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }



 if (strcmp(name,"xlabelvalue")==0) {
    struct GrPlotLabelValue *lbl;
    lbl=malloc(sizeof(struct GrPlotLabelValue));
    lbl->plt=plt;
    blk=GrPlotSave(plt->xml,name,lbl);
    XMLSetStart(plt->xml,GrPlotLabelValueStart,blk);
    XMLSetEnd(plt->xml,GrPlotLabelValueEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"ylabelvalue")==0) {
    struct GrPlotLabelValue *lbl;
    lbl=malloc(sizeof(struct GrPlotLabelValue));
    lbl->plt=plt;
    blk=GrPlotSave(plt->xml,name,lbl);
    XMLSetStart(plt->xml,GrPlotLabelValueStart,blk);
    XMLSetEnd(plt->xml,GrPlotLabelValueEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  


  if (strcmp(name,"xzero")==0) {
    struct GrPlotZero *zro;
    zro=malloc(sizeof(struct GrPlotZero));
    zro->plt=plt;
    blk=GrPlotSave(plt->xml,name,zro);
    XMLSetStart(plt->xml,GrPlotZeroStart,blk);
    XMLSetEnd(plt->xml,GrPlotZeroEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"yzero")==0) {
    struct GrPlotZero *zro;
    zro=malloc(sizeof(struct GrPlotZero));
    zro->plt=plt;
    blk=GrPlotSave(plt->xml,name,zro);
    XMLSetStart(plt->xml,GrPlotZeroStart,blk);
    XMLSetEnd(plt->xml,GrPlotZeroEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  } 

  if (strcmp(name,"body")==0) {
    blk=GrPlotSave(plt->xml,name,plt);
    XMLSetStart(plt->xml,GrPlotBodyStart,blk);
    XMLSetEnd(plt->xml,GrPlotBodyEnd,blk);
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


int GrPlotPanelEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  if (strcmp(name,blk->name)==0) {
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}

