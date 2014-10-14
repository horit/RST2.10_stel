/* fitdataidl.h
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

/*
 $Log: fitdataidl.h,v $
 Revision 1.1  2004/08/03 18:41:47  barnes
 Initial revision

*/

#ifndef _FITDATAIDL_H
#define _FITDATAIDL_H

 struct FitIDLData {
  struct {
    int32 major;
    int32 minor;
  } revision;
  struct {
    float sky;
    float lag0;
    float vel;
  } noise;
  float pwr0[MAX_RANGE];
  int16 nlag[MAX_RANGE]; 
  char qflg[MAX_RANGE]; 
  char gflg[MAX_RANGE]; 
  float p_l[MAX_RANGE]; 
  float p_l_e[MAX_RANGE];
  float p_s[MAX_RANGE];
  float p_s_e[MAX_RANGE];
  float v[MAX_RANGE];
  float v_e[MAX_RANGE];
  float w_l[MAX_RANGE];
  float w_l_e[MAX_RANGE];
  float w_s[MAX_RANGE];
  float w_s_e[MAX_RANGE];
  float sd_l[MAX_RANGE];
  float sd_s[MAX_RANGE];
  float sd_phi[MAX_RANGE];
  char x_qflg[MAX_RANGE];
  char x_gflg[MAX_RANGE];
  float x_p_l[MAX_RANGE];
  float x_p_l_e[MAX_RANGE];
  float x_p_s[MAX_RANGE];
  float x_p_s_e[MAX_RANGE];
  float x_v[MAX_RANGE];
  float x_v_e[MAX_RANGE];
  float x_w_l[MAX_RANGE];
  float x_w_l_e[MAX_RANGE];
  float x_w_s[MAX_RANGE];
  float x_w_s_e[MAX_RANGE];
  float phi0[MAX_RANGE];
  float phi0_e[MAX_RANGE];
  float elv[MAX_RANGE];
  float elv_low[MAX_RANGE];
  float elv_high[MAX_RANGE];
  float x_sd_l[MAX_RANGE];
  float x_sd_s[MAX_RANGE];
  float x_sd_phi[MAX_RANGE];   
};
 

#endif

