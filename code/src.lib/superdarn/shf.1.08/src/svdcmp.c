/* svdcmp.c
   ======== 
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

#include "nrutil.h"


/*
 $Log: svdcmp.c,v $
 Revision 1.2  2004/04/26 21:52:03  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

double CnvMapPythag(double a, double b) { 
  double absa,absb; 
  absa=fabs(a); 
  absb=fabs(b); 
  if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa)); 
  else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

void CnvMapSVDCMP(double *a, int m, int n, double *w, double *v)  { 

  int flag,i,its,j,jj,k,l=0,nm=0; 
  double anorm,c,f,g,h,s,scale,x,y,z,*rv1; 

  scale=0,g=0,s=0;
  rv1=malloc(sizeof(double)*n); 
  g=scale=anorm=0.0; 
 
  for (i=0;i<n;i++) { 
    l=i+1; 
    rv1[i]=scale*g; 
    g=s=scale=0.0; 
    if (i < m) { 
      for (k=i;k<m;k++) scale += fabs(a[k+m*i]); 
      if (scale) {
        for (k=i;k<m;k++) {
           a[k+m*i] /= scale; 
           s += a[k+m*i]*a[k+m*i];
        }
        f=a[i+m*i];
        g = -SIGN(sqrt(s),f);
        h=f*g-s; 
        a[i+m*i]=f-g;
        for (j=l;j<n;j++) { 
          for (s=0.0,k=i;k<m;k++) s += a[k+m*i]*a[k+m*j]; 
          f=s/h; 
          for (k=i;k<m;k++) a[k+m*j] += f*a[k+m*i];
        } 
        for (k=i;k<m;k++) a[k+m*i] *= scale;
      }
    }
    w[i]=scale*g; 
    g=s=scale=0.0; 
    
    if ((i<m) && (i != (n-1))) { 
      for (k=l;k<n;k++) scale += fabs(a[i+m*k]); 
      if (scale) {
        for (k=l;k<n;k++) { 
          a[i+m*k] /= scale; 
          s += a[i+m*k]*a[i+m*k];
        } 
        f=a[i+m*l];
        g = -SIGN(sqrt(s),f); 
        h=f*g-s; 
        a[i+m*l]=f-g;        
        for (k=l;k<n;k++) rv1[k]=a[i+m*k]/h; 
        for (j=l;j<m;j++) {
          for (s=0,k=l;k<n;k++) s += a[j+m*k]*a[i+m*k]; 
          for (k=l;k<n;k++) a[j+m*k] += s*rv1[k]; 
        } 
        for (k=l;k<n;k++) a[i+m*k] *= scale;
      } 
    } 
    anorm=FMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
  } 
  
  for (i=n-1;i>=0;i--) {
    if(i < (n-1)) { 
      if (g) { 
        for (j=l;j<n;j++) v[j+n*i]=(a[i+m*j]/a[i+m*l])/g; 
        for (j=l;j<n;j++) { 
          for (s=0.0,k=l;k<n;k++) s += a[i+m*k]*v[k+n*j]; 
          for (k=l;k<n;k++) v[k+n*j] += s*v[k+n*i];
        }
      } 
      for (j=l;j<n;j++) v[i+n*j]=v[j+n*i]=0.0;
    } 
    v[i+n*i]=1.0; 
    g=rv1[i]; 
    l=i; 
  } 
 
  for (i=(IMIN(m,n))-1;i>=0;i--) {
     l=i+1; 
     g=w[i];
     for (j=l;j<n;j++) a[i+m*j]=0.0; 
     if (g) { 
       g=1.0/g; 
       for (j=l;j<n;j++) { 
         for (s=0.0,k=l;k<m;k++) s += a[k+m*i]*a[k+m*j]; 
         f=(s/a[i+m*i])*g; 
         for (k=i;k<m;k++) a[k+m*j] += f*a[k+m*i]; 
       } 
       for (j=i;j<m;j++) a[j+m*i] *= g;
     } else for (j=i;j<m;j++) a[j+m*i]=0.0;
     ++a[i+m*i];
  }
    
  for (k=n-1;k>=0;k--) { 
     for (its=1;its<=30;its++) { 
       flag=1; 
       for (l=k;l>=0;l--) {
         nm=l-1; 
         if ((fabs(rv1[l])+anorm) == anorm) { 
          
           flag=0; 
           break;
         } 
         
         if ((fabs(w[nm])+anorm) == anorm) break;
       } 
            
       if (flag) { 
         c=0.0;
         s=1.0;   
         for (i=l;i<=k;i++) {
           
           f=s*rv1[i];
           rv1[i]=c*rv1[i]; 
           
           if ((fabs(f)+anorm) == anorm) break;
              
           g=w[i]; 
           h=CnvMapPythag(f,g); 
           w[i]=h; 
           h=1.0/h; 
           c=g*h; 
           s = -f*h; 
           for (j=0;j<m;j++) { 
             y=a[j+m*nm];
             z=a[j+m*i]; 
             a[j+m*nm]=y*c+z*s; 
             a[j+m*i]=z*c-y*s; 
           }
          
         }
        
       } 
       z=w[k]; 
       if (l == k) { 
          if (z < 0.0) { 
            w[k] = -z; 
            for (j=0;j<n;j++) v[j+n*k] = -v[j+n*k];
          } 
          break; 
       } 
       if (its==30) fprintf(stderr,"No convergence\n");
             
       x=w[l];
       nm=k-1; 
       y=w[nm]; 
       g=rv1[nm]; 
       h=rv1[k]; 
       f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y); 
       g=CnvMapPythag(f,1.0); 
       f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x; 
       c=s=1.0;
       for (j=l;j<=nm;j++) { 
         i=j+1; 
         g=rv1[i]; 
         y=w[i]; 
         h=s*g; 
         g=c*g; 
         z=CnvMapPythag(f,h); 
         rv1[j]=z; 
         c=f/z; 
         s=h/z; 
         f=x*c+g*s; 
         g = g*c-x*s; 
         h=y*s; 
         y *= c; 
         for (jj=0;jj<n;jj++) { 
           x=v[jj+n*j]; 
           z=v[jj+n*i]; 
           v[jj+n*j]=x*c+z*s; 
           v[jj+n*i]=z*c-x*s; 
         }
         z=CnvMapPythag(f,h); 
         w[j]=z; 
         if (z) { 
           z=1.0/z; 
           c=f*z; 
           s=h*z; 
         } 
         f=c*g+s*y; 
         x=c*y-s*g;

         for (jj=0;jj<m;jj++) { 
           y=a[jj+m*j]; 
           z=a[jj+m*i]; 
           a[jj+m*j]=y*c+z*s; 
           a[jj+m*i]=z*c-y*s; 
         } 
       }
       rv1[l]=0.0; 
       rv1[k]=f; 
       w[k]=x;
       
    }
  }     
 free(rv1);
} 



