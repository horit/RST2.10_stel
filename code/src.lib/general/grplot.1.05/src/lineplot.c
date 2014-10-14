/* lineplot.c
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
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "fontdb.h"
#include "rplotg.h"
#include "grplot.h"
#include "block.h"

/*
 $Log: lineplot.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:16:45  barnes
 Initial revision

*/

int GrPlotLinePlotStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
  int i;
  struct GrPlotBlock *blk;
  struct GrPlotLinePlot *ptr;
  struct GrPlotData *plt;


  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotLinePlot *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];
    ptr->color=0;
    ptr->mask=0;
    ptr->width=0;
    ptr->dash=NULL;
    ptr->state=0;
    ptr->xdmin=-HUGE_VAL;
    ptr->xdmax=HUGE_VAL;
    ptr->ydmin=-HUGE_VAL;
    ptr->ydmax=HUGE_VAL;
    ptr->name=NULL;
    ptr->encode=NULL;
    ptr->x=NULL;
    ptr->y=NULL;
    
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
      else if (strcmp(atname[i],"dash")==0)
        ptr->dash=RplotMakeDashString(atval[i]);

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

  if (strcmp(name,"range")==0) {
    struct GrPlotExtent *ext;
    ext=malloc(sizeof(struct GrPlotExtent));
    ext->cnv=&plt->cnv;
    ext->inc=&plt->inc;
    ext->xmin=&ptr->xdmin;
    ext->xmax=&ptr->xdmax;
    ext->ymin=&ptr->ydmin;
    ext->ymax=&ptr->ydmax;
    ptr->state=ptr->state | 0x02;
    blk=GrPlotSave(blk->xml,"range",ext);
    XMLSetStart(blk->xml,GrPlotExtentStart,blk);
    XMLSetEnd(blk->xml,GrPlotExtentEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }



  if (strcmp(name,"name")==0) {
    struct GrPlotString *str;
    str=malloc(sizeof(struct GrPlotString));
    str->txt=&ptr->name;
    str->inc=&plt->inc;
    blk=GrPlotSave(blk->xml,"name",str);
    XMLSetStart(blk->xml,GrPlotStringStart,blk);
    XMLSetText(blk->xml,GrPlotStringText,blk);
    XMLSetEnd(blk->xml,GrPlotStringEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"index")==0) {
    struct GrPlotIndex *inx;
    inx=malloc(sizeof(struct GrPlotIndex));
    inx->inc=&plt->inc;
    inx->x=&ptr->x;
    inx->y=&ptr->y;
    blk=GrPlotSave(blk->xml,"index",inx);
    XMLSetStart(blk->xml,GrPlotIndexStart,blk);
    XMLSetEnd(blk->xml,GrPlotIndexEnd,blk);
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


int GrPlotLinePlotEnd(char *name,char *buf,int sze,void *data) {
  void *fp;
  double xval,yval;
  double oldx=0,oldy=0;
  int flg;
  struct GrPlotBlock *blk;
  struct GrPlotLinePlot *ptr;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotLinePlot *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    if (plot==NULL) return -1;
   
    if ((ptr->state & 0x01)!=0x01) return -1;
    if (ptr->name==NULL) return -1;
    if (ptr->x==NULL) return -1;
    if (ptr->y==NULL) return -1;
  
    /* do the plot here */


    if ((plt->file.open !=NULL) &&
        (plt->file.close!=NULL) &&
        (plt->file.read !=NULL)) {
      fp=(plt->file.open)(ptr->name,ptr->encode,ptr->x,ptr->y,
                          plt->file.data);
      if (fp !=NULL) {
        flg=0;
        while ( (plt->file.read)(fp,ptr->encode,ptr->x,ptr->y,
                                &xval,&yval,plt->file.data) !=-1) {

          if (xval<ptr->xdmin) {
	    flg=0;
            continue;
	  }
          if (xval>ptr->xdmax) {
            flg=0;
            continue;
          }

	  if (yval<ptr->ydmin) {
            flg=0;
            continue;
          }
          if (yval>ptr->ydmax) {
            flg=0;
            continue;
          }

          if (flg==1) 
	    PlotLine(plot,plt->num,oldx,oldy,xval,yval,
		     ptr->xmin,ptr->xmax,ptr->ymin,ptr->ymax,
		     ptr->color,ptr->mask,ptr->width,ptr->dash);
          oldx=xval;
          oldy=yval;
          flg=1;
        }
        (plt->file.close)(fp,plt->file.data);
      } 
    }
    free(ptr->name);
    if (ptr->encode !=NULL) free(ptr->encode);
    free(ptr->x);
    free(ptr->y);    
    if (ptr->dash !=NULL) RplotFreeDash(ptr->dash);
    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}

