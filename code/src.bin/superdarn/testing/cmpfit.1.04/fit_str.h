/* fit_str.h
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

/*
 $Log: fit_str.h,v $
 Revision 1.1  2004/07/13 21:50:30  barnes
 Initial revision

*/


#ifndef _INT_TYPES
  #include "rtypes.h"
#endif

#define FIT_RECL 1024
#define INX_RECL 16

struct fit_rec1 {
  int32 rrn,r_time;
  int16 plist[sizeof(struct radar_parms)/2];
  int16 ppat[PULSE_PAT_LEN];
  int16 lags[2][LAG_TAB_LEN];
  char comment[ORIG_COMBF_SIZE];
  int32 r_noise_lev;
  int32 r_noise_lag0;  
  int32 r_noise_vel;
  int16 r_pwr0[ORIG_MAX_RANGE];
  int16 r_slist[ORIG_MAX_RANGE];
  char r_numlags[ORIG_MAX_RANGE];
};

struct fit_rec2 {
  int32 rrn, r_time, r_xflag;
  unsigned char range[25];
  unsigned char r_qflag[25];
  int16 r_pwr_l[25], r_pwr_l_err[25], r_pwr_s[25], r_pwr_s_err[25];
  int16 r_vel[25], r_vel_err[25], r_w_l[25], r_w_l_err[25];
  int16 r_w_s[25], r_w_s_err[25], r_phi0[25], r_phi0_err[25];
  int16 r_elev[25], r_elev_low[25], r_elev_high[25];
  int16 r_sdev_l[25], r_sdev_s[25], r_sdev_phi[25], r_gscat[25];
};

union fit_out {
  int16 rec0[512];
  struct fit_rec1 r1;
  struct fit_rec2 r2;
};



