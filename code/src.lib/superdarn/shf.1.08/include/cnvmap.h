/* cnvmap.h 
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


/*
 $Log: cnvmap.h,v $
 Revision 1.1  2004/07/12 22:12:03  barnes
 Initial revision

*/

#ifndef _CNVMAP_H
#define _CNVMAP_H

struct CnvMapData {

  int major_rev,minor_rev;
  char source[256];

  double st_time;
  double ed_time;

  int num_model;
  int doping_level;
  int model_wt;
  int error_wt;
  int imf_flag;
  int imf_delay;
   
  /* imf values */ 

  double Bx;
  double By;
  double Bz;
  
  char imf_model[2][64]; /* the IMF models used */
   
  int hemisphere;

  int fit_order;
  double latmin;
   
  int num_coef;

  double *coef; /* nx4 array of co-efficients */

  double chi_sqr;
  double chi_sqr_dat;
  double rms_err;
  
  double lon_shft;
  double lat_shft;

  struct {
    double start;
    double end;
    double av;
  } mlt;

  double pot_drop;
  double pot_drop_err;

  double pot_max;
  double pot_max_err;

  double pot_min;
  double pot_min_err;
  struct GridGVec *model;

  int num_bnd;
  double *bnd_lat;
  double *bnd_lon;


};

#endif









