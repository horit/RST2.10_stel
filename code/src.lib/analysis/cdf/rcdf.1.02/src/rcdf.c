/* rcdf.c
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

/* 
 $Log: rcdf.c,v $
 Revision 1.2  2004/08/29 21:30:59  barnes
 Added missing header.

 Revision 1.1  2003/08/07 13:45:16  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cdf.h"
#include "rcdf.h"

int RCDFSize(int type) {
  switch (type) {
  case CDF_BYTE:
  case CDF_CHAR:
  case CDF_INT1:
  case CDF_UCHAR:
  case CDF_UINT1:
    return 1;
  case CDF_INT2:
  case CDF_UINT2:
    return 2;
  case CDF_INT4:
  case CDF_UINT4:
  case CDF_REAL4:
  case CDF_FLOAT:
    return 4;
  default:
    return 8;
  }
} 

int RCDFMajority(CDFid id) {
  CDFstatus status;
  int majority;
  status=CDFlib(SELECT_,CDF_,id,GET_,CDF_MAJORITY_,&majority,NULL_);
  if (status !=CDF_OK) return -1;
  return majority;
}


int RCDFVarNumZ(CDFid id) {
  CDFstatus status;
  int varnum;
  status=CDFlib(SELECT_,CDF_,id,GET_,CDF_NUMzVARS_,&varnum,NULL_);
  if (status !=CDF_OK) return -1;
  return varnum;
}

int RCDFVarNumR(CDFid id) {
  CDFstatus status;
  int varnum;
  status=CDFlib(SELECT_,CDF_,id,GET_,CDF_NUMrVARS_,&varnum,NULL_);
  if (status !=CDF_OK) return -1;
  return varnum;
}

int RCDFMaxRecZ(CDFid id,char *varname) {
  CDFstatus status;
  int maxrec;
  status=CDFlib(SELECT_,CDF_,id,SELECT_,zVAR_NAME_,varname,NULL_);
  if (status !=CDF_OK) return -1;
  status=CDFlib(GET_,zVAR_MAXREC_,&maxrec,NULL_);
  if (status !=CDF_OK) return -1;
  return maxrec;
}

int RCDFMaxRecR(CDFid id,char *varname) {
  CDFstatus status;
  int maxrec;
  status=CDFlib(SELECT_,CDF_,id,SELECT_,rVAR_NAME_,varname,NULL_);
  if (status !=CDF_OK) return -1;
  status=CDFlib(GET_,rVAR_MAXREC_,&maxrec,NULL_);
  if (status !=CDF_OK) return -1;
  return maxrec;
}



int RCDFReadZ(CDFid id,int recno,char **varname,struct RCDFData *ptr) {

  CDFstatus status;  
  int i,j,cnt=0;

  int *var;
  int *offset;
  int *size;
  void **buffer;

  int total_size=0;
  void *data;

  status=CDFlib(SELECT_,CDF_,id,NULL_);
  if (status !=CDF_OK) return -1;

  for (i=0;varname[i] !=NULL;i++) cnt++;
  var=malloc(sizeof(int)*cnt);
  offset=malloc(sizeof(int)*cnt);
  size=malloc(sizeof(int)*cnt);
  buffer=malloc(sizeof(void *)*cnt);

  cnt=0; 
  for (i=0;varname[i] !=NULL;i++) {     
    
    /* get variable number */

    ptr[i].status=CDFlib(GET_,zVAR_NUMBER_,varname[i],&ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  
    /* select it */

    ptr[i].status=CDFlib(SELECT_,zVAR_,ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

    /* find its proper name */

    ptr[i].status=CDFlib(GET_,zVAR_NAME_,ptr[i].name,NULL_);
    if (ptr[i].status !=CDF_OK) continue;


    /* find its type */

    ptr[i].status=CDFlib(GET_,zVAR_DATATYPE_,&ptr[i].type,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  

    /* find number of dimensions */

    ptr[i].status=CDFlib(GET_,zVAR_NUMDIMS_,&ptr[i].numdim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
   
    /* find extent */

    ptr[i].status=CDFlib(GET_,zVAR_DIMSIZES_,&ptr[i].dim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
 
     ptr[i].status=CDFlib(GET_,zVAR_DIMVARYS_,&ptr[i].var,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
 
 
    ptr[i].size=RCDFSize(ptr[i].type);

    if (ptr[i].numdim>0) for (j=0;j<ptr[i].numdim;j++) 
      if (ptr[i].var[j]==VARY) ptr[i].size=ptr[i].size*ptr[i].dim[j];

    
    if (ptr[i].data==NULL) ptr[i].data=malloc(ptr[i].size);
    else ptr[i].data=realloc(ptr[i].data,ptr[i].size);
    
    if (ptr[i].data==NULL) continue;
    
    var[cnt]=ptr[i].num;
    buffer[cnt]=ptr[i].data;
    size[cnt]=ptr[i].size;
    offset[cnt]=total_size;
    total_size+=ptr[i].size;
    cnt++;
  }  

  
  data=malloc(total_size);
  if (data==NULL) return -1;

  status=CDFlib(SELECT_,zVARs_RECNUMBER_,recno,NULL_);
  if (status !=CDF_OK) return -1;
    
  status=CDFlib(GET_,zVARs_RECDATA_,cnt,var,data,NULL_);

  if (status==CDF_OK) 
    for (i=0;i<cnt;i++) memcpy(buffer[i],data+offset[i],size[i]);  
  free(data);   
  free(var);
  free(buffer);
  free(offset);
  free(size); 
  if (status !=CDF_OK) return -1;
  return 0;
}

int RCDFReadR(CDFid id,int recno,char **varname,struct RCDFData *ptr) {

  CDFstatus status;  
  int i,j,cnt=0;

  int *var;
  int *offset;
  int *size;
  void **buffer;

  int total_size=0;
  void *data;


  status=CDFlib(SELECT_,CDF_,id,NULL_);
  if (status !=CDF_OK) return -1;

  for (i=0;varname[i] !=NULL;i++) cnt++;
  var=malloc(sizeof(int)*cnt);
  offset=malloc(sizeof(int)*cnt);
  size=malloc(sizeof(int)*cnt);
  buffer=malloc(sizeof(void *)*cnt);

  cnt=0; 


  for (i=0;varname[i] !=NULL;i++) {     
    
    /* get variable number */

    ptr[i].status=CDFlib(GET_,rVAR_NUMBER_,varname[i],&ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  
    /* select it */

    ptr[i].status=CDFlib(SELECT_,rVAR_,ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

    /* find its proper name */

    ptr[i].status=CDFlib(GET_,rVAR_NAME_,ptr[i].name,NULL_);
    if (ptr[i].status !=CDF_OK) continue;


    /* find its type */

    ptr[i].status=CDFlib(GET_,rVAR_DATATYPE_,&ptr[i].type,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  

    /* find number of dimensions */

    ptr[i].status=CDFlib(GET_,rVARs_NUMDIMS_,&ptr[i].numdim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
   
    /* find extent */

    ptr[i].status=CDFlib(GET_,rVARs_DIMSIZES_,&ptr[i].dim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
  
 
    ptr[i].status=CDFlib(GET_,rVAR_DIMVARYS_,&ptr[i].var,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
 

    ptr[i].size=RCDFSize(ptr[i].type);

    if (ptr[i].numdim>0) for (j=0;j<ptr[i].numdim;j++) 
      if (ptr[i].var[j]==VARY) ptr[i].size=ptr[i].size*ptr[i].dim[j];


    
    if (ptr[i].data==NULL) ptr[i].data=malloc(ptr[i].size);
    else ptr[i].data=realloc(ptr[i].data,ptr[i].size);
    
    if (ptr[i].data==NULL) continue;    
    var[cnt]=ptr[i].num;
    buffer[cnt]=ptr[i].data;
    size[cnt]=ptr[i].size;
    offset[cnt]=total_size;
    total_size+=ptr[i].size;
    cnt++;
  }  

  
  data=malloc(total_size);
  if (data==NULL) return -1;

  status=CDFlib(SELECT_,rVARs_RECNUMBER_,recno,NULL_);
  if (status !=CDF_OK) return -1;
    
  status=CDFlib(GET_,rVARs_RECDATA_,cnt,var,data,NULL_);

 
  if (status==CDF_OK) 
    for (i=0;i<cnt;i++) memcpy(buffer[i],data+offset[i],size[i]);  
  free(data);   
  free(var);
  free(buffer);
  free(offset);
  free(size); 
  if (status !=CDF_OK) return -1;
  return 0;
}







