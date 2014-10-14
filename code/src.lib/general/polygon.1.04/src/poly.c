/* poly.c
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
#ifndef _DARWIN
#include <malloc.h>
#endif
#include <math.h>
#include "polygon.h"

/*
 $Log: poly.c,v $
 Revision 1.3  2004/08/25 19:00:00  barnes
 Fixes for MacOS X (Darwin)

 Revision 1.2  2004/06/16 16:58:53  barnes
 Added missing header.

 Revision 1.1  2003/03/17 18:13:35  barnes
 Initial revision

*/


struct PolygonData *PolygonMake(int sze,int (*bbox)(void *x,
                                                       void *y,int s)) {

  struct PolygonData *a;

  a=malloc(sizeof(struct PolygonData));
  a->polnum=0;
  a->pntnum=0;
  a->num=NULL;
  a->off=NULL;
  a->type=NULL;
  a->data=NULL;
  a->op=NULL;
  a->box=NULL;
  a->bbox=bbox;
  a->sze=sze;

  return a;
}


void PolygonFree(struct PolygonData *a) {
  if (a==NULL) return;
  if (a->num !=NULL) free(a->num);
  if (a->off !=NULL) free(a->off);
  if (a->type !=NULL) free(a->type);
  if (a->data !=NULL) free(a->data);
  if (a->op !=NULL) free(a->op);
  if (a->box !=NULL) free(a->box);
  free(a);
}
 
int PolygonAddPolygon(struct PolygonData *a,int type) {
  void *tmp;
 
  if (a->num==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->num,(a->polnum+1)*sizeof(int));
  if (tmp==NULL) return -1;
  a->num=tmp;

  if (a->off==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->off,(a->polnum+1)*sizeof(int));
  if (tmp==NULL) return -1;
  a->off=tmp;

  if (a->type==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->type,(a->polnum+1)*sizeof(int));
  if (tmp==NULL) return -1;
  a->type=tmp;

  if (a->box==NULL) tmp=malloc(2*a->sze);
  else tmp=realloc(a->box,(a->polnum+1)*2*a->sze);
  if (tmp==NULL) return -1;
  a->box=tmp;

  if (a->bbox !=NULL) 
     a->bbox(NULL,(char *) a->box+(2*a->sze*(a->polnum)),a->sze);

  a->type[a->polnum]=type;
  a->num[a->polnum]=0;
  a->off[a->polnum]=a->pntnum;
  a->polnum++;
  return 0;
}

int PolygonAdd(struct PolygonData *a,void *pnt,int op) {
  void *tmp;
  if (a->data==NULL) tmp=malloc(a->sze);
  else tmp=realloc(a->data,a->sze*(a->pntnum+1));
  if (tmp==NULL) return -1;
  a->data=tmp;

  if (a->op==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->op,sizeof(int)*(a->pntnum+1));
  if (tmp==NULL) return -1;
  a->op=tmp;
  a->op[a->pntnum]=op;

  memcpy((void *) ((char *) a->data+(a->sze*a->pntnum)),pnt,a->sze);
  
  if (a->bbox !=NULL) 
     a->bbox(pnt,(char *) a->box+(2*a->sze*(a->polnum-1)),a->sze);

  a->pntnum++;
  a->num[a->polnum-1]++;
  return 0; 
}

int PolygonReadType(struct PolygonData *ptr,int n) {
  if (ptr==NULL) return -1;
  if (n>=ptr->polnum) return -1;
  return ptr->type[n];
}

void *PolygonRead(struct PolygonData *ptr,int n,int o) {
  if (ptr==NULL) return NULL;
  if (n>=ptr->polnum) return NULL;
  if (o>=ptr->num[n]) return NULL;
  return  (void *) ((char *) ptr->data+(ptr->off[n]+o)*ptr->sze);
}
 
int PolygonReadOp(struct PolygonData *ptr,int n,int o) {
  if (ptr==NULL) return -1;
  if (n>=ptr->polnum) return -1;
  if (o>=ptr->num[n]) return -1;
  return ptr->op[ptr->off[n]+o];
}
 

 
struct PolygonData *PolygonCopy(struct PolygonData *src) {
  int s=0;
  struct PolygonData *dst=NULL;
  dst=malloc(sizeof(struct PolygonData));
  if (dst==NULL) return NULL;
  dst->num=NULL;
  dst->off=NULL;
  dst->type=NULL;
  dst->box=NULL;
  dst->data=NULL;

  dst->num=malloc(sizeof(int)*(src->polnum+1));
  if (dst->num==NULL) s=-1;
  dst->off=malloc(sizeof(int)*(src->polnum+1));
  if (dst->off==NULL) s=-1;
  dst->type=malloc(sizeof(int)*(src->polnum+1));
  if (dst->type==NULL) s=-1;
  dst->box=malloc(2*src->sze*(src->polnum+1));
  if (dst->box==NULL) s=-1;
  dst->data=malloc(src->sze*src->pntnum);
  if (dst->data==NULL) s=-1;
  dst->op=malloc(sizeof(int)*src->pntnum);
  if (dst->op==NULL) s=-1;
 
  if (s !=0) {
    if (dst->num !=NULL) free(dst->num);
    if (dst->off !=NULL) free(dst->off);
    if (dst->type !=NULL) free(dst->type);
    if (dst->box !=NULL) free(dst->box);
    if (dst->data !=NULL) free(dst->data);
    if (dst->op !=NULL) free(dst->op);
    free(dst);
  }
  dst->polnum=src->polnum;
  dst->pntnum=src->pntnum;
  dst->sze=src->sze;
  dst->bbox=src->bbox;

  memcpy(dst->num,src->num,sizeof(int)*(src->polnum+1));
  memcpy(dst->off,src->off,sizeof(int)*(src->polnum+1));
  memcpy(dst->type,src->type,sizeof(int)*(src->polnum+1));
  memcpy(dst->box,src->box,src->sze*2*(src->polnum+1));
  memcpy(dst->data,src->data,src->sze*src->pntnum);
  memcpy(dst->op,src->op,sizeof(int)*src->pntnum);
  return dst;
}


int PolygonRemove(struct PolygonData *a) {
  void *tmp;
  if (a==NULL) return -1;
  if (a->polnum==0) return -1;
  a->pntnum-=a->num[a->polnum-1];
  a->num[a->polnum-1]=0;

  if (a->pntnum==0) {
    free(a->data);
    a->data=NULL;
    free(a->op);
    a->op=NULL;
  } else {
    tmp=realloc(a->data,a->sze*a->pntnum);
    if (tmp==NULL) return -1;
    a->data=tmp;
    tmp=realloc(a->op,sizeof(int)*a->pntnum);
    if (tmp==NULL) return -1;
    a->op=tmp;
  }

  a->polnum--;
  if (a->polnum==0) {
    free(a->num);
    free(a->off);
    free(a->type);
    free(a->box);
    a->num=NULL;
    a->off=NULL;
    a->type=NULL;
    a->box=NULL;
  } else {
    tmp=realloc(a->num,a->polnum*sizeof(int));
    if (tmp==NULL) return -1;
    a->num=tmp;

    tmp=realloc(a->off,a->polnum*sizeof(int));
    if (tmp==NULL) return -1;
    a->off=tmp;

    tmp=realloc(a->type,a->polnum*sizeof(int));
    if (tmp==NULL) return -1;
    a->type=tmp;

    tmp=realloc(a->box,a->polnum*2*a->sze);
    if (tmp==NULL) return -1;
    a->box=tmp;
  }
 
  return 0;
}  

