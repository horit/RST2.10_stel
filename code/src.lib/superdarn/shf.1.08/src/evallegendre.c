/* evallegendre.c
   ============== 
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


#include <math.h>
#include <stdio.h>
#include "evallegendre.h"


/*
 $Log: evallegendre.c,v $
 Revision 1.2  2004/04/26 21:52:03  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/


double CnvMapEvalPLM(int l,int m,double x) {
  double pmm=1.0,pnmp1=0,pll=0;
  double somx2=0;
  double fact=0;
  int i=0,ll=0;
  if (m>0) { 
    somx2=sqrt((1.0-x)*(1.0+x));
    fact=1.0;
    for (i=1;i<=m;i++) {
      pmm=-pmm*fact*somx2;
      fact+=2.0;
    }
  }
  if (l==m) return pmm;
  else {
    pnmp1=x*(2*m+1)*pmm;
    if (l==(m+1)) return pnmp1;
    else {
      for (ll=m+2;ll<=l;ll++) {
        pll=(x*(2*ll-1)*pnmp1-(ll+m-1)*pmm)/(ll-m);
        pmm=pnmp1;
        pnmp1=pll;
      }
    }
  }
  return pll;
}


void CnvMapEvalLegendre(int Lmax, double *x,int n, double *plm) {
 
  int L,m,i;
  
  for (i=0; i<n; ++i) {
    for (L=0;L<=Lmax;L++) {
      for (m=0;m<=Lmax;m++) {
         PLM(L,m,i)=CnvMapEvalPLM(L,m,x[i]);
      }
    }
  }
}
    
