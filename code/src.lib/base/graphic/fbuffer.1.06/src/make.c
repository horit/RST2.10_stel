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
#include "rfbuffer.h"

/*
 $Log: make.c,v $
 Revision 1.2  2004/06/16 16:23:14  barnes
 Added missing header.

 Revision 1.1  2003/03/01 19:03:22  barnes
 Initial revision

*/

struct FrameBuffer *FrameBufferMake(char *name,
                       int wdt,int hgt,int depth) {
  int s=0;
  int d=1;
  struct FrameBuffer *ptr;

  if (wdt==0) return NULL;
  if (hgt==0) return NULL;

  ptr=malloc(sizeof(struct FrameBuffer));
  if (ptr==NULL) return NULL;
  ptr->name=NULL;
  ptr->img=NULL;
  ptr->msk=NULL;

  ptr->user.pixel=NULL;
  ptr->user.data=NULL;

  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->depth=depth;

  if (name !=NULL) {
    ptr->name=malloc(strlen(name)+1);
    if (ptr->name !=NULL) strcpy(ptr->name,name);
    else s=-1;
  }

  if (depth !=8) d=3;
  if (s==0) ptr->img=malloc(wdt*hgt*d);
  if (ptr->img==NULL) s=-1;
  if (s==0) ptr->msk=malloc(wdt*hgt);
  if (ptr->msk==NULL) s=-1;
  

  if (s !=0) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk); 
    free(ptr);
    return NULL;
  }

  memset(ptr->msk,255,wdt*hgt);
  memset(ptr->img,0,wdt*hgt*d);
  
  return ptr;
 
}

void FrameBufferFree(struct FrameBuffer *ptr) {
  if (ptr==NULL) return;
  if (ptr->name !=NULL) free(ptr->name);
  if (ptr->img !=NULL) free(ptr->img);
  if (ptr->msk !=NULL) free(ptr->msk);
  free(ptr);
}

struct FrameBuffer *FrameBufferCopy(struct FrameBuffer *src) {

  int s=0;
  int d=1;
  struct FrameBuffer *dst;

  if (src==NULL) return NULL;
  if (src->wdt==0) return NULL;
  if (src->hgt==0) return NULL;
  if (src->img==NULL) return NULL;
  if (src->msk==NULL) return NULL;


  dst=malloc(sizeof(struct FrameBuffer));
  if (dst==NULL) return NULL;
  dst->name=NULL;
  dst->img=NULL;
  dst->msk=NULL;
  dst->user.pixel=NULL;
  dst->user.data=NULL;

  dst->wdt=src->wdt;
  dst->hgt=src->hgt;
  dst->depth=src->depth;

  if (src->name !=NULL) {
    dst->name=malloc(strlen(src->name)+1);
    if (dst->name !=NULL) strcpy(dst->name,src->name);
    else s=-1;
  }

  if (src->depth !=8) d=3;
  if (s==0) dst->img=malloc(src->wdt*src->hgt*d);
  if (dst->img==NULL) s=-1;
  if (s==0) dst->msk=malloc(src->wdt*src->hgt);
  if (dst->msk==NULL) s=-1;
  

  if (s !=0) {
    if (dst->name !=NULL) free(dst->name);
    if (dst->img !=NULL) free(dst->img);
    if (dst->msk !=NULL) free(dst->msk);
    free(dst);
    return NULL;
  }

  memcpy(dst->msk,src->msk,src->wdt*src->hgt);
  memcpy(dst->img,src->img,src->wdt*src->hgt*d);
  return dst;
}

int FrameBufferClear(struct FrameBuffer *ptr,unsigned int color,
                     unsigned char m) {
  unsigned char r,g,b,a;

  if (ptr==NULL) return -1;
  if (ptr->msk==NULL) return -1;
  if (ptr->img==NULL) return -1; 

  b=(color & 0xff);
  g=(color & 0xff00)>>8;
  r=(color & 0xff0000)>>16;
  a=(color & 0xff000000)>>24;
 
  if (m & 0x08) memset(ptr->msk,a,ptr->wdt*ptr->hgt);
  if (m & 0x04) memset(ptr->img,r,ptr->wdt*ptr->hgt);

  if (ptr->depth !=8) { 
    if (m & 0x02) memset(ptr->img+(ptr->wdt*ptr->hgt),g,ptr->wdt*ptr->hgt);
    if (m & 0x01) memset(ptr->img+2*(ptr->wdt*ptr->hgt),b,ptr->wdt*ptr->hgt);
  }
  return 0;
}

