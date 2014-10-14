/* transform.c
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
#include <math.h> 
#include <sys/types.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rmath.h"
#include "rmap.h"

/*
 $Log: transform.c,v $
 Revision 1.5  2004/05/05 15:04:10  barnes
 Added missing header.

 Revision 1.4  2004/05/04 22:44:58  barnes
 Changed math header name.

 Revision 1.3  2004/05/04 18:00:17  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:03:12  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/08/06 20:07:06  barnes
 Initial revision

*/

int MapCylindrical(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  float lat,lon;
  float plon=0;
  int wrap=1;
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (data !=NULL) plon=((float *) data)[1];
  if (data !=NULL) wrap=((float *) data)[2];
  lon=lon-plon;
  pnt=(float *)dst;

  pnt[0]=lon/360.0;
  
  if (wrap) {
    if (pnt[0]<0) pnt[0]+=1.0;
    if (pnt[0]>=1) pnt[0]-=1.0;
  }
  pnt[1]=0.5-lat/180.0;
  return 0;
}



int MapMercator(int ssze,void *src,int dsze,void *dst,void *data) {

  float *pnt;
  float lat,lon;
  float plon=0;
  float wrap=1;

  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (data !=NULL) plon=((float *) data)[1];
  if (data !=NULL) wrap=((float *) data)[2];

  lon=lon-plon;
 
  pnt=(float *)dst;

  pnt[0]=lon/360.0;
  if (wrap) {
    if (pnt[0]<0) pnt[0]+=1.0;
    if (pnt[0]>=1) pnt[0]-=1.0;
  }
  if (lat==90) pnt[1]=0.0;
  else if (lat==-90) pnt[1]=1.0;
  else pnt[1]=0.5-0.25*log(tan(PI/4+lat*PI/360));
  return 0;
}


int MapTransverseMercator(int ssze,void *src,int dsze,void *dst,void *data) {

  float B;
  float plon=0;
  float *pnt;
  float lat,lon;
  float wrap=1;

  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];

  if (data !=NULL) plon=((float *) data)[1];
  if (data !=NULL) wrap=((float *) data)[2];

  lon=lon-plon;
  if (wrap) {
    if (lon<0) lon+=360;
    if (lon>=360) lon-=360;
  }
  B=cos(PI*lat/180.0)*sin(PI*lon/180.0);
  if (B==1)  return -1;
  if (B==-1) return -1;

  pnt=(float *)dst;
  pnt[1]=0.5+0.5*(1.0/(2.0*PI))*log((1+B)/(1-B));
  pnt[0]=0.5+(1.0/(2.0*PI))*atan2(tan(PI*lat/180.0),cos(PI*lon/180.0));
  return 0;
} 
  
int MapStereographic(int ssze,void *src,int dsze,void *dst,void *data) {
  float *pnt;
 
  float k=1.0,f=0;
  float z;
  float plon=0;
  float plat=0;
  float lat,lon;
 
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];
 
  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    plat=arg[0];
    plon=arg[1];
    k=arg[2];
    f=arg[3];
  }
 
  lat=lat*PI/180;
  lon=lon*PI/180;
  plat=plat*PI/180;
  plon=plon*PI/180;
 
  if ((plat>0) && (lat<=0)) return -1;
  if ((plat<0) && (lat>=0)) return -1;

  z=sin(plat)*sin(lat)+cos(plat)*cos(lat)*cos(lon-plon);
  if (z==-1) return -1;

  k=2*k/(1+z);

  pnt=(float *) dst;
  if (f==0) pnt[0]=(1.0+k*cos(lat)*sin(lon-plon))/2.0;
  else pnt[0]=(1.0-k*cos(lat)*sin(lon-plon))/2.0;
  pnt[1]=(1.0-k*(cos(plat)*sin(lat)-sin(plat)*cos(lat)*
                 cos(lon-plon)))/2.0;
 
 return 0;
}


int MapOrthographic(int ssze,void *src,int dsze,void *dst,void *data) {
  float *pnt;
  float z=0,f=0;
  float r=1.0;
  float sf=1.0;
  float plat=0,plon=0;

  float lat,lon;
 
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];
 
  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    plat=arg[0];
    plon=arg[1];
    sf=arg[2];
    f=arg[3];
  }

  z=sin(plat*PI/180)*sin(lat*PI/180)+
   cos(plat*PI/180)*cos(lat*PI/180)*
   cos(PI*(lon-plon)/180);

  if (z<0) return -1;
 
   pnt=(float *) dst;

  if (f==0) pnt[0]=(1.0+sf*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180))/2.0;
  else pnt[0]=(1.0-sf*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180))/2.0;
  pnt[1]=(1.0-sf*r*(cos(PI*plat/180)*
       sin(PI*lat/180)-
       sin(PI*plat/180)*cos(PI*lat/180)*
       cos(PI*(lon-plon)/180)))/2.0;
 return 0;
}



int MapGeneralVerticalPerspective(int ssze,void *src,int dsze,
                                  void *dst,void *data) {
  float *pnt;
  float z=0,f=0;
  float r=1.0;
  float K=1.0,P=0;
  float plat=0,plon=0;

  float lat,lon;
 
  pnt=(float *)src;
  lat=pnt[0];
  lon=pnt[1];
 
  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    plat=arg[0];
    plon=arg[1];
    P=arg[2];
    f=arg[3];
  }


  z=sin(plat*PI/180)*sin(lat*PI/180)+
   cos(plat*PI/180)*cos(lat*PI/180)*
   cos(PI*(lon-plon)/180);

  if (z<(1.0/P)) return -1;

  K=(P-1)/(P-z);
  r=P/(P-1); 
  pnt=(float *) dst;

  if (f==0) pnt[0]=0.5+0.5*K*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180);
  else pnt[0]=0.5-0.5*K*r*cos(PI*lat/180)*
       sin(PI*(lon-plon)/180);
  pnt[1]=0.5-0.5*K*r*(cos(PI*plat/180)*
       sin(PI*lat/180)-
       sin(PI*plat/180)*cos(PI*lat/180)*
       cos(PI*(lon-plon)/180));
 return 0;
}





