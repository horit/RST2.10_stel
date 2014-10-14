/* smybolstore.c
   =============
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
 $Log: symbolstore.c,v $
 Revision 1.2  2004/06/16 16:40:29  barnes
 Added missing header.

 Revision 1.1  2003/03/22 03:19:25  barnes
 Initial revision

*/

int GrPlotSymbolStoreText(char *buf,int sze,void *data) {
  struct GrPlotBlock *blk;
  struct GrPlotString *ptr;
  char *tmp;
  int o=0;
  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotString *) blk->data;
  if (ptr==NULL) return -1;

  if (buf==NULL) return 0;
  if (sze==0) return 0;

  if (ptr->buf !=NULL) o=strlen(ptr->buf);
  if (ptr->buf==NULL) ptr->buf=malloc(sze+1);
  else {
    tmp=realloc(ptr->buf,o+sze+1);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
  if (ptr->buf==NULL) return -1;
  memcpy(ptr->buf+o,buf,sze);
  ptr->buf[o+sze]=0;
  return 0;
}


int GrPlotSymbolStoreStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotString *ptr;
  char *tmp;
  int o=0;
  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotString *) blk->data;
  if (ptr==NULL) return -1;

  if (buf==NULL) return 0;
  if (sze==0) return 0;

  if (strcmp(name,blk->name)==0) {
    ptr->buf=NULL;
    return 0;
  }

  if (ptr->buf !=NULL) o=strlen(ptr->buf);
  if (ptr->buf==NULL) ptr->buf=malloc(sze+3);
  else {
    tmp=realloc(ptr->buf,o+sze+3);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
  if (ptr->buf==NULL) return -1;
  ptr->buf[o]='<';
  o++;
  memcpy(ptr->buf+o,buf,sze);
  ptr->buf[o+sze]='>';
  ptr->buf[o+sze+1]=0;
  return 0;
}

int GrPlotSymbolStoreEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotString *ptr;
  char *tmp;
  int o=0;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotString *) blk->data;
  if (ptr==NULL) return -1;
 
  if (buf==NULL) return 0;
  if (sze==0) return 0;


  if (strcmp(name,blk->name)==0) {
    if (ptr->buf==NULL) return -1;
    *(ptr->txt)=ptr->buf;
    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }

  if (ptr->buf !=NULL) o=strlen(ptr->buf);
  if (ptr->buf==NULL) ptr->buf=malloc(sze+3);
  else {
    tmp=realloc(ptr->buf,o+sze+3);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
  if (ptr->buf==NULL) return -1;
  ptr->buf[o]='<';
  o++;
  memcpy(ptr->buf+o,buf,sze);
  ptr->buf[o+sze]='>';
  ptr->buf[o+sze+1]=0;
  return 0;
}





