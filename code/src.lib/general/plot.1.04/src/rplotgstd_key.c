/* rplotgstd_key.c
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
 $Log: rplotgstd_key.c,v $
 Revision 1.2  2004/04/26 21:08:16  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/09 21:36:49  barnes
 Initial revision

 */

int PlotStdKey(struct Rplot *ptr,
               float ox,float oy,float wdt,float hgt,
	       double min,double max,double step,
               int or,int txt_or,int tflg, 
               int ttl_num,char *ttl_text,
	       int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *r,unsigned char *g,unsigned char *b) {
  int s=0;
  


  float tstep,tlim;

  double tick_major=0;
  double tick_minor=0;
   
  float major_size=0;
  float minor_size=0;

  float txt_off=5;
  float ttl_off=5;
  
  int tick_flg=0x04;
  int txt_flg=0x02;
  int ttl_flg=0x01;
  
  int ttl_or=0;
 
  if (tflg==1) {
    txt_flg=0x01;
    if (or==0) ttl_flg=0x02;
  }

  tick_major=step;

  if (or==0) tstep=hgt*step/(max-min);
  else tstep=wdt*step/(max-min);

  if (make_text !=NULL) {
    float minbox[3];
    float maxbox[3];
    float w,h,t;
    char *mintxt=NULL;
    char *maxtxt=NULL;

    mintxt=(*make_text)(min,min,max,textdata);
    maxtxt=(*make_text)(max,min,max,textdata);

    if (mintxt !=NULL)
       s=(*text_box)(fntname,fntsze,strlen(mintxt),mintxt,minbox,boxdata);
    
    if ((s==0) && (maxtxt !=NULL)) 
       s=(*text_box)(fntname,fntsze,strlen(maxtxt),maxtxt,maxbox,boxdata);
    if (s !=0) {
      if (mintxt !=NULL) free(mintxt);
      if (maxtxt !=NULL) free(maxtxt);
      return s;
    }
    
    w=maxbox[0];
    h=(maxbox[1]+maxbox[2])/2;
   
    if (minbox[0]>w) w=minbox[0];
    t=(minbox[1]+minbox[2])/2;
    if (t>h) h=t;

    if (or==0) {
      if (txt_or==0) tlim=h;
      else tlim=w;
    } else {
      if (txt_or==0) tlim=w;
      else tlim=h;
    }

    if (tlim>tstep) {
      tick_major=max-min;
      tick_minor=step;
      tick_flg=0x08;
    }
    if (mintxt !=NULL) free(mintxt);
    if (maxtxt !=NULL) free(maxtxt);

  }

  return PlotRawKey(ptr,ox,oy,wdt,hgt,min,max,tick_major,tick_minor,
                    major_size,minor_size,txt_off,ttl_off,tick_flg,
                    txt_flg,ttl_flg,txt_or,ttl_or,or,
                    ttl_num,ttl_text,text_box,
                    boxdata,make_text,textdata,fntname,fntsze,color,
                    mask,width,ksze,r,g,b);


}







int PlotStdKeyLog(struct Rplot *ptr,
               float ox,float oy,float wdt,float hgt,
	       double min,double max,double tick_major,double tick_minor,
	       int or,int txt_or,int tflg, 
               int ttl_num,char *ttl_text,
	       int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *r,unsigned char *g,unsigned char *b) {
  int s=0;


  float tstep,tlim;


  float major_size=0;
  float minor_size=2;

  float txt_off=5;
  float ttl_off=5;
  
  int tick_flg=0x07;
  int txt_flg=0x02;
  int ttl_flg=0x01;
  
  int ttl_or=0;
 
  if (tflg==1) {
    txt_flg=0x01;
    if (or==0) ttl_flg=0x02;
  }

  if (or==0) tstep=hgt*tick_major/(max-min);
  else tstep=wdt*tick_minor/(max-min);

  if (make_text !=NULL) {
    float minbox[3]={0,0,0};
    float maxbox[3]={0,0,0};
    float w,h,t;
    char *mintxt=NULL;
    char *maxtxt=NULL;

    mintxt=(*make_text)(min,min,max,textdata);
    maxtxt=(*make_text)(max,min,max,textdata);
   
    if (mintxt !=NULL) 
      s=(*text_box)(fntname,fntsze,strlen(mintxt),mintxt,minbox,boxdata);
    if ((s==0) && (maxtxt !=NULL)) 
      s=(*text_box)(fntname,fntsze,strlen(maxtxt),maxtxt,maxbox,boxdata);
    
    if (s !=0) {
      if (maxtxt !=NULL) free(maxtxt);
      if (mintxt !=NULL) free(mintxt);
    }    

    w=maxbox[0];
    h=(maxbox[1]+maxbox[2])/2;
   
    if (minbox[0]>w) w=minbox[0];
    t=(minbox[1]+minbox[2])/2;
    if (t>h) h=t;

    if (or==0) {
      if (txt_or==0) tlim=h;
      else tlim=w;
    } else {
      if (txt_or==0) tlim=w;
      else tlim=h;
    }

    if (tlim>tstep) {
       s=PlotRawKeyLog(ptr,ox,oy,wdt,hgt,min,max,tick_major,tick_minor,
                    major_size,minor_size,txt_off,ttl_off,tick_flg,
                    txt_flg,ttl_flg,txt_or,ttl_or,or,
                    ttl_num,ttl_text,text_box,
                    boxdata,NULL,NULL,fntname,fntsze,color,
                    mask,width,ksze,r,g,b);
       if (or==0) {
	 if ((s==0) && (mintxt !=NULL))  
            s=PlotRawYaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,min,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(mintxt),mintxt,fntname,fntsze,color,mask);

         if ((s==0) && (maxtxt !=NULL)) 
            s=PlotRawYaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,max,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(maxtxt),maxtxt,fntname,fntsze,color,mask);

       } else {
         if ((s==0) && (mintxt !=NULL)) 
            s=PlotRawXaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,min,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(mintxt),mintxt,fntname,fntsze,color,mask);
         if ((s==0) && (maxtxt !=NULL))
            s=PlotRawXaxisLabelValue(ptr,ox,oy,wdt,hgt,min,max,max,txt_off,
                      txt_flg,txt_or,text_box,boxdata,
                      strlen(maxtxt),maxtxt,fntname,fntsze,color,mask);
       }
       if (mintxt !=NULL) free(mintxt);
       if (maxtxt !=NULL) free(maxtxt);
       return s;
    
    }
    if (mintxt !=NULL) free(mintxt);
    if (maxtxt !=NULL) free(maxtxt);
  }

  return PlotRawKeyLog(ptr,ox,oy,wdt,hgt,min,max,tick_major,tick_minor,
                    major_size,minor_size,txt_off,ttl_off,tick_flg,
                    txt_flg,ttl_flg,txt_or,ttl_or,or,
                    ttl_num,ttl_text,text_box,
                    boxdata,make_text,textdata,fntname,fntsze,color,
                    mask,width,ksze,r,g,b);


}





