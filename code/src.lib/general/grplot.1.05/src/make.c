/* make.c
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
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "fontdb.h"
#include "rplotg.h"
#include "grplot.h"

/*
 $Log: make.c,v $
 Revision 1.1  2003/03/22 03:16:45  barnes
 Initial revision

*/

void GrPlotFree(struct GrPlotData *ptr) {
  if (ptr==NULL) return;
  free(ptr);
  return;
}

struct GrPlotData *GrPlotMake(struct XMLdata *xml) {
  struct GrPlotData *ptr;
  ptr=malloc(sizeof(struct GrPlotData));
  if (ptr==NULL) return NULL;
  ptr->xml=xml;
  ptr->rplot=NULL;
  ptr->plot=NULL;
  ptr->text.box=NULL;
  ptr->text.data=NULL;
  ptr->cnv.user=NULL;
  ptr->cnv.data=NULL;
  ptr->inc.user=NULL;
  ptr->inc.data=NULL;
  ptr->file.open=NULL;
  ptr->file.close=NULL;
  ptr->file.read=NULL;
  ptr->file.data=NULL;


  return ptr;
}

int GrPlotSetTextBox(struct GrPlotData *plt,
                   int  (*text)(char *,float,int,char *,float *,void *),
                   void *data) {
  if (plt==NULL) return -1;
  plt->text.box=text;
  plt->text.data=data;
  return 0;
}

int GrPlotSetMakeText(struct GrPlotData *plt,
		      char *(*text)(char *,double,double,double,void *),
		     void *data) {
  if (plt==NULL) return -1;
  plt->make.text=text;
  plt->make.data=data;
  return 0;
}

int GrPlotSetImageSize(struct GrPlotData *plt,
		       int (*size)(char *,int *,int *,void *),
		     void *data) {
  if (plt==NULL) return -1;
  plt->image.size=size;
  plt->image.data=data;
  return 0;
}



int GrPlotSetConvert(struct GrPlotData *plt,
                     int (*text)(char *,char *,double *,void *),
		      void *data) {
  if (plt==NULL) return -1;
  plt->cnv.user=text;
  plt->cnv.data=data;
  return 0;
}

int GrPlotSetInclude(struct GrPlotData *plt,
		   int (*text)(char *,int (*)(char *,int,void *),
				   void *,void *),void *data) {
  if (plt==NULL) return -1;
  plt->inc.user=text;
  plt->inc.data=data;
  return 0;
}


int GrPlotSetRplot(struct GrPlotData *plt,struct Rplot *rplot) {
  if (plt==NULL) return -1;
  plt->rplot=rplot;
  return 0;
}


int GrPlotSetOpenFile(struct GrPlotData *plt,
		      void *(*open)(char *,char *,char *,char *,void *)) {
  if (plt==NULL) return -1;
  plt->file.open=open;
  return 0;
}

int GrPlotSetCloseFile(struct GrPlotData *plt,
		      void (*close)(void *,void *)) {
  if (plt==NULL) return -1;
  plt->file.close=close;
  return 0;
}

int GrPlotSetReadFile(struct GrPlotData *plt,
		      int (*read)(void *,char *,char *,char *,
                                  double *,double *,void *)) {
  if (plt==NULL) return -1;
  plt->file.read=read;
  return 0;
}

int GrPlotSetFileData(struct GrPlotData *plt,void *data) {
  if (plt==NULL) return -1;
  plt->file.data=data;
  return 0;
}
