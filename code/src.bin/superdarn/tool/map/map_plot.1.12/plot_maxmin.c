/* plot_maxmin.c
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
#include <sys/types.h>
#include "rtypes.h"
#include "aacgm.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rfile.h"
#include "cnvgrid.h"

/*
 $Log: plot_maxmin.c,v $
 Revision 1.2  2004/05/10 22:10:30  barnes
 Removed redundant code.

 Revision 1.1  2003/03/15 21:20:45  barnes
 Initial revision

*/


void plot_maxmin(struct Rplot *rplot,
                 struct CnvGrid *ptr,int magflg,
		 float xoff,float yoff,float wdt,float hgt,float sze,
                 int (*trnf)(int,void *,int,void *,void *data),void *data,
                 unsigned int color,char mask,float width,
                 struct RplotDash *dash) {

  int i,s;

  double min=1e10;
  double max=-1e10;
  float max_lon=0,max_lat=0;
  float min_lon=0,min_lat=0;
  float map[2],pnt[2];
  float px,py;

  for (i=0;i<ptr->num;i++) {
    if (ptr->mag[i]>max) {
       max_lon=ptr->lon[i];
       max_lat=ptr->lat[i];
       max=ptr->mag[i];
    }
    if (ptr->mag[i]<min) {
       min_lon=ptr->lon[i];
       min_lat=ptr->lat[i];
       min=ptr->mag[i];
    }
  }  
  
  if (!magflg) {
    double mlat,mlon,glat,glon,r;
    int s;
    mlat=max_lat;
    mlon=max_lon;
    s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    max_lat=glat;
    max_lon=glon;
    mlat=min_lat;
    mlon=min_lon;
    s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    min_lat=glat;
    min_lon=glon;
  }

   
  map[0]=max_lat;
  map[1]=max_lon;
   
  s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
  if (s==0) {
    px=xoff+pnt[0]*wdt;
    py=yoff+pnt[1]*hgt;       
    RplotLine(rplot,px-sze,py,px+sze,py,color,mask,width,dash);   
    RplotLine(rplot,px,py-sze,px,py+sze,color,mask,width,dash);   
 
  } 

  map[0]=min_lat;
  map[1]=min_lon;
   
  s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
  if (s==0) {
    px=xoff+pnt[0]*wdt;
    py=yoff+pnt[1]*hgt;       
    RplotLine(rplot,px-sze,py+sze,px+sze,py-sze,color,mask,width,dash);  
    RplotLine(rplot,px-sze,py-sze,px+sze,py+sze,color,mask,width,dash);  
  } 

}
