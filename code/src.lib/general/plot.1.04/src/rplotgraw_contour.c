/* rplotgraw_contour.c
   =================== 
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
#include "rmath.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"

/*
 $Log: rplotgraw_contour.c,v $
 Revision 1.3  2004/05/04 22:43:16  barnes
 Changed math header name.

 Revision 1.2  2004/04/26 21:08:16  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/10/21 14:06:16  barnes
 Initial revision

*/


int PlotRawContour(struct Rplot *rplot,struct PolygonData *ptr,
                  float xoff,float yoff,float wdt,float hgt,
                  float step,float smooth,float lwdt,
                  int lstart,int lstep,
                  int sze,char *text,
                  unsigned int color,char mask,float width,
                  struct RplotDash *dash,
                  char *fntname,float fntsze,
                  int  (*text_box)(char *,float,int,char *,float *,void *),
                         void *boxdata) {

  struct RplotMatrix m;

  float txtbx[3];
 
  float x,y,u,v;
  float x0=0,y0=0;
  float x1,y1;
  float x2,y2;
  float x3,y3;
 
  int i,k,n,pn; 
  int lflg=0;
  float ang;
  float tang;
  float twdt;
  float tx,ty;
  float tw,th;
  float lw;
  int s=0;

  if (ptr==NULL) return -1;
  if (ptr->pntnum==0) return -1;
  if (fntname==NULL) return -1;
 
  if (text !=NULL) {
    if (text_box==NULL) return -1;
    s=(*text_box)(fntname,fntsze,sze,text,txtbx,boxdata);
    if (s !=0) return s;
  }
 
  if (step<0.01) step=0.01;
  if (step>0.5) step=0.5;
  if (smooth<0.1) smooth=0.1;
  

  for (n=0;n<ptr->polnum;n++) {
    pn=ptr->num[n];
    if (ptr->type[n] !=1) pn--;
    if (pn<2) continue;
    for (i=0;i<pn;i++) {

      lflg=((lstep !=0) && (text !=NULL) && 
	  (i-lstart) > 0 && (((i-lstart) % lstep)==0));

    
      x=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*i]*wdt+xoff;
      y=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*i+1]*hgt+yoff;
          
   
      k=(i<(ptr->num[n]-1)) ? (i+1) : 0;
  
      u=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*k]*wdt+xoff;
      v=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*k+1]*hgt+yoff;
   
      if (i==0) {    
        x0=(x+u)/2;
        y0=(y+v)/2;   
      } else  {
        if (lflg==0) {
          x3=(x+u)/2;
          y3=(y+v)/2;
          x1=x0+(x-x0)*smooth;
          y1=y0+(y-y0)*smooth;
          x2=x3+(x-x3)*smooth;
          y2=y3+(y-y3)*smooth;
          RplotBezier(rplot,x0,y0,x1,y1,x2,y2,x3,y3,color,mask,width,dash);
          x0=x3;
          y0=y3;
        } else { 
          ang=atan2(v-y,u-x);
          tang=ang;
          if (tang>(0.5*PI)) tang=tang-PI;
          if (tang<(-0.5*PI)) tang=tang+PI;
          twdt=sqrt((v-y)*(v-y)+(u-x)*(u-x));    
          if (twdt<lwdt) lw=twdt;
	  else lw=lwdt; 
          x3=(x+u-twdt*cos(ang))/2;
          y3=(y+v-twdt*sin(ang))/2;
          x1=x0+(x-x0)*smooth;
          y1=y0+(y-y0)*smooth;
          x2=x3+(x-x3)*smooth;
          y2=y3+(y-y3)*smooth;
          RplotBezier(rplot,x0,y0,x1,y1,x2,y2,x3,y3,color,mask,width,dash);
          x0=x3+lw*cos(ang);
          y0=y3+lw*sin(ang);
	  
          tx=(x+u)/2.0;
          ty=(y+v)/2.0;
          tw=txtbx[0]/2.0;
          th=(txtbx[1]+txtbx[2])/2;
          m.a=cos(tang);
          m.b=-sin(tang);
          m.c=sin(tang);
          m.d=cos(tang);

          RplotText(rplot,&m,fntname,fntsze,
                    tx-tw*cos(tang)-th*sin(tang),
                    ty-tw*sin(tang)+th*cos(tang),
		    sze,text,color,0x0f,1);
	  /* draw_text(txt,tang*180/PI);*/
	 
	          
        }
      }
    }
  

    /* complete the segment */

    if (ptr->type[n]==1) { /* closed contour */

      x=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[0]*wdt+xoff;
      y=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[1]*hgt+yoff;
 
      u=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2]*wdt+xoff;
      v=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[3]*hgt+yoff;
 
      x3=(x+u)/2;
      y3=(y+v)/2;
       
      x1=x0+(x-x0)*smooth;
      y1=y0+(y-y0)*smooth;
      x2=x3+(x-x3)*smooth;
      y2=y3+(y-y3)*smooth;
      RplotBezier(rplot,x0,y0,x1,y1,x2,y2,x3,y3,color,mask,width,dash);
    }
  }
  return 0;
}















