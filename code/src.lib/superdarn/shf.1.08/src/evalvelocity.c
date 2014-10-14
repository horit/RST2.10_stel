/* evalvelocity.c
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvgrid.h"
#include "evallegendre.h"
#include "crdshft.h"
#include "shfconst.h"


double *CnvMapEvalThetaCoef(int Lmax,double *coef,double *theta,int n,
		            double latmin) {

  int k1,k2,k3,k4,kmax;
  int i,m,L;
  double alpha=1;
  double tmax;
  double *ecoef;

  tmax=(90.0-latmin)*PI/180.0;
  
  for (i=0;i<n;i++) {
     theta[i]=theta[i]*PI/tmax;
  }

  alpha=PI/tmax;
  kmax=CnvMapIndexLegendre(Lmax,Lmax);
  ecoef=malloc(sizeof(double)*(kmax+2)*n);
  for (i=0;i<(kmax+2)*n;i++) ecoef[i]=0.0;

  for (m=0;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) { 
      k3=CnvMapIndexLegendre(L,m);
      k4=CnvMapIndexLegendre(L,m);
      if (k3>=0) {
        for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k4*n+i]=ecoef[k4*n+i]-
	   a(k3)*alpha*L*cos(theta[i])/sin(theta[i])/Radial_Dist;
      }
      if (L < Lmax) k1=CnvMapIndexLegendre(L+1,m);
      else k1=-1;
      k2=CnvMapIndexLegendre(L,m);
      if (k1 >=0) {
        for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k2*n+i]=ecoef[k2*n+i]+
	   a(k1)*alpha*(L+1+m)/sin(theta[i])/Radial_Dist;
      }
      if (m > 0) {
        if (k3 >=0) k3=k3+1;
        k4=k4+1;
        if (k1 >=0) k1=k1+1;
        k2=k2+1;
	if (k3 >=0) {
          for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k4*n+i]=ecoef[k4*n+i]-
	     a(k3)*alpha*L*cos(theta[i])/sin(theta[i])/Radial_Dist;
        }
        if (k1 >=0) {
          for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k2*n+i]=ecoef[k2*n+i]+
             a(k1)*alpha*(L+1+m)/sin(theta[i])/Radial_Dist;
	}
      }
    }
  }
  return ecoef;
}

     
double *CnvMapEvalPhiCoef(int Lmax,double *coef,double *theta,int n) { 
  int kmax,i,m,L;
  int k3,k4;
  double *ecoef;


  kmax=CnvMapIndexLegendre(Lmax,Lmax);
  ecoef=malloc(sizeof(double)*(kmax+2)*n);
  for (i=0;i<(kmax+2)*n;i++) ecoef[i]=0.0;

  for (m=1;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) {
      k3=CnvMapIndexLegendre(L,m);
      k4=CnvMapIndexLegendre(L,m);
      if (k3 >=0) {
	for (i=0;i<n;i++) if (theta[i] !=0.0) {
          ecoef[k4*n+i]=ecoef[k4*n+i]-a(k3+1)*m/sin(theta[i])/Radial_Dist;
          ecoef[(k4+1)*n+i]=ecoef[(k4+1)*n+i]+a(k3)*m/sin(theta[i])/
                            Radial_Dist;
	}
      }
    }
  }
  return ecoef;
}      

void CnvMapEvalComponent(int Lmax,double *ecoef,double *plm,
                      double *phi,int n,double *ecomp) {

  int i,m,L,k; 
  for (i=0;i<n;i++) ecomp[i]=0.0;
  for (m=0;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) {
       k=CnvMapIndexLegendre(L,m);
       for (i=0;i<n;i++) {
         if (m==0) ecomp[i]=ecomp[i]+ecoef[k*n+i]*PLM(L,m,i);
         else ecomp[i]=ecomp[i]+ecoef[k*n+i]*PLM(L,m,i)*cos(m*phi[i])+
              ecoef[(k+1)*n+i]*PLM(L,m,i)*sin(m*phi[i]); 
       }
    }
  }
}



void CnvMapEvalVelocity(int Lmax,double *coef,double *plm,
			struct CnvGrid *vptr,
		        double latmin,struct CnvMapData *ptr) {

  int i;
  double *theta,*phi;
  double *etc,*epc;
  double bpolar;
  double bmag=-0.5e-4;
  double *ex; 
  double *ey;
  double lon,lat,tmp=0;

  double vx,vy;  

  theta=malloc(vptr->num*sizeof(double));
  phi=malloc(vptr->num*sizeof(double));
  ex=malloc(vptr->num*sizeof(double));
  ey=malloc(vptr->num*sizeof(double));

  if ((theta==NULL) || (phi==NULL)) return;

  for (i=0;i<vptr->num;i++) {
     lat=fabs(vptr->lat[i]);
     lon=vptr->lon[i];
     if ((ptr->lat_shft !=0) || (ptr->lon_shft !=0)) 
       CnvMapCrdShft(&lat,&lon,&tmp,ptr->lat_shft,ptr->lon_shft);
     theta[i]=(90.0-lat)*PI/180.0;
     phi[i]=lon*PI/180.0;
  }
  epc=CnvMapEvalPhiCoef(Lmax,coef,theta,vptr->num);
  etc=CnvMapEvalThetaCoef(Lmax,coef,theta,vptr->num,fabs(latmin));
  CnvMapEvalComponent(Lmax,etc,plm,phi,vptr->num,ex);
  CnvMapEvalComponent(Lmax,epc,plm,phi,vptr->num,ey);
  
  for (i=0;i<vptr->num;i++) {

    if (ptr->hemisphere == -1) bpolar = BNorth;
    else bpolar = BSouth;

    bmag = bpolar*(1.0 - 3.0 * Altitude/Re)*
           sqrt(3.0*(cos(theta[i])*cos(theta[i])) + 1.0)/2.0;


    vx=ey[i]/bmag;
    vy=-ex[i]/bmag;

  
    vptr->mag[i]=sqrt(vx*vx+vy*vy);
    vptr->azm[i]=atan2(vy,-vx)*180.0/PI;
    
    if (ptr->hemisphere==-1) vptr->azm[i]=atan2(vy,vx)*180.0/PI;
    
  }
  free(ex);
  free(ey);
  free(phi);
  free(theta); 
  free(epc);
  free(etc);

}














