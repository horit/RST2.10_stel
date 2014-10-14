/* solvepotential.c
   ================= 
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
#include "rmath.h"
#include "rfile.h"
#include "cnvgrid.h"
#include "griddata.h"
#include "cnvmap.h"
#include "evallegendre.h"
#include "evalpotential.h"
#include "crdshft.h"

void CnvMapEvaPotential(int,int,double *,double *,double *,double *);

int CnvMapSolvePotential(struct CnvMapData *ptr,struct CnvGrid *pptr) {
  
  int i;
  double lon,lat,tmp=0; 
  double tlimit=PI;
  double alpha;
  double th;
  
  double *ph;
  double *x;
  double *plm;
   
 
  alpha=tlimit/((90.0-fabs(ptr->latmin))/180.0*PI);  

  x=malloc(sizeof(double)*pptr->num);
  ph=malloc(sizeof(double)*pptr->num);
 
  if ((x==NULL) || (ph==NULL)) return -1;

  for (i=0;i<pptr->num;i++) {
    lat=fabs(pptr->lat[i]);
    lon=pptr->lon[i];
    if ((ptr->lat_shft !=0) || (ptr->lon_shft !=0)) 
      CnvMapCrdShft(&lat,&lon,&tmp,ptr->lat_shft,ptr->lon_shft);
    th=alpha*(90.0-lat)/180.0*PI;
    ph[i]=lon/180.0*PI;
    x[i]=cos(th);
  }

  plm=malloc(sizeof(double)*(ptr->fit_order+1)*(ptr->fit_order+1)*pptr->num);

  CnvMapEvalLegendre(ptr->fit_order,x,pptr->num,plm);
  CnvMapEvalPotential(ptr->fit_order,pptr->num,ptr->coef,plm,ph,pptr->mag);
  free(plm);
  free(x);
  free(ph);
  return 0;
}











