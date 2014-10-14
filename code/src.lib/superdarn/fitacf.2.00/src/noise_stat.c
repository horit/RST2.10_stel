/* noise_stat.c
   ============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
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

/*
 $Log: noise_stat.c,v $
 Revision 1.8  2007/02/02 21:40:15  code
  Removed "signal" variable, whih become obsolete due to abandoning subtraction o
 f noise ACF.


 Revision 1.7  2004/05/04 22:54:21  barnes
 Changed math header name.

 Revision 1.6  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.5  2004/01/08 22:20:39  barnes
 Kile Baker's Modifications for improved velocity error and ground scatter
 determination.

 Revision 1.4  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include "rmath.h"
#include "limit.h"
#include "fitblk.h"

#include "badsmp.h"
#include "badlags.h"

#define PLIM (1.6)
#define ROOT_3 1.7


double lag_power(struct complex *a) {
  return sqrt(a->x*a->x + a->y*a->y);
}

double noise_stat(double mnpwr,struct FitPrm *ptr,
                  struct FitACFBadSample *badsmp,
		  struct complex *acf[MAX_RANGE]) {
	         /* double *signal)  { */
  double plim;
  int i, j, np0, npt;
  int bdlag[LAG_SIZE];
  double var, sigma, P, P2;
  double temp, fluct, low_lim, high_lim;

  plim = PLIM * mnpwr;

  P = 0.0;
  P2 = 0.0;
  var = 0.0;
  np0 = 0;
  npt = 0;

  for (i=0; i < ptr->nrang; ++i) { 
    if ((acf[i][0].x > plim) || (acf[i][0].x <= 0.0)) continue;
	FitACFCkRng((i+1), bdlag,badsmp, ptr);
	++np0;
	fluct = ((double) acf[i][0].x)/sqrt(ptr->nave);
	low_lim = acf[i][0].x - 2.0*fluct;
	if (low_lim < 0) low_lim = low_lim + fluct;
	high_lim = acf[i][0].x + fluct;

	for (j=1; j < ptr->mplgs; ++j) {
      if (bdlag[j]) continue;
	  temp = lag_power(&acf[i][j]);
	  if (temp < low_lim || temp > high_lim) continue;
	  ++npt;
	  P = P + temp;
	  P2 = P2 + temp*temp;
    }
  }

  if (npt < 2) {
  /*  *signal = 0; */
	return plim/sqrt((double) ptr->nave);
  }

  P = P/npt;
  var = (P2 - P*P*npt)/((double) (npt-1));
  sigma = (var > 0.0) ? sqrt(var) : 0.0;

  /*if ((P >= sigma * ROOT_3) && (sigma > 0.0)) *signal = P;
  else *signal = 0.0; */
  return (P > sigma) ? P : sigma;
}
