/* simage.h
   ======= 
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
 $Log: simage.h,v $
 Revision 1.1  2004/07/12 21:47:55  barnes
 Initial revision

*/

#ifndef _SIMAGE_H
#define _SIMAGE_H

struct  SImagePnt {
  struct { 
    int sec;
    int usec;
  } time;
  unsigned char flg;
  int x,y;
  float glat,glon;  
  float mlat,mlon;
  float alt;
};


  


struct SImage {

  double st_time;
  double ed_time;
 
  struct {
    int major;
    int minor;
  } version;

  char *source;

  int code;
  float *min,*max;
  int wdt,hgt;

  struct {
    unsigned char flg;
    float glat,glon;
    float mlat,mlon;
  } c;

  int32 colors;
  int32 numpix;
  int32 numpnt;
  struct SImagePnt *pnt;
  float *data;
};


int SImageRead(int fid,struct SImage *ptr);
int SImageWrite(int fid,struct SImage *ptr);
int SImageFread(FILE *fp,struct SImage *ptr);
int SImageFwrite(FILE *fp,struct SImage *ptr);

#endif
