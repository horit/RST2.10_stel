/* rplotg_key.c
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
#include <math.h>
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rplotgraw.h"
#include "rplotgstd.h"
#include "rplotg.h"


/* 
 $Log: rplotg_key.c,v $
 Revision 1.1  2004/04/26 15:47:15  barnes
 Initial revision

 */


 
int PlotKey(struct PlotData *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double step,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
            int ksze,unsigned char *r,unsigned char *g,unsigned char *b) {

  int s=0;
  int xnm,ynm;
  float ox,oy;
  float tx,ty;
  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  
  if (key_flg & 0x01) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=PlotStdKey(plt->rplot,ox-xoff-wdt,ty+yoff,wdt,hgt,
                      xmin,xmax,step,0,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);
  }

  if (key_flg & 0x02) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=PlotStdKey(plt->rplot,ox+plt->box_wdt+xoff,ty+yoff,wdt,hgt,
                      xmin,xmax,step,0,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);

  }

  if (key_flg & 0x04) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=PlotStdKey(plt->rplot,tx-yoff,oy-xoff-wdt,hgt,wdt,
                      xmin,xmax,step,1,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);
  }

  if (key_flg & 0x08) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=PlotStdKey(plt->rplot,tx-yoff,oy+plt->box_hgt+xoff,hgt,wdt,
                      xmin,xmax,step,1,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);
  }

  return 0;
}

  

int PlotKeyLog(struct PlotData *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double tick_major,double tick_minor,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
            int ksze,unsigned char *r,unsigned char *g,unsigned char *b) {

  int s=0;
  int xnm,ynm;
  float ox,oy;
  float tx,ty;
  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  
  if (key_flg & 0x01) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=PlotStdKeyLog(plt->rplot,ox-xoff-wdt,ty+yoff,wdt,hgt,
                      xmin,xmax,tick_major,tick_minor,0,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);
  }

  if (key_flg & 0x02) {
    ty=oy+(plt->box_hgt-hgt)/2;  
    s=PlotStdKeyLog(plt->rplot,ox+plt->box_wdt+xoff,ty+yoff,wdt,hgt,
                      xmin,xmax,tick_major,tick_minor,0,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);

  }

  if (key_flg & 0x04) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=PlotStdKeyLog(plt->rplot,tx-yoff,oy-xoff-wdt,hgt,wdt,
                      xmin,xmax,tick_major,tick_minor,1,0,0,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);
  }

  if (key_flg & 0x08) {
    tx=ox+(plt->box_wdt-hgt)/2;  
    s=PlotStdKeyLog(plt->rplot,tx-yoff,oy+plt->box_hgt+xoff,hgt,wdt,
                      xmin,xmax,tick_major,tick_minor,1,0,1,ttl_num,ttl_txt,
                      plt->text.box,plt->text.data,make_text,textdata,
                      fntname,fntsze,color,mask,width,ksze,r,g,b);
  }

  return 0;
}

  
