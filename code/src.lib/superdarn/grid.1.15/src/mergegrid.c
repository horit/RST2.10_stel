/* mergegrid.c
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "rfile.h"
#include "griddata.h"

/*
 $Log: mergegrid.c,v $
 Revision 1.6  2004/10/08 20:09:11  barnes
 Changed griddata structure.

 Revision 1.5  2004/05/05 15:47:01  barnes
 Added missing header.

 Revision 1.4  2004/05/04 22:46:08  barnes
 Changed math header name.

 Revision 1.3  2004/05/04 18:29:58  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:18:23  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

void GridLinReg(int num,struct GridGVec **data,double *vpar,double *vper) {

  int k;
  double sx=0,cx=0,ysx=0,ycx=0,cxsx=0;
  double den;
  for (k=0;k<num;k++) {
    sx=sx+sin(data[k]->azm*PI/180.0)*sin(data[k]->azm*PI/180.0);
    cx=cx+cos(data[k]->azm*PI/180.0)*cos(data[k]->azm*PI/180.0);
    ysx=ysx+data[k]->vel.median*sin(data[k]->azm*PI/180.0);
    ycx=ycx+data[k]->vel.median*cos(data[k]->azm*PI/180.0);
    cxsx=cxsx+sin(data[k]->azm*PI/180.0)*cos(data[k]->azm*PI/180.0);
  }   
    
  
  den=sx*cx-cxsx*cxsx;
  if (den !=0) {
    *vpar=(sx*ycx-cxsx*ysx)/den;
    *vper=(cx*ysx-cxsx*ycx)/den;
  } else {
    *vpar=0;
    *vper=0;
  }

}


void GridMerge(struct GridData *mptr,struct GridData *ptr) {

  int i;
  int k;
  int *index=NULL;
  int *count=NULL;

  struct GridGVec **data=NULL;
  int data_count=0;
  int num=0;

  double vpar,vper;

  ptr->st_time=mptr->st_time;
  ptr->ed_time=mptr->ed_time;

  ptr->xtd=0;  
  ptr->vcnum=0;
  ptr->stnum=0;

  if (mptr->stnum==0) return;

  ptr->stnum=1;
   
  if (ptr->sdata !=NULL) ptr->sdata=realloc(ptr->sdata,
					    sizeof(struct GridSVec));
  else ptr->sdata=malloc(sizeof(struct GridSVec));
   
  ptr->sdata[0].st_id=255;
  ptr->sdata[0].freq0=0;
  ptr->sdata[0].major_revision=mptr->sdata[0].major_revision;
  ptr->sdata[0].minor_revision=mptr->sdata[0].minor_revision;
 
 /* need to verify that these are correct */

  ptr->sdata[0].prog_id=mptr->sdata[0].prog_id;
  ptr->sdata[0].noise.mean=mptr->sdata[0].noise.mean;
  ptr->sdata[0].noise.sd=mptr->sdata[0].noise.sd;

  ptr->sdata[0].gsct=mptr->sdata[0].gsct;

  ptr->sdata[0].vel.min=mptr->sdata[0].vel.min;
  ptr->sdata[0].vel.max=mptr->sdata[0].vel.max;
  ptr->sdata[0].pwr.min=mptr->sdata[0].pwr.min;
  ptr->sdata[0].pwr.max=mptr->sdata[0].pwr.max;
  ptr->sdata[0].wdt.min=mptr->sdata[0].wdt.min;
  ptr->sdata[0].wdt.max=mptr->sdata[0].wdt.max;
  
  if (ptr->data !=NULL) free(ptr->data);
  ptr->data=NULL;
  
  /* scan the grid file for unique cell numbers */
 
  for (i=0;i<mptr->vcnum;i++) {
   
    if (num==0) {
      index=malloc(sizeof(int));
      count=malloc(sizeof(int));
      index[0]=mptr->data[i].index;
      count[0]=1;
      num++;
    }  else {
      /* search to determine if this is a new cell */
      for (k=0;(index[k] !=mptr->data[i].index) && (k<num);k++);
      if (k==num) {
        /* new cell */
        index=realloc(index,sizeof(int)*(num+1));
        count=realloc(count,sizeof(int)*(num+1));
        index[num]=mptr->data[i].index;
        count[num]=1;
        num++;
      } else count[k]++; /* existing cell */
    }
  }
 

  for (k=0;k<num;k++) {
    if (count[k]<2) continue;
    data_count=0;         
    /* locate all the data points in the array */
   
    for (i=0;i<mptr->vcnum;i++) {
      
      if (mptr->data[i].index==index[k]) {
        /* got a point from a grid cell so add to the list */
        if (data==NULL) data=malloc(sizeof(struct gvec *));
        else data=realloc(data,sizeof(struct gvec *)*(data_count+1));
        data[data_count]=&mptr->data[i];
        data_count++;
      }
    }
    /* done the search and data is an array pointing to all the vectors */
   
    if (data==NULL) continue;
  
    /* add the new point to the merge grid */

    if (ptr->data==NULL) ptr->data=malloc(sizeof(struct GridGVec));
    else ptr->data=realloc(ptr->data,sizeof(struct GridGVec)*(ptr->vcnum+1));


    for (i=0;i<data_count;i++) data[i]->azm=90-data[i]->azm;
    GridLinReg(data_count,data,&vpar,&vper);

    if (vper !=0) {         
      ptr->data[ptr->vcnum].azm=atan(vpar/vper)*180.0/PI;
      if (vper<0) ptr->data[ptr->vcnum].azm+=180.0;
    } else ptr->data[ptr->vcnum].azm=0;

    ptr->data[ptr->vcnum].mlon=data[0]->mlon;
    ptr->data[ptr->vcnum].mlat=data[0]->mlat;
    ptr->data[ptr->vcnum].vel.median=sqrt(vpar*vpar+vper*vper);
    ptr->data[ptr->vcnum].vel.sd=0;
    ptr->data[ptr->vcnum].pwr.median=0;
    ptr->data[ptr->vcnum].pwr.sd=0;
    ptr->data[ptr->vcnum].wdt.median=0;
    ptr->data[ptr->vcnum].wdt.sd=0;
    ptr->data[ptr->vcnum].st_id=255;
    ptr->data[ptr->vcnum].chn=0;   
    ptr->data[ptr->vcnum].index=index[k];
    ptr->vcnum++;
    ptr->sdata[0].npnt++;
    free(data);
    data=NULL;
    data_count=0;


  }

  if (index !=NULL) free(index);
  if (count !=NULL) free(count);
 
}
     

















