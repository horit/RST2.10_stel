/* isect.c
   =======
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
#ifndef _DARWIN
#include <malloc.h>
#endif
#include <math.h>
#include "polygon.h"

/*
 $Log: isect.c,v $
 Revision 1.4  2004/08/25 19:00:00  barnes
 Fixes for MacOS X (Darwin)

 Revision 1.3  2004/06/16 16:58:53  barnes
 Added missing header.

 Revision 1.2  2004/04/26 21:10:34  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/17 18:13:03  barnes
 Initial revision

*/

int PolygonXYbbox(void *a,void *b,int s) {

  float *pnt;
  float *box;

  pnt=(float *) a;
  box=(float *) b;

  if (pnt==NULL) {
    box[0]=1e16;
    box[1]=1e16;
    box[2]=-1e16;
    box[3]=-1e16;
    return 0;
  }  

  if (pnt[0]<box[0]) box[0]=pnt[0];
  if (pnt[1]<box[1]) box[1]=pnt[1];
  if (pnt[0]>box[2]) box[2]=pnt[0];
  if (pnt[1]>box[3]) box[3]=pnt[1];

  return 0;

}

int PolygonTestBox(float *a,float *b) {
 int s;
 s=((a[0]<b[0]) && (a[2]<b[0])) ||
   ((a[1]<b[1]) && (a[3]<b[1])) ||
   ((a[0]>b[2]) && (a[2]>b[2])) ||
   ((a[1]>b[3]) && (a[3]>b[3]));
 return s;

}

float *PolygonCoord(void *a,int s) {
  float *pnt;
  pnt=(float *) a;
  return pnt;
}


struct PolygonData *PolygonClip(struct PolygonData *apol,
                                struct PolygonData *bpol) {

  /* algorithm calculates intersection with each boundary */

  int an;
  int xn,n,j;
  float ax,ay,bx,by;
  float *ptr;
  char *pp,*qp;
  float ip[2];
  char *ipnt;
  int wp,wq,p,q;
  int m,r,s;
  int opp,opq;

 
  float ma,mb,mc,md; /* matrix */
  float determ;      /* determinant */
  float ia,ib,ic,id; /* inverse */
  float dx,dy,dt;
  float sx,sy,tx,ty;
  float kl,jl;
 
  struct PolygonData *xpol=NULL;
  struct PolygonData *ypol=NULL;

  if ((apol==NULL) || (bpol==NULL)) return NULL;

  an=apol->num[0];

  ipnt=malloc(bpol->sze);
  xpol=bpol;
  ypol=PolygonMake(bpol->sze,PolygonXYbbox);

  for (m=1;m<=an;m++) {

    r=m-1;
    s=(m==an) ? 0 : m;
    ptr=PolygonCoord((char *) apol->data+(apol->off[0]+r)*apol->sze,
                     apol->sze);
    ax=ptr[0];
    ay=ptr[1];
    ptr=PolygonCoord((char *) apol->data+(apol->off[0]+s)*apol->sze,
                     apol->sze);
    bx=ptr[0];
    by=ptr[1];

  
    /* co-ordinate set of line segment */

    dx=bx-ax;
    dy=by-ay;

    dt=sqrt(dx*dx+dy*dy);
   
    ma=dx/dt;
    mb=-dy/dt;
    mc=dy/dt;
    md=dx/dt;

   /* find inverse of equation */

   determ=1/(ma*md-mb*mc);
   ia=md*determ;
   ib=-mb*determ;
   ic=-mc*determ;
   id=ma*determ;

  
   for (j=0;j<xpol->polnum;j++) {    
     /*
     if (PolygonTestBox( (float *) ((char *) xpol->box+xpol->sze*2*j), 
                         (float *) apol->box)==1) continue; 
     */

     PolygonAddPolygon(ypol,xpol->type[j]);
     xn=xpol->num[j];     
     for (n=1;n<=xn;n++) {
  
       p=n-1;
       q=(n==xn) ? 0 : n;
       
       opq=xpol->op[xpol->off[j]+q];
       opp=xpol->op[xpol->off[j]+p];

       pp=((char *) xpol->data+(xpol->off[j]+p)*xpol->sze);
       qp=((char *) xpol->data+(xpol->off[j]+q)*xpol->sze);

       ptr=PolygonCoord(pp,xpol->sze);


       ty=ic*(ptr[0]-ax)+id*(ptr[1]-ay);
       wp=(ty>0);
       ptr=PolygonCoord(qp,xpol->sze);
  
       ty=ic*(ptr[0]-ax)+id*(ptr[1]-ay);
       wq=(ty>0);
 
 
       if (wp && wq) {
         if (p==0) PolygonAdd(ypol,pp,opp);      
         if (q !=0) PolygonAdd(ypol,qp,opq); 
       } else if (wp != wq) {
         if ((wp) && (p==0)) PolygonAdd(ypol,pp,opp); 
           
         /* add intersection point here */

         ptr=PolygonCoord(pp,xpol->sze);

         sx=ia*(ptr[0]-ax)+ib*(ptr[1]-ay);
         sy=ic*(ptr[0]-ax)+id*(ptr[1]-ay);

         ptr=PolygonCoord(qp,apol->sze);

         tx=ia*(ptr[0]-ax)+ib*(ptr[1]-ay);
         ty=ic*(ptr[0]-ax)+id*(ptr[1]-ay);

         if (tx !=sx) {
           kl=(ty-sy)/(tx-sx);
           jl=sy-sx*kl;
           ip[0]=ma*-jl/kl+ax;
           ip[1]=mc*-jl/kl+ay;
          } else {
           ip[0]=ma*sx+ax;
           ip[1]=mc*sx+ay;
          }    

	  memcpy(ipnt,qp,xpol->sze);
          ptr=PolygonCoord(ipnt,xpol->sze);
          ptr[0]=ip[0];
          ptr[1]=ip[1];
   
          PolygonAdd(ypol,ipnt,opq);

          if ((wq) && (q !=0)) PolygonAdd(ypol,qp,opq); 
         
        }
     }
  
     if (ypol->num[ypol->polnum-1]<3) {
        PolygonRemove(ypol);
     }
     
    } 

   if (xpol !=bpol) PolygonFree(xpol);
   xpol=ypol;
   ypol=PolygonMake(xpol->sze,PolygonXYbbox);
    
  }
 
  PolygonFree(ypol);
  
  if (xpol->pntnum==0) {
    PolygonFree(xpol);
    return NULL;
  } 
  return xpol;  
}














