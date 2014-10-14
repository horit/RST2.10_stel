/* calcvector.c
   ============
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
#include <math.h>
#include "rmath.h"

#define Re 6362
#define h 400

/*
 $Log: calcvector.c,v $
 Revision 1.2  2004/05/04 22:47:16  barnes
 Changed math header name.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

void RPosCalcVector(double lat,double lon,double mag,double azm,
		 double *clat,double *clon) {
   
  /* a routine to transform vectors for plotting */

  double ar,cr,br,arg,asr,as;
 
  if (mag==0) {
     *clat=lat;
     *clon=lon;
     return;
  }
  ar=azm*PI/180.0;  
  cr=(90.0-lat)*PI/180.0;
  br=0.2*mag/(Re+h);
  
  arg=cos(br)*cos(cr)+sin(br)*sin(cr)*cos(ar);
  asr=acos(arg);
  as=asr*180.0/PI;
  *clat=90-as;
  if (asr !=0) arg=sin(ar)*sin(br)/sin(asr);
    else arg=0;
  *clon=lon+asin(arg)*180.0/PI;
}












