/* transform.c
   ===========
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
#ifndef _DARWIN
#include <malloc.h>
#endif
#include <math.h>
#include "polygon.h"

/*
 $Log: transform.c,v $
 Revision 1.2  2004/08/25 19:00:00  barnes
 Fixes for MacOS X (Darwin)

 Revision 1.1  2003/03/17 18:13:57  barnes
 Initial revision

*/

struct PolygonData *PolygonTransform(struct PolygonData *src,
                                     int sze,
				     int (*bbox)(void *x,
						void *y,int s),
                                     int (*trf)(int ssze,void *src,int dsze,
                                                void *dst,void *data),
                                     void *data) {

  int flg;
  void *spnt;
  char *dpnt;
  int n,s;
  int i,j;
  int op;
  struct PolygonData *dst=NULL;

  if (src==NULL) return NULL;
  if (trf==NULL) return NULL;

  dpnt=malloc(sze);
  if (dpnt==NULL) return NULL;
  dst=PolygonMake(sze,bbox);
  if (dst==NULL) {
    free(dpnt);
    return NULL;
  }
  
  for (i=0;i<src->polnum;i++) {    
    flg=1;
    n=src->num[i];   
    op=0;    
    for (j=0;j<n;j++) {
      spnt=PolygonRead(src,i,j);
      s=(*trf)(src->sze,spnt,sze,dpnt,data);
      if (s !=0) {
        op=1;
        continue;
      }
      if (flg==1) s=PolygonAddPolygon(dst,src->type[i]);
      if (s !=0) break;
      flg=0;
      s=PolygonAdd(dst,dpnt,op);
      if (s !=0) break;
      op=0;
    }
    if (j !=n) break;
  }
  free(dpnt);
  if (i !=src->polnum) {
    PolygonFree(dst);
    return NULL;
  } 
  return dst;
}


void PolygonModify(struct PolygonData *src,
                   int (*trf)(int ssze,void *src,int dsze,
                              void *dst,void *data),void *data) {

  int flg;
  void *pnt;
  int n,s;
  int i,j;
  if (src==NULL) return;
  if (trf==NULL) return;
 
  for (i=0;i<src->polnum;i++) {    
    flg=1;
    n=src->num[i];       
    for (j=0;j<n;j++) {
      pnt=PolygonRead(src,i,j);
      s=(*trf)(src->sze,pnt,src->sze,pnt,data);
    }
  }
}


