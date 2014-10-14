/* plot_model.c
   ============ 
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
#include "calcvector.h"
#include "griddata.h"
#include "cnvmap.h"

/*
 $Log: plot_model.c,v $
 Revision 1.2  2004/05/10 22:10:30  barnes
 Removed redundant code.

 Revision 1.1  2003/03/15 21:20:45  barnes
 Initial revision

*/


void plot_model(struct Rplot *rplot,
              struct CnvMapData *ptr,float latmin,int magflg,
              float xoff,float yoff,float wdt,float hgt,float sf,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int(*cfn)(double,void *),void *cdata,
              float width) {

  int i,s;
  double olon,olat,lon,lat,vazm;
  float map[2],pnt[2];
  unsigned int color=0;
  float ax,ay,bx,by;

  for (i=0;i<ptr->num_model;i++) {
    
    olon=ptr->model[i].mlon;
    olat=ptr->model[i].mlat;
    vazm=ptr->model[i].azm;

    lat=olat;
    lon=olon;
    if (!magflg) {
      double mlat,mlon,glat,glon,r;
      int s;
      mlat=lat;
      mlon=lon;
      s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
      lat=glat;
      lon=glon;
    }

    if (fabs(lat)<fabs(latmin)) continue;
    if (cfn !=NULL) color=(*cfn)(ptr->model[i].vel.median,cdata);
    
    map[0]=lat;
    map[1]=lon;
   
    s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
    if (s==-1) continue;
    ax=xoff+pnt[0]*wdt;
    ay=yoff+pnt[1]*hgt;    
    
    RPosCalcVector(olat,olon,ptr->model[i].vel.median*sf,vazm,&lat,&lon);

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
    if (s==-1) continue;
    bx=xoff+pnt[0]*wdt;
    by=yoff+pnt[1]*hgt;    
    
    RplotEllipse(rplot,NULL,ax,ay,
                 2,2,1,color,0x0f,0,NULL);
    
    RplotLine(rplot,ax,ay,bx,by,color,0x0f,width,NULL);    
    
  } 
}
