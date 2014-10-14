/* map.c
   =====
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
#include "simage.h"
#include "raster.h"

/*
 $Log: map.c,v $
 Revision 1.3  2004/05/05 16:32:29  barnes
 Added missing header.

 Revision 1.2  2004/05/04 20:07:41  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/04/27 20:27:09  barnes
 Initial revision

*/


float *SImageMap(int wdt,int hgt,int mode,
		 int (*trf)(int ssze,void *src,int dsze, void *dst,void *data),
                 void *data,
		 struct SImage *ip,int inx) {
  int x,y;
  int s,i,j;
  float *pnt=NULL;
  float *zdata=NULL;
  int *vertex=NULL;
  int *mapping=NULL;
  float *image=NULL;
  int in;
  int num=0,tnum,poly=0,tpoly;
  float zeroval=-1000;
  int v;
  float ox,oy,px,py;


  in=ip->colors;
  pnt=malloc(sizeof(float)*ip->wdt*ip->hgt*2);
  zdata=malloc(sizeof(float)*ip->wdt*ip->hgt);
  vertex=malloc(sizeof(int)*ip->wdt*ip->hgt*4);
  mapping=malloc(sizeof(int)*ip->wdt*ip->hgt);
 
  for (x=0;x<ip->wdt;x++) {
    for (y=0;y<ip->hgt;y++) {

      if (ip->pnt[x*ip->hgt+y].flg==0) {
	if (mode==0) {
          pnt[2*num]=ip->pnt[x*ip->hgt+y].glat;
          pnt[2*num+1]=ip->pnt[x*ip->hgt+y].glon;
	} else {
          pnt[2*num]=ip->pnt[x*ip->hgt+y].mlat;
          pnt[2*num+1]=ip->pnt[x*ip->hgt+y].mlon;
	}
	zdata[num]=ip->data[(x*ip->hgt+y)*in+inx];
      } else {
        pnt[2*num]=-1e10;
        pnt[2*num+1]=-1e10;
        zdata[num]=-1e10;
      }
      if ((x !=(ip->wdt-1)) && (y !=(ip->hgt-1))) {
        vertex[4*poly]=num;
        vertex[4*poly+1]=num+1;
        vertex[4*poly+2]=num+ip->hgt+1;
        vertex[4*poly+3]=num+ip->hgt;
        poly++;
      }
      num++;
    }
  }
  tnum=0;
  for (i=0;i<num;i++) {
    mapping[i]=-1;
    if ((pnt[2*i]<-1e4) || (pnt[2*i+1]<-1e4)) s=1;
    else s=(*trf)(2*sizeof(float),&pnt[2*i],2*sizeof(float),
              &pnt[2*tnum],data);
    if (s !=0) continue;
    mapping[i]=tnum;
    zdata[tnum]=zdata[i];
    pnt[2*tnum]=pnt[2*tnum]*wdt;
    pnt[2*tnum+1]=pnt[2*tnum+1]*hgt;
    tnum++;
  }

  tpoly=0;
  for (i=0;i<poly;i++) {
    if (mapping[vertex[4*i]]==-1) continue;
    v=mapping[vertex[4*i]];
    vertex[4*tpoly]=v;
    ox=pnt[2*v];
    oy=pnt[2*v+1]; 
    for (j=1;j<4;j++) {
      if (mapping[vertex[4*i+j]]==-1) break;
      v=mapping[vertex[4*i+j]];
      vertex[4*tpoly+j]=v;
      px=pnt[2*v];
      py=pnt[2*v+1]; 
      if (fabs(px-ox)>0.5*wdt) break;
      if (fabs(py-oy)>0.5*hgt) break;
      ox=px;
      oy=py;
    }
    if (j<4) continue;
    tpoly++;
  }
  image=Raster(wdt,hgt,1,raster_FLOAT,&zeroval,tpoly,pnt,vertex,
               zdata);
  free(pnt);
  free(vertex);
  free(mapping);
  free(zdata);
  return image;

}



