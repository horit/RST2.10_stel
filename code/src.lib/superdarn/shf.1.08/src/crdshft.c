/* crdshft.c
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
#include <math.h>

#include "rmath.h"

void CnvMapCrdShft(double *lat,double *lon,double *vazm,double lat_shft,
	           double lon_shft) {
   double aside;
   double bang;
   double cang;
   double bside;
   double dside;
   double signd;
   double arg,ang;
   
   *lon=*lon+lon_shft; 
   if (*lon>360) *lon=*lon-360;
   if (*lon<0) *lon=*lon+360;
   aside=(90.0-*lat)*PI/180.0;
   bang=(180.0-*lon)*PI/180.0;
   dside=lat_shft*PI/180.0;

   arg=cos(aside)*cos(dside)+sin(aside)*sin(dside)*cos(bang);

   if (fabs(arg)>1) arg=arg/fabs(arg); /* normalize */
   
   bside=acos(arg);
   if (bside==0) bside=0.1;

   arg=(cos(aside)-cos(bside)*cos(dside))/(sin(bside)*sin(dside));

   if (fabs(arg)>1) arg=arg/fabs(arg); /* normalize */

   ang=acos(arg);
   if (*lon>180.0) ang=2.0*PI-ang;
   
   *lon=ang*180.0/PI;
   *lat=(PI/2.0-bside)*180.0/PI;

   arg=(cos(dside)-cos(aside)*cos(bside))/(sin(aside)*sin(bside));
   if (fabs(arg)>1) arg=arg/fabs(arg);
   cang=acos(arg);
   signd=dside/fabs(dside);
    
   if (*lon<=180) *vazm=*vazm-signd*cang*180.0/PI;
   else *vazm=*vazm+signd*cang*180.0/PI;

   *lon=*lon-lon_shft;
 
   if (*lon>360) *lon=*lon-360;
   if (*lon<0) *lon=*lon+360;
}









