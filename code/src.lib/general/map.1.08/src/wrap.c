/* wrap.c
   ====== 
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
 $Log: wrap.c,v $
 Revision 1.5  2006/02/03 21:40:28  barnes
 Changed the op type for the first element to 2, this allows for the proper
 splitting of the antarctic polygon in cylindrical projections, while
 allowing the clip detect algorithm to function.

 Revision 1.4  2004/05/05 15:04:10  barnes
 Added missing header.

 Revision 1.3  2004/05/04 22:44:58  barnes
 Changed math header name.

 Revision 1.2  2004/05/04 18:00:17  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/08/06 20:07:25  barnes
 Initial revision

*/

struct PolygonData *MapWraparound(struct PolygonData *map) {

  struct PolygonData *clip=NULL;
  struct PolygonData *ptr=NULL;
  struct PolygonData *dst=NULL;
  float *pnt;

  float dpnt[2];

  int c=0,i,j;
  int op;
  int left,right;
  int top,bottom;
 
  float ox,oy,px,py; 

  clip=MapSquareClip();
  if (clip==NULL) return NULL;

  ptr=PolygonMake(2*sizeof(float),PolygonXYbbox);
  
  for (i=0;i<map->polnum;i++) {
    left=0;
    right=0;
    top=0;
    bottom=0;
 
    PolygonAddPolygon(ptr,map->type[i]);
    pnt=(float *) PolygonRead(map,i,0);
    op=PolygonReadOp(map,i,0);
    ox=pnt[0];
    oy=pnt[1];
    dpnt[0]=ox;
    dpnt[1]=oy;
    PolygonAdd(ptr,dpnt,2);
    c++;
    for (j=1;j<map->num[i];j++) {
      pnt=(float *) PolygonRead(map,i,j);
      op=PolygonReadOp(map,i,j);
      px=pnt[0];
      py=pnt[1];
     

      if ((ox-px)>0.95) {
        px+=1.0;
        left=1;
      } else if ((ox-px)<-0.95) {
        px-=1.0;
        right=1;
      }

      if ((oy-py)>0.95) {
	py+=1.0;
        top=1;
      } else if ((oy-py)<-0.95) {
        py-=1.0;
        bottom=1;
      }
      dpnt[0]=px;
      dpnt[1]=py;
      PolygonAdd(ptr,dpnt,op);
      ox=px;
      oy=py;
    }
    
    if (right) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         op=PolygonReadOp(ptr,c-1,j);
         dpnt[0]=pnt[0]+1.0;
         dpnt[1]=pnt[1];
         PolygonAdd(ptr,dpnt,op);
      }   
      c++;         
    }
    
    if (left) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         op=PolygonReadOp(ptr,c-1,j);
         dpnt[0]=pnt[0]-1.0;
         dpnt[1]=pnt[1];
         PolygonAdd(ptr,dpnt,op);
      }       
      c++;
    }
    if (top) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         op=PolygonReadOp(ptr,c-1,j);
         dpnt[0]=pnt[0];
         dpnt[1]=pnt[1]+1.0;
         PolygonAdd(ptr,dpnt,op);
      }       
      c++;
    }
    if (bottom) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         op=PolygonReadOp(ptr,c-1,j);
         dpnt[0]=pnt[0];
         dpnt[1]=pnt[1]-1.0;
         PolygonAdd(ptr,dpnt,op);
      }       
      c++;
    }
    
  }      
  dst=PolygonClip(clip,ptr);  
  PolygonFree(clip);
  PolygonFree(ptr);
  return dst;
}

