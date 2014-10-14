/* phong.c
   =======
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

#include "rmath.h"

/*
 $Log: phong.c,v $
 Revision 1.2  2004/05/14 19:40:22  barnes
 Changed header name.

 Revision 1.1  2003/08/06 20:11:17  barnes
 Initial revision

*/

void PhongModel(int wdt,int hgt,int emin,float *zbuf,float *Lvec,
                     float diff,float spec,float speca,
                     float **dptr,float **sptr) {
  
  /* this is a reasonable fast bump-map phong shader. It could
   * be a lot more efficient but as it only takes a half a second to run
   * who cares.
   */

 
  float px[4];
  float py[4];
  float pz[4];

  float Nx,Ny,Nz;
  float N;
  float NdL,NdV,LdV;
  float D,S;
  float Vvec[3]={0.0,0.0,1.0};
  int x,y,e,k;
  float *dbuf;
  float *sbuf;

  

  if (*dptr==NULL) *dptr=malloc(sizeof(int)*wdt*hgt);
  else *dptr=realloc(*dptr,sizeof(int)*wdt*hgt);
  if (*sptr==NULL) *sptr=malloc(sizeof(int)*wdt*hgt);
  else *sptr=realloc(*sptr,sizeof(int)*wdt*hgt);

  dbuf=*dptr;
  sbuf=*sptr;


  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
      dbuf[y*wdt+x]=1e16;
      sbuf[y*wdt+x]=1e16;

    }
  }

  for (x=1;x<wdt;x++) {
    for (y=1;y<hgt;y++) {
      e=zbuf[y*wdt+x];
      if (e==-10000) continue;
      dbuf[y*wdt+x]=-1e16;
      sbuf[y*wdt+x]=-1e16;

      if (e<emin) continue;
  
      /* calculate normal for the point */

      px[0]=px[3]=x-1;
      px[1]=px[2]=x;
      
      py[0]=py[1]=y-1;
      py[2]=py[3]=y;

      pz[0]=zbuf[(y-1)*wdt+(x-1)];
      pz[1]=zbuf[y*wdt+(x-1)];
      pz[2]=zbuf[y*wdt+x];
      pz[3]=zbuf[(y-1)*wdt+x];
  
      for (k=0;k<4;k++) {
       if (pz[k]<emin) pz[k]=0;
      }

      Nx=Ny=Nz=0;

      for (k=0;k<4;k++) {
        
        Nx+=(py[k]-py[(k+1) % 4])*(pz[k]+pz[(k+1) % 4]);
        Ny+=(pz[k]-pz[(k+1) % 4])*(px[k]+px[(k+1) % 4]);
        Nz+=(px[k]-px[(k+1) % 4])*(py[k]+py[(k+1) % 4]);
      }
      N=sqrt(Nx*Nx+Ny*Ny+Nz*Nz);
     
      Nx=Nx/N;
      Ny=Ny/N;
      Nz=Nz/N;

      NdL=Nx*Lvec[0]+Ny*Lvec[1]+Nz*Lvec[2];
      NdV=Nx*Vvec[0]+Ny*Vvec[1]+Nz*Vvec[2];
      LdV=Lvec[0]*Vvec[0]+Lvec[1]*Vvec[1]+Lvec[2]*Vvec[2];

      D=NdL;
      S=2*NdV*NdL-LdV;
      dbuf[y*wdt+x]=diff*D;
      sbuf[y*wdt+x]=spec*pow(S,speca);
    }
  }
}




