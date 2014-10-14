/* grddataidl.h
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
 $Log: griddataidl.h,v $
 Revision 1.1  2004/11/01 19:10:06  barnes
 Initial revision

*/

#ifndef _GRIDDATAIDL_H
#define _GRIDDATAIDL_H

struct GridIDLPrm {
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
};

struct GridIDLStVec {
  int16 stid;
  int16 chn;
  int16 npnt;
  float freq;
  int16 major_revision;
  int16 minor_revision;
  int16 prog_id;
  int16 gsct;
  struct {
    float mean;
    float sd;
  } noise;
  struct {
    float min;
    float max;
  } vel;
  struct {
    float min;
    float max;
  } pwr;
  struct {
    float min;
    float max;
  } wdt;
  struct {
    float min;
    float max;
  } verr;
};

struct GridIDLGVec {
  float mlat;
  float mlon;
  float azm;
  struct {
    float median;
    float sd;
  } vel;
 struct {
    float median;
    float sd;
  } pwr;      
  struct {
    float median;
    float sd;
  } wdt;
  int16 stid;
  int16 chn;
  int32 index;
};




#endif

