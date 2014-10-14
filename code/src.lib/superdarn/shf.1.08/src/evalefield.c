/* evalefield.c
   ============= 
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
#include "evalvelocity.h"
#include "crdshft.h"

/*
 $Log: evalefield.c,v $
 Revision 1.5  2004/05/05 15:27:19  barnes
 Added missing header.

 Revision 1.4  2004/05/04 22:50:02  barnes
 Changed math header name.

 Revision 1.3  2004/05/04 18:21:12  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:52:03  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/


void CnvMapEvalEfield(int Lmax,double *coef,double *plm,
		      struct CnvGrid *vptr,
		      double latmin,struct CnvMapData *ptr) {

  int i;
  double *theta,*phi;
  double *etc,*epc;
  double lon,lat,tmp=0;

  theta=malloc(vptr->num*sizeof(double));
  phi=malloc(vptr->num*sizeof(double));
 
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
  CnvMapEvalComponent(Lmax,etc,plm,phi,vptr->num,vptr->ex);
  CnvMapEvalComponent(Lmax,epc,plm,phi,vptr->num,vptr->ey);
   
  free(phi);
  free(theta); 
  free(epc);
  free(etc);

}














