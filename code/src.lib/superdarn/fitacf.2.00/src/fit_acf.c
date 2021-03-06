/* fit_acf.c
   =========
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
 $Log: fit_acf.c,v $
 Revision 1.11  2007/02/02 21:38:42  code
  Modifications made to leave fluctuation level(s) intact and to subtract lag 0 n
 oise from R(0).


 Revision 1.10  2005/06/30 23:28:27  barnes
 Corrected problems discovered in the IDL version of the code.

 Revision 1.9  2005/06/30 16:50:46  barnes
 Minor bug fixes.

 Revision 1.8  2004/05/04 22:54:21  barnes
 Changed math header name.

 Revision 1.7  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.6  2004/01/08 22:20:39  barnes
 Kile Baker's Modifications for improved velocity error and ground scatter
 determination.

 Revision 1.5  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.4  2002/03/05 16:59:40  barnes
 Fixed bug that meant nonsense was returned if a power fit failed.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>

#include "rmath.h"
#include "limit.h"
#include "badsmp.h"
#include "fitblk.h"


#include "acf_preproc.h" 
#include "calc_phi_res.h"
#include "omega_guess.h"
#include "badlags.h"
#include "more_badlags.h"
#include "do_phase_fit.h"  
  

#define determ(aa,bb,cc,dd) (aa*dd - bb*cc)

int fit_acf (struct complex *acf,int range,
             int *badlag,struct FitACFBadSample *badsmp,int lag_lim,
			 struct FitPrm *prm,
		     double noise_lev_in,char xflag,double xomega,
		     struct FitRange *ptr) {

  double sum_np,sum_w,sum_wk,sum_wk2,sum_wk2_arr[LAG_SIZE],sum_wk4;
  double sum_p,sum_pk,sum_pk2,sum_phi,sum_kphi;
  double t0,t2,t4,phi_res[LAG_SIZE];
  int j;
  long k;
  int npp;
  double tau[LAG_SIZE], tau2[LAG_SIZE];
  double phi_k[LAG_SIZE]; 
  double w[LAG_SIZE], pwr[LAG_SIZE];
  double wt[LAG_SIZE], wt2[LAG_SIZE], wp[LAG_SIZE];
  double e2;

  double c_log,c_log_err,d;
  double omega_loc, omega_err_loc, phi_loc, noise_lev;
  double omega_base, omega_high, omega_low;
  double phase_sdev;
  double temp;
  double phi_err, omega_err;
  double wbar;

/*  The following variables have been added for version 2.0 of cfitacf */

  double /*P0, */ P0n;   /* this is the power level where the acf levels off */
  int last_good;
  int bad_pwr[LAG_SIZE];

  int status;

/* the following array has been added to support preprocessing of the
   acf */

  int acf_stat=ACF_UNMODIFIED;

/* ----------------End of declarations ----------------------------------*/
  
  if (cabs(acf[0]) < noise_lev_in) {
    for (j=0; j<prm->mplgs; j++) badlag[j]=3;
    ptr->p_l = 0.0;
    ptr->p_s = 0.0;
    ptr->p_l_err = 0.0;
    ptr->p_s_err = 0.0;
    ptr->v = 0.0;
    ptr->v_err = 0.0;
    ptr->w_l = 0.0;
    ptr->w_l_err = 0.0;
    ptr->w_s = 0.0;
    ptr->w_s_err = 0.0;
    ptr->phi0 = 0.0;
    ptr->phi0_err = 0.0;
    ptr->sdev_l = 0.0;
    ptr->sdev_s = 0.0;
    return 2;
  }

  /* initialize the table of abs(acf[k]) and log(abs(acf[k])) */

  FitACFCkRng(range, badlag, badsmp, prm);

  /* Save the original ACF in a new variable so we can try some
     preprocessing on it.
  */

