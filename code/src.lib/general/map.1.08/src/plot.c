/* plot.c
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
 $Log: plot.c,v $
 Revision 1.6  2006/02/03 21:40:28  barnes
 Reinstated suspect code - it is for making sure that after clipping
 we do not get "jumps" - long straight line segments that do not follow
 the edge of the globe.

 Revision 1.5  2006/02/03 21:28:58  barnes
 Commented out suspect code that doesn't seem to server a purpose at this
 time.

 Revision 1.4  2004/05/05 15:04:10  barnes
 Added missing header.

 Revision 1.3  2004/05/04 22:44:58  barnes
 Changed math header name.

 Revision 1.2  2004/05/04 18:00:17  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/08/06 20:07:48  barnes
 Initial revision

*/

void MapPlotPolygon(struct Rplot *rplot,struct RplotMatrix *matrix,
                    float x,float y,float w,float h,int fill,
                    unsigned int color,unsigned char mask,
                    float width,struct RplotDash *dash,
                    struct PolygonData *poly,int type) {
  float *ptr;
  float *px=NULL,*py=NULL;
  int *t=NULL;

  
  int *stbl=NULL,snum=0;
  float dx,dy;
  float tf,nrm;
  int n,s,vx,vy,c,k;

  int i,j,op;

  if (poly==NULL) return;
  
  stbl=malloc(sizeof(int)*poly->polnum);
  
  for (i=0;i<poly->polnum;i++) {  
    if (poly->type[i] !=type) continue;
    px=malloc(sizeof(float)*poly->num[i]);
    py=malloc(sizeof(float)*poly->num[i]);
    t=malloc(sizeof(int)*poly->num[i]);
    for (j=0;j<poly->num[i];j++) {
      op=PolygonReadOp(poly,i,j);
            
      if (op==1) break;
      
      ptr=(float *) PolygonRead(poly,i,j);
      px[j]=ptr[0]*w;
      py[j]=ptr[1]*h;
      t[j]=0;
    }
    
    if (j==poly->num[i]) 
      RplotPolygon(rplot,matrix,x,y,poly->num[i],px,py,t,fill,
                 color,mask,width,dash);
    else {
     stbl[snum]=i;
     snum++;
    }
    
    free(px);
    free(py);
    free(t);
  }

  /* this code is to make sure that after a clipping event, extra path elements
     are added to remove jumps */
 

  
  for (i=0;i<snum;i++) { /* split these polygons */
    n=stbl[i];
    px=malloc(sizeof(float)*(64+poly->num[n]));
    py=malloc(sizeof(float)*(64+poly->num[n]));
    t=malloc(sizeof(int)*(64+poly->num[n]));

    s=0;
    while (PolygonReadOp(poly,n,s)==0) s++;
    vx=s;
    do {
      vy=vx;
      do {
        vy++;
        if (vy==poly->num[n]) vy=0;
      } while (PolygonReadOp(poly,n,vy)==0);
     
      c=vx;
      j=0;
      do {
        ptr=(float *) PolygonRead(poly,n,c);
        px[j]=ptr[0]*w;
        py[j]=ptr[1]*h;
        t[j]=0;
        c++;
        j++;
        if (c==poly->num[n]) c=0;
      } while (c !=vy);
      k=j-1;
      for (tf=0;tf<1.0;tf+=0.05) {
        dx=(1-tf)*px[k]+px[0]*tf;
        dy=(1-tf)*py[k]+py[0]*tf;
        dx=(dx/w)-0.5;
        dy=(dy/h)-0.5;
        nrm=sqrt(dx*dx+dy*dy);
        dx=dx/nrm;
        dy=dy/nrm;
        dx=(dx/2+0.5)*w;
        dy=(dy/2+0.5)*h;
        px[j]=dx; 
        py[j]=dy;
        t[j]=0;
        j++;
      }
      RplotPolygon(rplot,matrix,x,y,j,px,py,t,fill,
                   color,mask,width,dash);
      vx=vy;
    } while (vx !=s);
    
 
    free(px);
    free(py);
    free(t);
  }
  free (stbl);
  
}

void MapPlotOpenPolygon(struct Rplot *rplot,struct RplotMatrix *matrix,
                    float x,float y,float w,float h,
                    unsigned int color,unsigned char mask,
                    float width,struct RplotDash *dash,
                    struct PolygonData *poly,int type) {
  float *ptr;
  int i,j;
  int op;
  float ox,oy,nx,ny;
  
  if (poly==NULL) return;

  for (i=0;i<poly->polnum;i++) { 
    if ((type !=-1) && (poly->type[i] !=type)) continue;
    ptr=(float *) PolygonRead(poly,i,0);
    ox=x+ptr[0]*w;
    oy=y+ptr[1]*h;
    if (matrix !=NULL) RplotMatrixTransform(matrix,ox,oy,&ox,&oy);

    for (j=1;j<poly->num[i];j++) {
      ptr=(float *) PolygonRead(poly,i,j);
      op=PolygonReadOp(poly,i,j);
      nx=x+ptr[0]*w;
      ny=y+ptr[1]*h;
      if (matrix !=NULL) RplotMatrixTransform(matrix,nx,ny,&nx,&ny);
      if (op==0) RplotLine(rplot,ox,oy,nx,ny,
                           color,mask,width,dash);
      ox=nx;
      oy=ny;
    }   
  }
}

