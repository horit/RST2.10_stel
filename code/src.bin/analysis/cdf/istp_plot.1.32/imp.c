/* imp.c
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

/* 
 $Log: imp.c,v $
 Revision 1.9  2004/08/29 21:41:42  barnes
 Fixes for Mac OS X.

 Revision 1.8  2004/05/04 19:16:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.7  2004/04/27 18:24:08  barnes
 Audit - Enforced warning set to "all".

 Revision 1.6  2003/08/06 22:19:02  barnes
 Changed library calls.

 Revision 1.5  2003/03/09 17:31:05  barnes
 Changed CDF header name.

 Revision 1.4  2003/02/27 23:47:46  barnes
 Modification to use the base libraries.

 Revision 1.3  2001/06/27 20:57:09  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:12:10  barnes
 Added Author Name

 Revision 1.1  2000/06/24 20:08:24  barnes
 Initial revision

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

extern struct RCDFData data[10];


int impmag_pos(CDFid id,struct posdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","SC_pos_sm","SC_pos_se",0,NULL};
  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;

  strcpy(ptr->sat,"i8");
  strcpy(ptr->ins,"mag");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;   

  if (max==0) return -1;
  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->PGSM==NULL) ptr->PGSM=malloc(3*sizeof(float)*max);
  else ptr->PGSM=realloc(ptr->PGSM,3*sizeof(float)*max);

  if (ptr->PGSE==NULL) ptr->PGSE=malloc(3*sizeof(float)*max);
  else ptr->PGSE=realloc(ptr->PGSE,3*sizeof(float)*max);

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
       ptr->PGSM[cnt*3]=fptr[0]/6370.0;
       ptr->PGSM[cnt*3+1]=fptr[1]/6370.0;
       ptr->PGSM[cnt*3+2]=fptr[2]/6370.0;
       fptr=(float *) data[2].data;
       ptr->PGSE[cnt*3]=fptr[0]/6370.0;
       ptr->PGSE[cnt*3+1]=fptr[1]/6370.0;
       ptr->PGSE[cnt*3+2]=fptr[1]/6370.0;
       cnt++;
    }
  }
  ptr->cnt=cnt;
  return 0;
}


int imppla_pos(CDFid id,struct posdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","SC_pos_gsm","SC_pos_gse",0,NULL};
  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;

  strcpy(ptr->sat,"i8");
  strcpy(ptr->ins,"pla");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecZ(id,varlist[0]);
  max=cnt+rmax;  
 
  if (max==0) return -1;
  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->PGSM==NULL) ptr->PGSM=malloc(3*sizeof(float)*max);
  else ptr->PGSM=realloc(ptr->PGSM,3*sizeof(float)*max);

  if (ptr->PGSE==NULL) ptr->PGSE=malloc(3*sizeof(float)*max);
  else ptr->PGSE=realloc(ptr->PGSE,3*sizeof(float)*max);

  for (i=0;i<rmax;i++) {
    status=RCDFReadZ(id,i,varlist,data);
    
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 

       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;
       ptr->time[cnt]=rtime;

     
       fptr=(float *) data[1].data;
       if (fptr[0]>999) {
         ptr->PGSM[cnt*3]=1e31;
         ptr->PGSM[cnt*3+1]=1e31;
         ptr->PGSM[cnt*3+2]=1e31;
         ptr->PGSE[cnt*3]=1e31;
         ptr->PGSE[cnt*3+1]=1e31;
         ptr->PGSE[cnt*3+2]=1e31;
         cnt++;
         continue;
       }

       ptr->PGSM[cnt*3]=fptr[0];
       ptr->PGSM[cnt*3+1]=fptr[1];
       ptr->PGSM[cnt*3+2]=fptr[2];
       fptr=(float *) data[2].data;
       ptr->PGSE[cnt*3]=fptr[0];
       ptr->PGSE[cnt*3+1]=fptr[1];
       ptr->PGSE[cnt*3+2]=fptr[1];
       cnt++;
    }
  }
  ptr->cnt=cnt;
  return 0;
}

int imppla_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","V_GSE","Np",NULL};
  int max;
  int rmax;
  int status;

  double gse[3];
  double gsm[3];

  double *dptr;
  float *fptr;
  float Mp=1.6e-27;
  float Vx;
  int pcnt=0;

  cnt=ptr->cnt;
  rmax=RCDFMaxRecZ(id,varlist[0]);
  max=rmax+cnt;   

  if (max==0) return -1;
  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->VGSM==NULL) ptr->VGSM=malloc(3*sizeof(float)*max);
  else ptr->VGSM=realloc(ptr->VGSM,3*sizeof(float)*max);

  if (ptr->VGSE==NULL) ptr->VGSE=malloc(3*sizeof(float)*max);
  else ptr->VGSE=realloc(ptr->VGSE,3*sizeof(float)*max);

  if (ptr->vth==NULL) ptr->vth=malloc(sizeof(float)*max);
  else ptr->vth=realloc(ptr->vth,sizeof(float)*max);

  if (ptr->den==NULL) ptr->den=malloc(sizeof(float)*max);
  else ptr->den=realloc(ptr->den,sizeof(float)*max);

 if (ptr->pre==NULL) ptr->pre=malloc(sizeof(float)*max);
  else ptr->pre=realloc(ptr->pre,sizeof(float)*max);


  for (i=0;i<rmax;i++) {
  
    status=RCDFReadZ(id,i,varlist,data);
    
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms);

       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;
       ptr->time[cnt]=rtime;
 
       fptr=(float *) data[1].data;
   
       if (fptr[0]>9999) { 
           ptr->VGSE[cnt*3]=1e31;
           ptr->VGSE[cnt*3+1]=1e31;
           ptr->VGSE[cnt*3+2]=1e31;
           ptr->VGSM[cnt*3]=1e31;
           ptr->VGSM[cnt*3+1]=1e31;
           ptr->VGSM[cnt*3+2]=1e31;
           ptr->vth[cnt]=1e31;
           ptr->den[cnt]=1e31;
           ptr->pre[cnt]=1e31;
           cnt++;
           continue;
       }
      
       ptr->VGSE[cnt*3]=fptr[0];
       ptr->VGSE[cnt*3+1]=fptr[1];
       ptr->VGSE[cnt*3+2]=fptr[2];

       gse[0]=fptr[0];
       gse[1]=fptr[1];
       gse[2]=fptr[2];

       GEOPGseGsm(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0),gse,gsm);
       ptr->VGSM[cnt*3]=gsm[0];
       ptr->VGSM[cnt*3+1]=gsm[1];
       ptr->VGSM[cnt*3+2]=gsm[2];

       ptr->vth[cnt]=0;
       fptr=(float *) data[2].data;
       ptr->den[cnt]=fptr[0];
       
       Vx=ptr->VGSM[3*cnt];
       if ((ptr->den[cnt]>0) && (Vx > -10000)) {
          ptr->pre[cnt]=(ptr->den[cnt]*Mp*1e6)*
                     fabs(Vx*1e3)*fabs(Vx*1e3)*1e9;
          pcnt++;
       } else ptr->pre[cnt]=ptr->den[cnt];
       
       cnt++;
    }
  }
  if (pcnt==0) memset(ptr->pre,0,sizeof(float)*cnt);


  ptr->cnt=cnt;
  return 0;
}



int impmag_imf(CDFid id,struct imfdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","B_GSM_c","B_GSE_c",NULL};
  int max=0;
  int rmax;
  int status;

  double *dptr;
  float *fptr;

  strcpy(ptr->sat,"i8");
  strcpy(ptr->ins,"mag");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=max+rmax;

  if (max==0) return -1;
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
       ptr->BGSMc[cnt*3]=fptr[0];
       ptr->BGSMc[cnt*3+1]=fptr[1];
       ptr->BGSMc[cnt*3+2]=fptr[2];
       fptr=(float *) data[2].data;
       ptr->BGSEc[cnt*3]=fptr[0];
       ptr->BGSEc[cnt*3+1]=fptr[1];
       ptr->BGSEc[cnt*3+2]=fptr[2];
       cnt++;
    }
  }
  ptr->cnt=cnt;
  return 0;
}


