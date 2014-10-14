/* axis.c
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
 $Log: axis.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:13:49  barnes
 Initial revision

*/

int GrPlotAxisStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
  int i;
  struct GrPlotBlock *blk;
  struct GrPlotAxis *ptr;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotAxis *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];
    ptr->color=0;
    ptr->mask=0;
    ptr->width=0;
    ptr->major_size=0;
    ptr->minor_size=0;
    ptr->state=0;
    ptr->flg=0;
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
      else if (strcmp(atname[i],"major")==0) ptr->major_size=atof(atval[i]);
      else if (strcmp(atname[i],"minor")==0) ptr->minor_size=atof(atval[i]);
      else if (strcmp(atname[i],"flag")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->flg);
      }
    }
    return 0;
  }

 
  if (strcmp(name,"limit")==0) {
    struct GrPlotLimit *lmt;
    lmt=malloc(sizeof(struct GrPlotLimit));
    lmt->cnv=&plt->cnv;
    lmt->inc=&plt->inc;
    lmt->min=&ptr->min;
    lmt->max=&ptr->max;
    ptr->state=ptr->state | 0x01;
    blk=GrPlotSave(blk->xml,"limit",lmt);
    XMLSetStart(blk->xml,GrPlotLimitStart,blk);
    XMLSetEnd(blk->xml,GrPlotLimitEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"tick")==0) {
    struct GrPlotTick *tck;
    tck=malloc(sizeof(struct GrPlotTick));
    tck->cnv=&plt->cnv;
    tck->inc=&plt->inc;
    tck->major=&ptr->major;
    tck->minor=&ptr->minor;
    blk=GrPlotSave(blk->xml,"tick",tck);
    XMLSetStart(blk->xml,GrPlotTickStart,blk);
    XMLSetEnd(blk->xml,GrPlotTickEnd,blk);
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


int GrPlotAxisEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotAxis *ptr;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotAxis *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    if (plot==NULL) return -1;
    if (ptr->state !=1) return -1;

    plot->major_wdt=ptr->major_size;
    plot->minor_wdt=ptr->minor_size;
    plot->major_hgt=ptr->major_size;
    plot->minor_hgt=ptr->minor_size;

    if (strcmp(name,"xaxis")==0) 
    PlotXaxis(plot,plt->num,ptr->min,ptr->max,ptr->major,ptr->minor,
              ptr->flg,ptr->color,ptr->mask,ptr->width);


    if (strcmp(name,"yaxis")==0) 
    PlotYaxis(plot,plt->num,ptr->min,ptr->max,ptr->major,ptr->minor,
              ptr->flg,ptr->color,ptr->mask,ptr->width);

    if (strcmp(name,"xaxislog")==0) 
    PlotXaxisLog(plot,plt->num,ptr->min,ptr->max,ptr->major,ptr->minor,
              ptr->flg,ptr->color,ptr->mask,ptr->width);

    if (strcmp(name,"yaxislog")==0) 
    PlotYaxisLog(plot,plt->num,ptr->min,ptr->max,ptr->major,ptr->minor,
              ptr->flg,ptr->color,ptr->mask,ptr->width);

     

    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}

