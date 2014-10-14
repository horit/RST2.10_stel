/* sza.c
   =====
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

/*
 $Log: sza.c,v $
 Revision 1.3  2006/09/18 21:24:00  code
 Fixed bug in solar location.

 Revision 1.2  2004/05/04 18:12:51  barnes
 Removed redundant headers.

 Revision 1.1  2004/03/08 23:14:13  barnes
 Initial revision

*/

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define cosd(a) cos(PI*(a)/180)
#define sind(a) sin(PI*(a)/180)

#define scosd(a) (acos(a)*180/PI)
#define asind(a) (asin(a)*180/PI)

double SZASgn(double a,double b) {
  double x=0;
  x=(double) (a>=0) ? a : -a;
  return (double) (b>=0) ? x: -x;
}

double SZAMod(double x,double y) {
  double quotient;
  quotient = x / y;
  if (quotient >= 0) quotient = floor(quotient);
  else quotient = -floor(-quotient);
  return(x - y *quotient);
}

int SZASolarLoc(int yr,int t1,double *mean_lon,double *dec) {

  int index,delta_yr,yr_step,i;
  double d,lambda,g,eps,L;
  double L0[12]={279.642,279.403,279.165,278.926,279.673,279.434,
		 279.196,278.957,279.704,279.465,279.226,278.982};
  double DL=0.985647;
  double G0[12]={356.892984,356.637087,356.381191,356.125295,
                 356.854999,356.599102,356.343206,356.087308,
		 356.817011,356.561113,356.31,356.05};
  double DG=0.98560028;
  double EPS0[12]={23.440722,23.440592,23.440462,23.440332,
	           23.440202,23.440072,23.439942,23.439811,
		   23.439682,23.439552,23.439422,23.439292};
  double DE=-0.00000036;
      
  d = 0.0;
  if (yr<1900) index = yr - 88;
  else index = yr - 1988;

  if (index<=0) delta_yr = index - 1;
  else if (index>10) delta_yr = index - 10;
  else  delta_yr = 0;
     
  if (index<=0) index = 1;
  if (index>12) index = 12;

  yr_step = SZASgn(1,delta_yr);
  delta_yr = fabs(delta_yr);

  for (i=1;i<=delta_yr;i++) {
    if (yr_step>0) yr=98+i-1;
    else yr=89-i;

    if (SZAMod(yr,4)==0) d = d + 366*yr_step;
    else d = d + 365*yr_step;
  }

     
  d = d + t1/86400.0;
  L = L0[index-1] + DL*d;
  g = G0[index-1] + DG*d;

  while (L<0) L = L + 360;
  while (g<0) g = g + 360;
    
  L = SZAMod(L,360.0);
  g = SZAMod(g,360.0);

  lambda = L + 1.915*sind(g) + 0.020*sind(2*g);
  eps = EPS0[index-1] + DE*d;

  *dec = asind(sind(eps)*sind(lambda));
  *mean_lon = L;
  return 0;
}


double SZAEqOfTime(double mean_lon,int yr) {

  int index;
  double coef[12][7]={
     {-105.8,596.2,4.4,-12.7,-429.0,-2.1,19.3},
     {-105.9,596.2,4.4,-12.7,-429.0,-2.1,19.3},
     {-106.1,596.2,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.2,596.2,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.4,596.1,4.4,-12.7,-428.9,-2.1,19.3},
     {-106.5,596.1,4.4,-12.7,-428.8,-2.1,19.3},
     {-106.6,596.1,4.4,-12.7,-428.8,-2.1,19.3},
     {-106.7,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-106.8,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-107.0,596.1,4.4,-12.7,-428.7,-2.1,19.3},
     {-107.2,596.1,4.4,-12.7,-428.6,-2.1,19.3},
     {-107.3,596.1,4.4,-12.7,-428.6,-2.1,19.3},
  };

  if (yr<88) index = yr + 2000 - 1988;
  if ((yr>=88) && (yr<100)) index = yr - 88;
  else if ((yr>=100) && (yr<1900)) index=yr-88;
  else index=yr-1988;

   
   if (index<0) index = 1;
   if (index>12) index = 12;

    return coef[index-1][0]*sind(mean_lon)+
           coef[index-1][1]*sind(2.0*mean_lon)+
           coef[index-1][2]*sind(3.0*mean_lon)+
           coef[index-1][3]*sind(4.0*mean_lon)+
           coef[index-1][4]*cosd(mean_lon)+
           coef[index-1][5]*cosd(2.0*mean_lon)+
           coef[index-1][6]*cosd(3.0*mean_lon);
}


double SZAAngle(double lon,double lat,double dec,double Hangle) {
  double Z;
  Z=acos(sin(PI*lat/180.0)*sin(PI*dec/180.0)+
         cos(PI*lat/180.0)*cos(PI*dec/180.0)*cos(PI*Hangle/180.0))*180.0/PI;
  return Z;
}







