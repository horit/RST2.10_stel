/* limit.c
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
#include "block.h"
#include "grplot.h"

/*
 $Log: limit.c,v $
 Revision 1.2  2004/06/16 16:40:29  barnes
 Added missing header.

 Revision 1.1  2003/03/22 03:16:45  barnes
 Initial revision

*/


int GrPlotLimitStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotLimit *ptr;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotLimit *) blk->data;
  if (ptr==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    ptr->state=0;
    return 0;
  }

  if (strcmp(name,"min")==0) {
    struct GrPlotValue *val;
    val=malloc(sizeof(struct GrPlotValue));
    val->cnv=ptr->cnv;
    val->inc=ptr->inc;
    val->val=ptr->min;
    ptr->state=ptr->state | 0x01;
    blk=GrPlotSave(blk->xml,"min",val);
    XMLSetStart(blk->xml,GrPlotValueStart,blk);
    XMLSetText(blk->xml,GrPlotValueText,blk);
    XMLSetEnd(blk->xml,GrPlotValueEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }
  
  if (strcmp(name,"max")==0) {
    struct GrPlotValue *val;
    val=malloc(sizeof(struct GrPlotValue));
    val->cnv=ptr->cnv;
    val->inc=ptr->inc;
    val->val=ptr->max;
    ptr->state=ptr->state | 0x02;
    blk=GrPlotSave(blk->xml,"max",val);
    XMLSetStart(blk->xml,GrPlotValueStart,blk);
    XMLSetText(blk->xml,GrPlotValueText,blk);
    XMLSetEnd(blk->xml,GrPlotValueEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"include")==0) {
    struct GrPlotInclude *inc;
    inc=malloc(sizeof(struct GrPlotInclude));
    inc->inc=ptr->inc;
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

int GrPlotLimitEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotLimit *ptr;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotLimit *) blk->data;
  if (ptr==NULL) return -1;
 
  if (strcmp(name,blk->name)==0) {
    if (ptr->state !=0x03) return -1;
    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}