/*  for (k=0; k < prm->mplgs; k++) {
    orig_acf[k].x = acf[k].x;
    orig_acf[k].y = acf[k].y;
  }
*/

  /* This next statement provides a hook for a routine to pre-process
    the ACF and return a modified ACF that will actually be fitted.

    The function acf_preproc should return a 0 if no change was made
    and a non-zero value otherwise - the actual non-zero value may be
    used to provide information about the change that was made.  Specifically
    the following values should be defined:
	   ACF_UNMODIFIED	  the ACF was unchanged
       ACF_GROUND_SCAT    the preprocessing indicates the ACF is ground
	   						scatter.
	   ACF_ION_SCAT		  the preprocessing indicates the ACF is Ionospheric
	   ACF_MIXED_SCAT	  the preprocessing indicates a mixture of
	   						ionospheric and ground scatter.

    The original acf will be in the array "orig_acf" and the modified
    acf will be in "acf".

    To write an acf_preprocessor you must use the calling convention below.
    The revised acf will be returned in the array acf.  You must also provide
    the value of the noise_level (noise_lev_in), the range number, the badlag
    table, and the number of lags
  */

  noise_lev = noise_lev_in;
/*
  if (noise_lev != 0.0) acf_stat = acf_preproc (acf, orig_acf, &noise_lev,
			range, badlag, prm->mplgs);
*/
  for (k=0; k<prm->mplgs; k++) {
    tau[k] = prm->lag[1][k] - prm->lag[0][k];
    tau2[k] = tau[k] * tau[k];
    w[k] = cabs(acf[k]); /* w[k] = cabs(acf[k])- noise_lev; */
     if (w[k] <= noise_lev) w[k] = 0.1; /* if (w[k] <= 0.0) w[k] = 0.1; */
  }

  /* we now have to compute the amount of power to subtract off the 
   power level at each range.  The amouont to be subtracted is P(0)/sqrt(nave)
   which is the approximate expectation value of the power level after the
   ACF has decorrelated. 

   [ To derive this, determine the expectation value of
   P**2 = R(tau)*conj(R(tau))]
  */

  P0n = w[0]/sqrt((double) prm->nave);


  if ((w[0] - P0n) < noise_lev) return 2;  
    /* give up if left over pwr is too low */


  /*  identify any additional bad lags */

  sum_np = more_badlags(w, badlag, noise_lev, prm->mplgs,prm->nave);

  ptr->nump = (char) sum_np;

  /*  We must have at least lag_lim good lags */

  if (sum_np < lag_lim) return 4;

  if (noise_lev <= 0.0) noise_lev = 0.1; 
    /* this is required to make logs ok */
  w[0]=w[0]-prm->noise; /* This is to remove background delta-correlated noise from lag 0 power (version 2.0)*/

  /* OK, now we have determined the good lags for the phase fit.  
     Now subtract of P0n from the power profile */

  /* calculate the power values for each lag.  'w' is the linear power.
     wt is the power times the lag.  wt2 is power times lag**2.  
     pwr is the log of the power. wp is the linear power times the log of
     the power.  The items that involve the linear power are all parts of 
     the least squares fits with the weighting done by the linear power. */

  for (k=0; k<prm->mplgs; k++) {
 
    if (w[k] <= P0n) w[k] = 0.1; /* if (w[k] <= 0.0) w[k] = 0.1; */
    wt[k] = w[k]*w[k]*tau[k];
    wt2[k] = wt[k]*tau[k];
    pwr[k] = log(w[k]);
    wp[k] = w[k]*w[k]*pwr[k];
  }
  /* we now have to check to see how many additional bad lags have been
     introduced by subtracting off P0n. */

  for (k=0, npp=0; k < prm->mplgs; k++) {
    if (w[k] < noise_lev+P0n && !badlag[k]) bad_pwr[k] = 1; /* if (w[k] < noise_lev && !badlag[k]) bad_pwr[k] = 1; */
    else bad_pwr[k] = 0;
    if (! (badlag[k] || bad_pwr[k])) ++npp;
  }

 /* initialize the sums */

  sum_np = 1;
  sum_w = w[0]*w[0];
  sum_wk = 0;
  sum_wk2 = 0;
  sum_wk2_arr[0] = 0;
  sum_wk4 = 0;
  sum_p = w[0]*w[0]*pwr[0];
  sum_pk = 0;
  sum_pk2 = 0;
  phi_loc = atan2(acf[0].y, acf[0].x);
  sum_kphi = 0;
  t0 =  prm->mpinc * 1.0e-6;
  t2 = t0 * t0;
  t4 = t2 * t2;

  /* calculate all the residual phases */
  /* if calc_phi_res returns a bad status abort the fit */

  if (calc_phi_res(acf, badlag, phi_res, prm->mplgs) != 0){
    return 2;
  }

  if (!xflag) {
    if (acf_stat == ACF_GROUND_SCAT) omega_loc = 0.0;
    else omega_loc = omega_guess(acf, tau, badlag, phi_res, 
				 &omega_err_loc,prm->mpinc,prm->mplgs);
    phi_k[0] = 0;
    sum_phi = 0;
  } else {
    phi_k[0] = phi_loc;
    sum_phi = phi_loc * w[0]*w[0];
    omega_loc = xomega;
  }


  /*  The preliminaries are now over.  
  Now start the fitting process */

  /* first, calculate the sums needed for the phase fit */

  for (k=1; k<prm->mplgs; k++) {
    if (badlag[k]) {
      sum_wk2_arr[k] = sum_wk2_arr[k-1];
      continue;
    }
    sum_w = sum_w + w[k]*w[k];
    sum_np = sum_np + 1;
    sum_wk = sum_wk + w[k]*w[k]*tau[k];
    sum_wk2 = sum_wk2 + wt2[k];
    sum_wk2_arr[k] = sum_wk2;
  }

  /* Now do the phase fit using the best initial guess for omega */

 
  status = do_phase_fit (omega_loc, xflag, prm->mplgs, acf, tau,
			 w, sum_wk2_arr, phi_res, badlag, t0,
			 sum_w, sum_wk, sum_wk2,
			 &omega_base, &phi_loc, &phase_sdev,
			 &phi_err, &omega_err);

  ptr->phi0 = phi_loc;
  ptr->v = omega_base;
  ptr->sdev_phi = phase_sdev;
  ptr->phi0_err = phi_err;
  ptr->v_err = omega_err;

  /* check the status of the phase fit to see if it was actually OK.  
     if not, set error bars to HUGE_VAL */

  if (status != 0) {
    ptr->sdev_phi = HUGE_VAL;
    ptr->v_err = HUGE_VAL;
    if (xflag) ptr->phi0_err = HUGE_VAL;
  }
  
  /* OK, we now have our baseline value for omega.  Now re-do the
     phase fit, but using omega_loc + omega__err_loc. */


  if (!xflag && (status == 0)) {
  status = do_phase_fit (omega_loc + omega_err_loc,
                         xflag, prm->mplgs, acf, tau,
			 w, sum_wk2_arr, phi_res, badlag, t0,
			 sum_w, sum_wk, sum_wk2,
			 &omega_high, &phi_loc, &phase_sdev,
			 &phi_err, &omega_err);

  status = do_phase_fit (omega_loc - omega_err_loc, 
                         xflag, prm->mplgs, acf, tau,
			 w, sum_wk2_arr, phi_res, badlag, t0,
			 sum_w, sum_wk, sum_wk2,
			 &omega_low, &phi_loc, &phase_sdev,
			 &phi_err, &omega_err);

  /* if the difference between the high and low values of omega
     is greater than the error estimate of the original fit,
     we will use the original fit as our best guess for the
     velocity, but we'll set the error to be the difference between
     the high and low values.  Actually, at this point we should have
     non-symmetric error bar, but the file format has no provision 
     for that. */

  if (fabs(omega_high - omega_low) >= 2*ptr->v_err) {
    ptr->v = omega_base;
    ptr->v_err = ptr->v_err; /* fabs(omega_high - omega_low); */ /* modified according to task 2 of DAWG by Pasha, Tomo on Oct 14, 2014*/ 
    }
  }
  

  /* POWER FITS:  We now turn to the power fits.  The sums have to be
  partially redone, since we have subtracted P0n. */

  /* We are now faced with the question of what to do if we don't have enough 
  lags left to do a fit.  we can't abaondon the data because the phase fit is
  actually ok.  we have to have at least 3 points to do the fit and estimate 
  an error on the fit.

  If we don't have at least 3 good points, then simply set the lamda and
  sigma powers both to the power_lag0 level.  If there are only 2 good points
  then calculate the value of sigma and lamda, but set the error estimate
  to HUGE_VAL.

  If we end up with only lag-0 being good, then flag the width estimate
  by setting it to a large negative value.

*/

  if (npp < 3) {
    c_log = pwr[0];

    /* if c_log < 0 it means that after subtracting the noise and P0n,
     the result is less than 1.0.  This must really be pretty meaningless
     It shouldn't even be possible since we have supposedly already checked
     this at the beginning. */

    if (c_log < 0 ) return 2;    

    ptr->p_l = c_log;
    ptr->p_s = c_log;

    /* find the last good lag */
    last_good = -1;
    for (k= 0; k < prm->mplgs; k++) if (!badlag[k]) last_good = k;

    /* if there are no good lags, or only lag-0 is good, set the width
       to a high negative value, by setting the last_good lag to 1
       */

    if (last_good <=0 ) {
  	  ptr->w_l = -9999.0;
	  ptr->w_s = -9999.0;
	  ptr->p_l_err = HUGE_VAL;
	  ptr->p_s_err = HUGE_VAL;
	  ptr->w_l_err = HUGE_VAL;
	  ptr->w_s_err = HUGE_VAL;
	  ptr->sdev_l = HUGE_VAL;
	  ptr->sdev_s = HUGE_VAL;
    } else {
      /* now calculate the width as the lag-0 power divided by the
       time to the last good lag. */

      ptr->w_l = c_log/(tau[last_good]*t0);
      ptr->w_s = c_log/(tau2[last_good]*t2);

      /* set the errors to the maximum value */

      ptr->p_l_err = HUGE_VAL;
      ptr->p_s_err = HUGE_VAL;
      ptr->w_l_err = HUGE_VAL;
      ptr->w_s_err = HUGE_VAL;
      ptr->sdev_l = HUGE_VAL;
      ptr->sdev_s = HUGE_VAL;
    }
  } else {
    /*  Calculate the sums that were not used in the phase fit */
    for (k=1; k < prm->mplgs; k++) {
	  if (badlag[k] || bad_pwr[k]) continue;
	  sum_p = sum_p + wp[k];
	  sum_pk = sum_pk + pwr[k]*wt[k];
	  sum_pk2 = sum_pk2 + pwr[k]*wt2[k];
	  sum_wk4 = sum_wk4 + wt2[k]*tau2[k];
    }

    /* Now adjust the sums that were used in the phase fit, but that
       have changed because of additional bad lags */

    for (k=1; k< prm->mplgs; k++) {
	  if (bad_pwr[k]) {
	    sum_w = sum_w - w[k]*w[k];
	    sum_np = sum_np - 1;
	    sum_wk = sum_wk - w[k]*w[k]*tau[k];
	    sum_wk2 = sum_wk2 - wt2[k];
  	  }
    }
    
/*  start with the lamda fit */

    d = determ(sum_w,-t0*sum_wk,t0*sum_wk,-t2*sum_wk2);
    c_log = determ(sum_p,-t0*sum_wk,t0*sum_pk,-t2*sum_wk2)/d;

    ptr->p_l = c_log;

    ptr->w_l = determ(sum_w,sum_p,t0*sum_wk,t0*sum_pk)/d;

    if (sum_np > 3) {
	  e2 = 0.;
	  wbar = 0.;
	  npp = 0;
	  for (k=0; k<prm->mplgs; k++)
	    if ((badlag[k] == 0) && (bad_pwr[k] == 0)) {
		  temp = pwr[k] - (c_log - tau[k]*t0* (ptr->w_l));
		  e2 = e2 + w[k]*w[k]*(temp*temp);
		  wbar = wbar + w[k];
		  npp++;
	    }
	  wbar = wbar/npp;
	  ptr->sdev_l = sqrt(e2/sum_w*npp/(npp - 2)); /* This line was modified according to task 1 of DAWG, Pasha, Tomo on Oct 14, 2014 */ 

	  if ((sum_w*sum_wk2 - sum_wk*sum_wk) <=0) {
	    ptr->p_l_err = HUGE_VAL;
	    ptr->w_l_err = HUGE_VAL;
	    ptr->sdev_l = HUGE_VAL;
	  } else {
	    c_log_err = ptr->sdev_l * wbar *
		sqrt(sum_wk2/(sum_w*sum_wk2 - sum_wk*sum_wk));
	    ptr->p_l_err = c_log_err;

	    ptr->w_l_err = ptr->sdev_l * wbar *
		 sqrt(sum_w/(t2*(sum_w*sum_wk2 -
                         sum_wk*sum_wk)));
	  }
    } else {
	  ptr->p_l_err = HUGE_VAL;
	  ptr->w_l_err = HUGE_VAL;
	  ptr->sdev_l = HUGE_VAL;
    }
    

/* ----------------now do the sigma fit ------------------------ */

    d = determ(sum_w,-t2*sum_wk2,t2*sum_wk2,-t4*sum_wk4);
    c_log = determ(sum_p,-t2*sum_wk2,t2*sum_pk2,-t4*sum_wk4)/d;

    ptr->p_s = c_log;

    ptr->w_s = determ(sum_w,sum_p,t2*sum_wk2,t2*sum_pk2)/d;

    if (sum_np > 3) {	
	  e2 = 0.;
	  wbar = 0.;
	  npp = 0;
	  for (k=0; k<prm->mplgs; k++)
	    if ((badlag[k] == 0) && (bad_pwr[k] == 0)) {
		  temp = pwr[k] - (c_log - tau2[k]*t2* (ptr->w_s)); 
		  e2 = e2 + w[k]*w[k]*(temp*temp);
		  wbar = wbar + w[k];
		  npp++;
	    }
	  wbar = wbar/npp;
	  ptr->sdev_s = sqrt(e2/sum_w*npp/(npp - 2)); /* This line was modified according to task 1 of DAWG, Pasha,     Tomo on Oct 14, 2014 */

	  if ((sum_w*sum_wk4 - sum_wk2*sum_wk2) <= 0.0 ) {
	    ptr->p_s_err = HUGE_VAL;
	    ptr->w_s_err = HUGE_VAL;
	    ptr->sdev_s = HUGE_VAL;
	  } else {
	    c_log_err = ptr->sdev_s * wbar *
	      sqrt(sum_wk4/(sum_w*sum_wk4 - sum_wk2*sum_wk2));
	    ptr->p_s_err = c_log_err;	    
	    ptr->w_s_err = ptr->sdev_s * wbar * 
	       sqrt(sum_w/(t4*(sum_w*sum_wk4 - sum_wk2*sum_wk2)));


	  }
    } else {
	  ptr->p_s_err = HUGE_VAL;
	  ptr->w_s_err = HUGE_VAL;
	  ptr->sdev_s = HUGE_VAL;
    }	

    /* finally check for ground scatter fit */

    /*  first, see if an ACF preprocessor has already identified the
  	  scatter as being ground scatter.  */
	
    if (acf_stat == ACF_GROUND_SCAT) ptr->gsct = 1; 
    else {
      ptr->gsct = 0;
    }
  }
    
  /* all done - return code = 1 */
  if (npp < 1) return 4;
  else return 1;
}
