/* rplotgraw_key.c
   ===============
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
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rplotgraw.h"

/* 
 $Log: rplotgraw_key.c,v $
 Revision 1.1  2003/03/09 21:36:49  barnes
 Initial revision

 Revision 1.1  2003/03/05 15:31:17  barnes
 Initial revision

 */

int PlotRawKey(struct Rplot *ptr,
               float ox,float oy,float wdt,float hgt,
	       double min,double max,
     	       double tick_major,double tick_minor,
               float major_size,float minor_size,
               float txt_off,float ttl_off,
               int tick_flg,int txt_flg,int ttl_flg,
               int txt_or,int ttl_or,
               int or, 
               int ttl_num,char *ttl_text,
	       int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *r,unsigned char *g,unsigned char *b) {
  int s=0;
  int i=0;
  struct FrameBuffer *img=NULL;

  if (ptr==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (fntsze==0) return -1;
  if (ksze==0) return -1;
  

  if (or==0) img=FrameBufferMake(NULL,1,ksze,24);
  else img=FrameBufferMake(NULL,ksze,1,24);

  if (img==NULL) return -1;

  if (or==0) {
    for (i=0;i<ksze;i++) {
      if (r !=NULL) img->img[ksze-1-i]=r[i];
      if (g !=NULL) img->img[2*ksze-1-i]=g[i];
      if (b !=NULL) img->img[3*ksze-1-i]=b[i];
    }
  } else {
    for (i=0;i<ksze;i++) {
      if (r !=NULL) img->img[i]=r[i];
      if (g !=NULL) img->img[ksze+i]=g[i];
      if (b !=NULL) img->img[2*ksze+i]=b[i];
    }
  }


  s=PlotRawFitImage(ptr,ox,oy,wdt,hgt,img,mask);
  
  if (s==0) s=PlotRawPanel(ptr,ox,oy,wdt,hgt,color,mask,width);
  if (or==0) {
    if (s==0) s=PlotRawYaxis(ptr,ox,oy,wdt,hgt,
                 min,max,tick_major,tick_minor,major_size,minor_size,
                 tick_flg,color,mask,width);
    if ((s==0) && (make_text !=NULL)) 
         s=PlotRawYaxisLabel(ptr,ox,oy,wdt,hgt,min,max,tick_major,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
		      make_text,textdata,fntname,fntsze,color,mask);
    
  } else {
    if (s==0) s=PlotRawXaxis(ptr,ox,oy,wdt,hgt,
                 min,max,tick_major,tick_minor,major_size,minor_size,
                 tick_flg,color,mask,width);
    if ((s==0) && (make_text !=NULL))
        s=PlotRawXaxisLabel(ptr,ox,oy,wdt,hgt,min,max,tick_major,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      make_text,textdata,fntname,fntsze,color,mask);
  }

  if ((s==0) && (ttl_text !=NULL) && (ttl_num !=0)) 
      s=PlotRawYaxisTitle(ptr,ox,oy,wdt,hgt,ttl_off,ttl_flg,ttl_or,
                          text_box,boxdata,
                          ttl_num,ttl_text,fntname,fntsze,color,mask);

  FrameBufferFree(img);
  return s;
}





int PlotRawKeyLog(struct Rplot *ptr,
               float ox,float oy,float wdt,float hgt,
               double min,double max,
               double tick_major,double tick_minor,
               float major_size,float minor_size,
               float txt_off,float ttl_off,
               int tick_flg,int txt_flg,int ttl_flg,
               int txt_or,int ttl_or,
               int or,
               int ttl_num,char *ttl_text,
               int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *r,unsigned char *g,unsigned char *b) {

  int s=0;
  int i=0;
  struct FrameBuffer *img;

  if (ptr==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (fntsze==0) return -1;
  if (ksze==0) return -1;
  

  if (or==0) img=FrameBufferMake(NULL,1,ksze,24);
  else img=FrameBufferMake(NULL,ksze,1,24);

  if (img==NULL) return -1;

  if (or==0) {
    for (i=0;i<ksze;i++) {
      if (r !=NULL) img->img[ksze-1-i]=r[i];
      if (g !=NULL) img->img[2*ksze-1-i]=g[i];
      if (b !=NULL) img->img[3*ksze-1-i]=b[i];
    }
  } else {
    for (i=0;i<ksze;i++) {
      if (r !=NULL) img->img[i]=r[i];
      if (g !=NULL) img->img[ksze+i]=g[i];
      if (b !=NULL) img->img[2*ksze+i]=b[i];
    }
  }
 
  s=PlotRawFitImage(ptr,ox,oy,wdt,hgt,img,mask);
 
  if (s==0) s=PlotRawPanel(ptr,ox,oy,wdt,hgt,color,mask,width);
  if (or==0) {
    if (s==0) s=PlotRawYaxisLog(ptr,ox,oy,wdt,hgt,
                 min,max,tick_major,tick_minor,major_size,minor_size,
                 tick_flg,color,mask,width);
    if ((s==0) && (make_text !=NULL))
       s=PlotRawYaxisLabel(ptr,ox,oy,wdt,hgt,min,max,tick_major,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      make_text,textdata,fntname,fntsze,color,mask);

  } else {
    if (s==0) s=PlotRawXaxisLog(ptr,ox,oy,wdt,hgt,
                 min,max,tick_major,tick_minor,major_size,minor_size,
                 tick_flg,color,mask,width);
    if ((s==0) && (make_text !=NULL)) 
       s=PlotRawXaxisLabel(ptr,ox,oy,wdt,hgt,min,max,tick_major,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      make_text,textdata,fntname,fntsze,color,mask);
  }


  if ((s==0) && (ttl_text !=NULL) && (ttl_num !=0)) 
      s=PlotRawYaxisTitle(ptr,ox,oy,wdt,hgt,ttl_off,ttl_flg,ttl_or,
                          text_box,boxdata,
                           ttl_num,ttl_text,fntname,fntsze,color,mask);
  FrameBufferFree(img);
  return s;
}







