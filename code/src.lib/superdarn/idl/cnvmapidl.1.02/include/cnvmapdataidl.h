/* cnvmapdataidl.h
   =============== 
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
 $Log: cnvmapdataidl.h,v $
 Revision 1.1  2004/11/03 22:56:14  barnes
 Initial revision

*/

#ifndef _CNVMAPDATAIDL_H
#define _CNVMAPDATAIDL_H

struct CnvMapIDLPrm {
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    double sc;
  } start;
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    double sc;
  } end;
  int32 stnum;
  int32 vcnum;
  int16 xtd;
  int16 major_rev;
  int16 minor_rev;
  
  IDL_STRING source;

  int32 modnum;
  int16 doping_level;
  int16 model_wt;
  int16 error_wt;
  int16 imf_flag;
  int16 imf_delay;
  double Bx;
  double By;
  double Bz;
  
  IDL_STRING model[2];

  int16 hemisphere;
  int16 fit_order;
  float latmin;
  int16 coefnum;
  double chi_sqr;
  double chi_sqr_dat;
  double rms_err;
  float lon_shft;
  float lat_shft;
  struct {
    double st;
    double ed;
    double av;
  } mlt;
  double pot_drop;
  double pot_drop_err;
  double pot_max;
  double pot_max_err;
  double pot_min;
  double pot_min_err;
  int32 bndnum;
};


struct CnvMapIDLBnd {
  float lat;
  float lon;
};



#endif

