/* fitmap.c
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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "rfile.h"
#include "aacgm.h"

#include "griddata.h"
#include "cnvgrid.h"
#include "cnvmap.h"

#include "shfvector.h"
#include "fitvector.h"

#include "evallegendre.h"
#include "evalpotential.h"
#include "crdshft.h"

/*
 $Log: fitmap.c,v $
 Revision 1.6  2004/10/12 22:49:37  barnes
 Changed grid data structure.

 Revision 1.5  2004/05/05 15:27:19  barnes
 Added missing header.

 Revision 1.4  2004/05/04 22:50:02  barnes
 Changed math header name.

 Revision 1.3  2004/05/04 18:21:12  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:52:03  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/15 23:27:23  barnes
 Initial revision

*/



int CnvMapFitMap(struct CnvMapData *map,struct GridData *grd) {
  double terr=0,merr=0;
  double mlat,mlon,tmp;
  double pot_max=-1e6;
  double pot_min=1e6;
  double vx,vy;
  double rms=0;
  struct CnvMapSHFVec *data;
  double *fitvel;
  double *plm;
  double *x;
  double *ph;
  double *mag;
  double lat,lon,lat_step,lon_step,th;
  double tlimit=PI,alpha,a,b;
  double var_max,var_min;
  int num=0,i=0,j,nlat,nlon,m,L,k,n,Lmax;
  int max_sub=0,min_sub=10000;
  int dnum;
  double asum=0;

  double vel_max=2000;
  double verr_min=100; /* used to be 50 for original map_potential */
  int ewt=1,mwt=1;

  fitvel=malloc(sizeof(struct CnvMapSHFVec)*(grd->vcnum+2*map->num_model));
  if (fitvel==NULL) return -1;
  data=malloc(sizeof(struct CnvMapSHFVec)*(grd->vcnum+2*map->num_model));
  if (data==NULL) return -1;

  if (map->coef !=NULL) free(map->coef);
  map->num_coef=CnvMapIndexLegendre(map->fit_order,map->fit_order)+2;
  map->coef=malloc(4*map->num_coef*sizeof(double));
     
  
  for (j=0;j<grd->vcnum;j++) {
    if (grd->data[j].st_id==-1) continue;
    mlat=fabs(grd->data[j].mlat);
    mlon=grd->data[j].mlon;
    tmp=grd->data[j].azm;
       
    if ((map->lat_shft !=0) || (map->lon_shft !=0)) 
    CnvMapCrdShft(&mlat,&mlon,&tmp,map->lat_shft,map->lon_shft);
       
    if (mlat < fabs(map->latmin)) continue;
    if (fabs(grd->data[j].vel.median)>vel_max) continue;
    data[num].lat=mlat;
    data[num].lon=mlon;
    data[num].vlos=grd->data[j].vel.median;
    data[num].verr=grd->data[j].vel.sd;
    if (data[num].verr<=verr_min) data[num].verr=verr_min;
    data[num].cos=-cos(tmp*PI/180)*map->hemisphere;
    data[num].sin=sin(tmp*PI/180);
       
    terr+=1/(data[num].verr*data[num].verr);
    num++;
  }   
  dnum=num; 
  if (num !=0) merr=sqrt(num/terr);
  else merr=verr_min;
  
  /* if the ewt==0 then substitute the averaged error */
  
  if (ewt==0) for (i=0;i<num;i++) data[i].verr=merr;

  /* if mwt==1 then adjust the error according to order */
  
  if (mwt==1) merr=sqrt( (map->fit_order/4.0)*(map->fit_order/4.0) )*merr;

  for (i=0;i<map->num_model;i++) {
    if (map->model[i].vel.median==1) continue; /* screen out boundary vecs */
    mlat=fabs(map->model[i].mlat);
    mlon=map->model[i].mlon;
    vx=map->model[i].vel.median*cos(map->model[i].azm*PI/180);
    vy=map->model[i].vel.median*sin(map->model[i].azm*PI/180);
  
    data[num].lat=mlat;
    data[num].lon=mlon;
    data[num].vlos=vx*map->hemisphere;
    data[num].verr=merr;
    data[num].cos=-1;
    data[num].sin=0;

   
    num++;
  }

  for (i=0;i<map->num_model;i++) {
    if (map->model[i].vel.median==1) continue; /* screen out boundary vecs */
    mlat=fabs(map->model[i].mlat);
    mlon=map->model[i].mlon;
    vx=map->model[i].vel.median*cos(map->model[i].azm*PI/180);
    vy=map->model[i].vel.median*sin(map->model[i].azm*PI/180);

    data[num].lat=mlat;
    data[num].lon=mlon;
    data[num].vlos=vy;
    data[num].verr=merr;
    data[num].cos=0;
    data[num].sin=1;

 

    num++;
  }

  /* okay now do boundary vecs */

  for (i=0;i<map->num_model;i++) {
    if (map->model[i].vel.median !=1) continue;
    mlat=fabs(map->model[i].mlat);
    mlon=map->model[i].mlon;
    tmp=map->model[i].azm;
     
    data[num].lat=mlat;
    data[num].lon=mlon;
    data[num].vlos=map->model[i].vel.median;
    data[num].verr=2*verr_min;
    data[num].cos=-cos(tmp*PI/180);
    data[num].sin=sin(tmp*PI/180);
   
    num++;
  }

  
   

  map->chi_sqr=CnvMapFitVector(num,data,map->coef,fitvel,map->fit_order,
			  fabs(map->latmin));

 

  /* calculate chi_sqr associated with the data values */

  for(i=0;i<dnum;i++) {
    asum+=((fitvel[i]-data[i].vlos)/data[i].verr)*
          ((fitvel[i]-data[i].vlos)/data[i].verr);
  }
  map->chi_sqr_dat=asum;
  
  rms=0;
  for (i=0;i<dnum;i++) {
     rms+=(fitvel[i]-data[i].vlos)*(fitvel[i]-data[i].vlos);
  }
  if (dnum > 0)
    map->rms_err=sqrt(rms/dnum);
  else map->rms_err=1e32;

  /* having done the fit we now need to calculate potential 
     limits and errors */

  lat_step=2.0;
  lon_step=5.0;
  n=0;
  

  Lmax=map->fit_order;  
  nlat=(int) (90.0+fabs(map->latmin))/lat_step;
  nlon=(int) (360.0/lon_step);
  alpha=tlimit/((90.0+fabs(map->latmin))/180.0*PI);  
   
  x=malloc(sizeof(double)*nlat*nlon);
  ph=malloc(sizeof(double)*nlat*nlon);
  mag=malloc(sizeof(double)*nlat*nlon); 

  if ((x==NULL) || (ph==NULL) || (mag==NULL)) return -1;
  for (j=0;j<nlon;j++) {
    for (i=0;i<nlat;i++) { 
       if (map->latmin>0) lat=i*lat_step+map->latmin;
       else lat=-i*lat_step+map->latmin;
       lat=fabs(lat);
       lon=j*lon_step;
       th=alpha*(90.0+lat)/180.0*PI;
       ph[n]=lon/180.0*PI;
       x[n]=cos(th);
       n++;
    }
  }
  plm=malloc(sizeof(double)*(Lmax+1)*(Lmax+1)*n);

  CnvMapEvalLegendre(Lmax,x,n,plm);
  CnvMapEvalPotential(Lmax,n,map->coef,plm,ph,mag);
 
  for (i=0;i<n;i++) {
    if (mag[i]>pot_max) {
      pot_max=mag[i];
      max_sub=i;
    }
    if (mag[i]<pot_min) {
      pot_min=mag[i];
      min_sub=i;
    }
  }

  /* now find the errors */
  var_max=0;
  var_min=0;
  for (m=0;m<=Lmax;m++) {
    for(L=m;L<=Lmax;L++) {
      k=CnvMapIndexLegendre(L,m); 
      if (m==0) {
        a=map->coef[k*4+3];
        var_max+=(a*PLM(L,0,max_sub))*(a*PLM(L,0,max_sub));
        var_min+=(a*PLM(L,0,min_sub))*(a*PLM(L,0,min_sub));
      } else {
        a=map->coef[k*4+3];
        b=map->coef[(k+1)*4+3];
        var_max+=(a*PLM(L,m,max_sub)*cos(m*ph[max_sub]))*
		 (a*PLM(L,m,max_sub)*cos(m*ph[max_sub]))+
		 (b*PLM(L,m,max_sub)*sin(m*ph[max_sub]))*
		 (b*PLM(L,m,max_sub)*sin(m*ph[max_sub]));
        var_min+=(a*PLM(L,m,min_sub)*cos(m*ph[min_sub]))*
 		 (a*PLM(L,m,min_sub)*cos(m*ph[min_sub]))+
		 (b*PLM(L,m,min_sub)*sin(m*ph[min_sub]))*
		 (b*PLM(L,m,min_sub)*sin(m*ph[min_sub]));
      }
    }
  }
  map->pot_min=pot_min;
  map->pot_min_err=sqrt(var_min);
  map->pot_max=pot_max;
  map->pot_max_err=sqrt(var_max);

  map->pot_drop=pot_max-pot_min;
  map->pot_drop_err=sqrt(var_max+var_min);

  free(mag);
  free(plm);
  free(x);
  free(ph);
  free(fitvel);
  free(data);
  return 0;
}















