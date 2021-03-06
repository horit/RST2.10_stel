/* avggrid.c
   =========
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
 $Log: avggrid.c,v $
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

int GridLocateCell(int npnt,struct GridGVec *ptr,int index) {
  int i;
  for (i=0;i<npnt && (ptr[i].index !=index);i++);
  return i;
}

void GridAverage(struct GridData *mptr,struct GridData *ptr,int flg) {

  int i;
  int k;
 

  ptr->st_time=mptr->st_time;
  ptr->ed_time=mptr->ed_time;
  ptr->xtd=mptr->xtd;  
  ptr->vcnum=0;
  ptr->stnum=1;
  
  if (ptr->sdata !=NULL) ptr->sdata=realloc(ptr->sdata,
					    sizeof(struct GridSVec));
  else ptr->sdata=malloc(sizeof(struct GridSVec));
   
  ptr->sdata[0].st_id=0;
  ptr->sdata[0].chn=0;
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


  if (ptr->data !=NULL) {
    free(ptr->data);
    ptr->data=NULL;
  }

  
  for (i=0;i<mptr->vcnum;i++) {

      k=GridLocateCell(ptr->vcnum,ptr->data,mptr->data[i].index);
      if (k==ptr->vcnum) {
        /* new cell */
        ptr->vcnum++;
        if (ptr->data==NULL) ptr->data=malloc(sizeof(struct GridGVec));
        else ptr->data=realloc(ptr->data,sizeof(struct GridGVec)*ptr->vcnum);

        ptr->data[k].azm=mptr->data[i].azm;
        ptr->data[k].vel.median=mptr->data[i].vel.median;
        ptr->data[k].vel.sd=mptr->data[i].vel.sd;
        ptr->data[k].pwr.median=mptr->data[i].pwr.median;
        ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
        ptr->data[k].wdt.median=mptr->data[i].wdt.median;
        ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
        ptr->data[k].mlon=mptr->data[i].mlon;
        ptr->data[k].mlat=mptr->data[i].mlat;
        ptr->data[k].index=mptr->data[i].index;      
     
        ptr->data[k].st_id=1;
        ptr->data[k].chn=0;
        if (flg !=0) {
          ptr->data[k].st_id=mptr->data[i].st_id;
          ptr->data[k].chn=mptr->data[i].chn;
	}  
      } else { 
        if (flg==0) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm+=mptr->data[i].azm;


                ptr->data[k].vel.median+=mptr->data[i].vel.median;
                ptr->data[k].vel.sd+=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median+=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd+=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median+=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd+=mptr->data[i].wdt.sd;

                ptr->data[k].st_id++;
              } else if ((flg==1) && 
                       (mptr->data[i].pwr.median>ptr->data[k].pwr.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
                ptr->data[k].st_id=mptr->data[i].st_id;
	      } else if ((flg==2) && 
                       (mptr->data[i].vel.median>ptr->data[k].vel.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==3) && 
                       (mptr->data[i].wdt.median>ptr->data[k].wdt.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==4) && 
                       (mptr->data[i].pwr.median<ptr->data[k].pwr.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
	      } else if ((flg==5) && 
			(mptr->data[i].vel.median<ptr->data[k].vel.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==6) && 
                       (mptr->data[i].wdt.median<ptr->data[k].wdt.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              }
	    }
        
  }
  if (flg==0) {
    for (i=0;i<ptr->vcnum;i++) {
      ptr->data[i].azm=ptr->data[i].azm/ptr->data[i].st_id;
      ptr->data[i].vel.median=ptr->data[i].vel.median/ptr->data[i].st_id;
      ptr->data[i].vel.sd=ptr->data[i].vel.sd/ptr->data[i].st_id;

      ptr->data[i].pwr.median=ptr->data[i].pwr.median/ptr->data[i].st_id;
      ptr->data[i].pwr.sd=ptr->data[i].pwr.sd/ptr->data[i].st_id;
      ptr->data[i].wdt.median=ptr->data[i].wdt.median/ptr->data[i].st_id;
      ptr->data[i].wdt.sd=ptr->data[i].wdt.sd/ptr->data[i].st_id;
    }
  }
  ptr->sdata[0].npnt=ptr->vcnum;
  return;
}

















