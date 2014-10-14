/* ace.c
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
#include "rtypes.h"
#include "rtime.h"
#include "geop.h"
#include "cdf.h"
#include "rcdf.h"
#include "istp.h"

/* 
 $Log: ace.c,v $
 Revision 1.7  2004/08/29 21:37:38  barnes
 Fixes for Mac OS X.

 Revision 1.6  2004/05/10 22:04:41  barnes
 Removed redundant code.

 Revision 1.5  2003/03/09 17:34:24  barnes
 Changed CDF header name.

 Revision 1.4  2003/02/26 16:09:24  barnes
 Modification to deal with base libraries.

 Revision 1.3  2002/11/25 21:04:42  barnes
 Added support for the h0 data files.

 Revision 1.2  2001/06/27 21:09:30  barnes
 Added license tag

 Revision 1.1  2000/07/31 17:36:29  barnes
 Initial revision

 */

extern struct RCDFData data[10];

int acemfi_imf(CDFid id,struct imfdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","BGSEc",NULL};
  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;
  double gse[3];
  double gsm[3];

  strcpy(ptr->sat,"ac");
  strcpy(ptr->ins,"mfi");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;

  if (rmax==0) return -1;
  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->BGSMc==NULL) ptr->BGSMc=malloc(3*sizeof(float)*max);
  else ptr->BGSMc=realloc(ptr->BGSMc,3*sizeof(float)*max);

  if (ptr->BGSEc==NULL) ptr->BGSEc=malloc(3*sizeof(float)*max);
  else ptr->BGSEc=realloc(ptr->BGSEc,3*sizeof(float)*max);

  for (i=0;i<rmax;i++) {
    status=RCDFReadR(id,i,varlist,data);
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;
       ptr->time[cnt]=rtime;
       fptr=(float *) data[1].data;
          
       ptr->BGSEc[cnt*3]=fptr[0];
       ptr->BGSEc[cnt*3+1]=fptr[1];
       ptr->BGSEc[cnt*3+2]=fptr[2];
       gse[0]=fptr[0];
       gse[1]=fptr[1];
       gse[2]=fptr[2];

       GEOPGseGsm(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0),gse,gsm);

       ptr->BGSMc[cnt*3]=gsm[0];
       ptr->BGSMc[cnt*3+1]=gsm[1];
       ptr->BGSMc[cnt*3+2]=gsm[2];
       cnt++;
    }
  }
  ptr->cnt=cnt;
  return 0;
}


