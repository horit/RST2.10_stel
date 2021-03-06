/* label.c
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
 $Log: label.c,v $
 Revision 1.3  2006/12/05 22:27:03  code
 Added the ability to change the orientation of labels and titles.

 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:16:45  barnes
 Initial revision

*/

char *GrPlotMakeText(double value,double min,double max,void *data) {
  struct GrPlotLabel *ptr;
  struct GrPlotData *plt;

  ptr=(struct GrPlotLabel *)data;
  if (ptr==NULL) return NULL;
  plt=ptr->plt;
  if (plt==NULL) return NULL;

  if (plt->make.text !=NULL)
    return (plt->make.text)(ptr->style,value,min,max,plt->make.data);

  return NULL;
}

int GrPlotLabelStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
  int i;
  struct GrPlotBlock *blk;
  struct GrPlotLabel *ptr;
  struct GrPlotData *plt;


  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotLabel *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];

    ptr->state=0;
    ptr->color=0;
    ptr->mask=0;
    ptr->off=0;
    ptr->flg=0;
    ptr->or=0;
    ptr->fname=NULL;
    ptr->fsize=0;
    ptr->style=NULL;

    ptr->min=0;
    ptr->max=0;
    ptr->step=0;


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
      } else if (strcmp(atname[i],"font")==0) {
        if (ptr->fname !=NULL) free(ptr->fname);
        ptr->fname=malloc(strlen(atval[i]+1));
	strcpy(ptr->fname,atval[i]);
      } else if (strcmp(atname[i],"size")==0) ptr->fsize=atof(atval[i]);
      else if (strcmp(atname[i],"offset")==0) ptr->off=atof(atval[i]);
      else if (strcmp(atname[i],"flag")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->flg);
      } else if (strcmp(atname[i],"or")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->or);
      } else if (strcmp(atname[i],"style")==0) {
        if (ptr->style !=NULL) free(ptr->style);
        ptr->style=malloc(strlen(atval[i]+1));
	strcpy(ptr->style,atval[i]);
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

  if (strcmp(name,"step")==0) {
    struct GrPlotValue *val;
    val=malloc(sizeof(struct GrPlotValue));
    val->cnv=&plt->cnv;
    val->inc=&plt->inc;
    val->val=&ptr->step;
    ptr->state=ptr->state | 0x02;
    blk=GrPlotSave(blk->xml,"step",val);
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


int GrPlotLabelEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotLabel *ptr;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotLabel *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
  if (plt==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    if (plot==NULL) return -1;
    if (ptr->state !=0x03) return -1;

    plot->lbl_wdt=ptr->off;
    plot->lbl_hgt=ptr->off;

    if (strcmp(name,"xlabel")==0) {
      plot->lbl_xor=ptr->or;
      PlotXaxisLabel(plot,plt->num,ptr->min,ptr->max,ptr->step,
              ptr->flg,GrPlotMakeText,ptr,ptr->fname,ptr->fsize,ptr->color,
              ptr->mask);
    }
    if (strcmp(name,"ylabel")==0) {
      plot->lbl_yor=ptr->or;
      PlotYaxisLabel(plot,plt->num,ptr->min,ptr->max,ptr->step,
		     ptr->flg,GrPlotMakeText,ptr,
                     ptr->fname,ptr->fsize,ptr->color,
		     ptr->mask);
    }

    free(ptr->fname);
    free(ptr->style);
    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}

