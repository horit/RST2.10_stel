/* plot_vec.c
   ========== 
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
#include "aacgm.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "calcvector.h"
#include "text_box.h"

/*
 $Log: plot_vec.c,v $
 Revision 1.2  2004/05/10 22:10:30  barnes
 Removed redundant code.

 Revision 1.1  2003/03/15 21:20:45  barnes
 Initial revision

*/


void plot_vec(struct Rplot *rplot,float px,float py,int or,double max,
              int magflg,
              float xoff,float yoff,float wdt,float hgt,float sf,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int color,char mask,float width,
              char *fontname,float fontsize,
              void *txtdata) {

  int s;
  char txt[256];
  double lon,lat;
  float mag;
  float map[2],pnt[2];
  float ax,ay,bx,by;
  float txbox[3];

  lat=90.0;
  lon=0.0;
  if (!magflg) {
    double mlat,mlon,glat,glon,r;
    int s;
    mlat=lat;
    mlon=lon;
    s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    lat=glat;
    lon=glon;
  }
  map[0]=lat;
  map[1]=lon;
   
  s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  if (s==-1) return;
  ax=xoff+pnt[0]*wdt;
  ay=yoff+pnt[1]*hgt;    
    
  RPosCalcVector(90.0,0.0,max*sf,0.0,&lat,&lon);

  if (!magflg) {
    double mlat,mlon,glat,glon,r;
    int s;
    mlat=lat;
    mlon=lon;
    s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    lat=glat;
    lon=glon;
  }
  map[0]=lat;
  map[1]=lon;
  s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  if (s==-1) return;

  bx=xoff+pnt[0]*wdt;
  by=yoff+pnt[1]*hgt;    
    
  mag=sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));

  RplotEllipse(rplot,NULL,px,py,
               2,2,1,color,mask,0,NULL);
   
  if (or==0) RplotLine(rplot,px,py,px+mag,py,color,0x0f,width,NULL);    
  else if (or==1) RplotLine(rplot,px,py,px-mag,py,color,0x0f,width,NULL);    
  else if (or==2) RplotLine(rplot,px,py-mag,px,py,color,0x0f,width,NULL);    
  else if (or==3) RplotLine(rplot,px,py,px,py+mag,color,0x0f,width,NULL);    
  
  sprintf(txt,"%g m/s",max);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  
  if (or==0) RplotText(rplot,NULL,fontname,fontsize,px-4,py-(txbox[2]-txbox[1]),
                       strlen(txt),txt,color,mask,1);
  

 
}
