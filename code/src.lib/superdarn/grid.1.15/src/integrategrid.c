/* integrategrid.c
   ===============
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
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"

/*
 $Log: integrategrid.c,v $
 Revision 1.5  2004/10/08 20:09:11  barnes
 Changed griddata structure.

 Revision 1.4  2004/06/16 17:06:44  barnes
 Added missing header.

 Revision 1.3  2004/05/05 15:47:01  barnes
 Added missing header.

 Revision 1.2  2004/05/04 18:29:58  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

void GridIntegrate(struct GridData *a,struct GridData *b,double *err) {


  int i=0,j=0,k=0,l=0,m=0;
  int st_id,index;
  int pnt=0;
  double v_e,p_e,w_e;

  if (b==NULL) return;
  GridSort(b);

  a->st_time=b->st_time;
  a->ed_time=b->ed_time;
  a->xtd=b->xtd;
  a->stnum=b->stnum;
  a->vcnum=0;

 
   
  if (b->stnum>0) {
    if (a->sdata==NULL) a->sdata=malloc(sizeof(struct GridSVec)*b->stnum);
    else a->sdata=realloc(a->sdata,sizeof(struct GridSVec)*b->stnum);
  } else if (a->sdata !=NULL) {
    free(a->sdata);
    a->sdata=NULL;
  }

  /* allocate enough space in the output grid to store
     every possible vector in the input */

  if (b->vcnum>0) { 
  if (a->data==NULL) a->data=malloc(sizeof(struct GridGVec)*b->vcnum);
  else a->data=realloc(a->data,sizeof(struct GridGVec)*b->vcnum);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->stnum !=0) memcpy(a->sdata,b->sdata,
                             sizeof(struct GridSVec)*b->stnum);
  if (b->vcnum ==0) return;

  /* clear the array */
  memset(a->data,0,sizeof(struct GridGVec)*b->vcnum);
    
  /* average together vectors from the same radar that lie
     in the same cell */

 
  i=0;
  while (i<b->vcnum) {
    st_id=b->data[i].st_id;
    j=i+1;
    while (j<b->vcnum) {
      if (b->data[j].st_id !=st_id) break;
      j++;
    }
    
    /* okay i and j mark the start and end of a station */    
    k=i;
    while (k<j) {
      index=b->data[k].index;
      l=k+1;
      while (l<j) {
        if (b->data[l].index !=index) break;      
        l++;
      }
      for (m=k;m<l;m++) { /* add vectors to the output */
        v_e=b->data[m].vel.sd;
        p_e=b->data[m].pwr.sd;
        w_e=b->data[m].wdt.sd;

        if (v_e<err[0]) v_e=err[0];
        if (p_e<err[1]) p_e=err[1];
        if (w_e<err[2]) w_e=err[2];

        a->data[pnt].azm+=b->data[m].azm;
        a->data[pnt].vel.median+=b->data[m].vel.median*1/(v_e*v_e);
        a->data[pnt].pwr.median+=b->data[m].pwr.median*1/(p_e*p_e);
        a->data[pnt].wdt.median+=b->data[m].wdt.median*1/(w_e*w_e);
        
        a->data[pnt].vel.sd+=1/(v_e*v_e);
        a->data[pnt].pwr.sd+=1/(p_e*p_e);
        a->data[pnt].wdt.sd+=1/(w_e*w_e);
      }

      a->data[pnt].azm=a->data[pnt].azm/(l-k);
      
      a->data[pnt].vel.median=a->data[pnt].vel.median/a->data[pnt].vel.sd;
      a->data[pnt].wdt.median=a->data[pnt].wdt.median/a->data[pnt].wdt.sd;
      a->data[pnt].pwr.median=a->data[pnt].pwr.median/a->data[pnt].pwr.sd;
      

      a->data[pnt].vel.sd=1/sqrt(a->data[pnt].vel.sd);
      a->data[pnt].wdt.sd=1/sqrt(a->data[pnt].wdt.sd);
      a->data[pnt].pwr.sd=1/sqrt(a->data[pnt].pwr.sd);
      a->data[pnt].mlat=b->data[k].mlat;
      a->data[pnt].mlon=b->data[k].mlon;

      a->data[pnt].st_id=st_id;
      a->data[pnt].index=index;
      pnt++; 
      
      k=l;
    }
    i=j;
  }
  a->vcnum=pnt;
  a->data=realloc(a->data,sizeof(struct GridGVec)*b->vcnum);


}

 
