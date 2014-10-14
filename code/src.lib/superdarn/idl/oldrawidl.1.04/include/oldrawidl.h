/* oldrawidl.h
   =========== 
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
 $Log: oldrawidl.h,v $
 Revision 1.1  2004/08/04 19:55:48  barnes
 Initial revision

*/

#ifndef _OLDRAWIDL_H
#define _OLDRAWIDL_H
 
struct OldRawIDLFp {
  int32 rawunit;
  int32 inxunit;
  double ctime;
  double stime;
  int32 frec;
  int32 rlen;
  int32 ptr;
  int32 thr;
  int32 major_rev;
  int32 minor_rev;

  /* The actual IDL structure has an integer for the next member */

  int (*rawread)(struct OldRawFp *ptr,struct RadarParm *,struct RawData *);

};


void OldRawIDLToRawFp(struct OldRawIDLFp *idlrawfp,struct OldRawFp *rawfp);
void OldRawRawFpToIDL(struct OldRawFp *rawfp,struct OldRawIDLFp *idlrawfp);
int32 OldRawIDLOpen(int argc,char *argv[]);
int32 OldRawIDLRead(int argc,char *argv[]);
int32 OldRawIDLWrite(int argc,char *argv[]);
int32 OldRawIDLWriteHeader(int argc,char *argv[]);
int32 OldRawIDLSeek(int argc,char *argv[]);

#endif
