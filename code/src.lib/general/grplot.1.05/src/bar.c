/* bar.c
   ====== 
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
 $Log: bar.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:13:49  barnes
 Initial revision

*/

int GrPlotBarStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
  int i;
  struct GrPlotBlock *blk;
  struct GrPlotBar *ptr;
  struct GrPlotData *plt;


  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotBar *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];
    ptr->color=0;
    ptr->mask=0;
    ptr->width=0;
    ptr->fill=0;
    ptr->or=0;
    ptr->dash=NULL;
    ptr->state=0;
    ptr->w=0;
    ptr->o=0;
    for (i=0;i<atnum;i++) {
      if (strcmp(atname[i],"color")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->color);
      } else if (strcmp(atname[i],"mask")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&mask);
        ptr->mask=mask;
      } else if (strcmp(atname[i],"width")==0) ptr->width=atof(atval[i]);
      else if (strcmp(atname[i],"fill")==0) ptr->fill=1;
      else if (strcmp(atname[i],"dash")==0)
        ptr->dash=RplotMakeDashString(atval[i]);
      else if (strcmp(atname[i],"or")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->or);
      }
    }
    return 0;
  }
 
  if (strcmp(name,"extent")==0) {
    struct GrPlotExtent *ext;
    ext=malloc(sizeof(struct GrPlotExtent));
    ext->cnv=&plt->cnv;
    ext->inc=&plt->inc;
    ext->xmin=&ptr->xmin;
    ext->xmax=&ptr->xmax;
    ext->ymin=&ptr->ymin;
    ext->ymax=&ptr->ymax;
    ptr->state=ptr->state | 0x01;
    blk=GrPlotSave(blk->xml,"extent",ext);
    XMLSetStart(blk->xml,GrPlotExtentStart,blk);
    XMLSetEnd(blk->xml,GrPlotExtentEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"p")==0) {
    struct GrPlotPoint *pnt;
    pnt=malloc(sizeof(struct GrPlotPoint));
    pnt->cnv=&plt->cnv;
    pnt->inc=&plt->inc;
    pnt->x=&ptr->x;
    pnt->y=&ptr->y;
    ptr->state=ptr->state | 0x02;
    blk=GrPlotSave(blk->xml,"p",pnt);
    XMLSetStart(blk->xml,GrPlotPointStart,blk);
    XMLSetEnd(blk->xml,GrPlotPointEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"w")==0) {
    struct GrPlotValue *val;
    val=malloc(sizeof(struct GrPlotValue));
    val->cnv=&plt->cnv;
    val->inc=&plt->inc;
    val->val=&ptr->w;
    ptr->state=ptr->state | 0x04;
    blk=GrPlotSave(blk->xml,"w",val);
    XMLSetStart(blk->xml,GrPlotValueStart,blk);
    XMLSetText(blk->xml,GrPlotValueText,blk);
    XMLSetEnd(blk->xml,GrPlotValueEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"o")==0) {
    struct GrPlotValue *val;
    val=malloc(sizeof(struct GrPlotValue));
    val->cnv=&plt->cnv;
    val->inc=&plt->inc;
    val->val=&ptr->o;
    ptr->state=ptr->state | 0x08;
    blk=GrPlotSave(blk->xml,"o",val);
    XMLSetStart(blk->xml,GrPlotValueStart,blk);
    XMLSetText(blk->xml,GrPlotValueText,blk);
    XMLSetEnd(blk->xml,GrPlotValueEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
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


int GrPlotBarEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotBar *ptr;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotBar *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
 
  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    if (plot==NULL) return -1;
 
    if ((ptr->state & 0x07) !=0x07) return -1;

    PlotBar(plot,plt->num,ptr->x,ptr->y,ptr->w,ptr->o,
             ptr->xmin,ptr->xmax,ptr->ymin,ptr->ymax,
             ptr->or,ptr->fill,ptr->color,ptr->mask,ptr->width,ptr->dash);
     
    if (ptr->dash !=NULL) RplotFreeDash(ptr->dash);
    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}

