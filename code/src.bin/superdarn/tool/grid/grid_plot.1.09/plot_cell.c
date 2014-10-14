/* plot_cell.c
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
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include "rmath.h"
#include "rtypes.h"
#include "aacgm.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rfile.h"
#include "calcvector.h"
#include "griddata.h"

/*
 $Log: plot_cell.c,v $
 Revision 1.3  2004/10/08 19:00:43  barnes
 Added DataMap format.

 Revision 1.2  2004/05/10 21:56:29  barnes
 Removed redundant code.

 Revision 1.1  2003/04/25 14:49:00  barnes
 Initial revision

*/

int cell_convert(float xoff,float yoff,float wdt,float hgt,
                 float lat,float lon,float *px,float *py,int magflg,
                 int (*trnf)(int,void *,int,void *,void *data),void *data) {
  int s;
  float map[2],pnt[2];

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
    if (s !=0) return -1;
    *px=xoff+wdt*pnt[0];
    *py=yoff+hgt*pnt[1]; 
    return 0;
}

void plot_cell(struct Rplot *rplot,
               struct GridData *ptr,float latmin,int magflg,
               float xoff,float yoff,float wdt,float hgt,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
               unsigned int(*cfn)(double,void *),void *cdata,
               int cprm) {


  int i,s,nlon;
  double lon,lat,lstp;
 
  unsigned int color=0;
  float px[4],py[4];
  int t[4]={0,0,0,0};

  for (i=0;i<ptr->vcnum;i++) {
    if (cfn !=NULL) {
      if (cprm==0) color=(*cfn)(ptr->data[i].pwr.median,cdata);
      else color=(*cfn)(ptr->data[i].wdt.median,cdata);
    }
    lon=ptr->data[i].mlon;
    lat=ptr->data[i].mlat;
    if (lat<latmin) continue;
    nlon=(int) (360*cos((lat-0.5)*PI/180)+0.5);
    lstp=360.0/nlon; 
    s=cell_convert(xoff,yoff,wdt,hgt,lat-0.5,lon-lstp/2,&px[0],&py[0],
                 magflg,trnf,data);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat-0.5,lon+lstp/2,&px[1],&py[1],
                 magflg,trnf,data);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat+0.5,lon+lstp/2,&px[2],&py[2],
                 magflg,trnf,data);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat+0.5,lon-lstp/2,&px[3],&py[3],
                 magflg,trnf,data);
    if (s !=0) continue;   
    RplotPolygon(rplot,NULL,0,0,4,px,py,t,1,color,0x0f,0,NULL);
    
        
    
  } 

}
