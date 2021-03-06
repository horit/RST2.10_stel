/* plot_radial.c
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

/* 
 $Log: plot_radial.c,v $
 Revision 1.4  2003/03/01 03:20:11  barnes
 Major modifications to support base libraries and Rplot XML output.

 Revision 1.3  2001/06/27 21:08:56  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.1  1998/11/12 18:10:38  barnes
 Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rfbuffer.h"
#include "rplot.h"
#include "rmap.h"

void plot_radial(struct Rplot *rplot,float *arg,float lat,
	         float start,float stop,float step,
	         float xoff,float yoff,float wdt,float hgt,
                 unsigned int color,unsigned char mask,float width,
                 struct RplotDash *dash) {


  float map[2]={0.0,0.0};
  float pnt[2];
  
  float ax,ay,bx,by;
  float lon;

  for (lon=start;lon<=stop;lon+=step) { 
    if (lat>0) map[0]=90.0;
    else map[0]=-90.0;
    map[1]=lon;
    MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,arg); 
    ax=pnt[0]*wdt+xoff;
    ay=pnt[1]*hgt+yoff;
    map[0]=lat;
    MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,arg); 
    bx=pnt[0]*wdt+xoff;
    by=pnt[1]*hgt+yoff;
    RplotLine(rplot,ax,ay,bx,by,color,mask,width,dash);
  }
}






















