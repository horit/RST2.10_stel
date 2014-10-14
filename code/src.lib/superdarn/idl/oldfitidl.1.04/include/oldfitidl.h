/* oldfitidl.h
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
 $Log: oldfitidl.h,v $
 Revision 1.1  2004/08/04 23:34:44  barnes
 Initial revision

*/

#ifndef _OLDFITIDL_H
#define _OLDFITIDL_H
 
struct OldFitIDLFp {
  int32 fitunit;
  int32 inxunit;
  int32 fit_recl;
  int32 inx_recl;
  int32 blen;
  int32 inx_srec;
  int32 inx_erec;
  double ctime;
  double stime;
  double etime;
  int32 time;
  char header[80];
  char date[32];
  char extra[256];
  char major_rev;
  char minor_rev;  

  /* The actual IDL structure has an integer for the next member */

  int (*fitread)(struct OldFitFp *ptr,struct RadarParm *prm,
                 struct FitData *fit);

};


void OldFitIDLToFitFp(struct OldFitIDLFp *idlfitfp,struct OldFitFp *fitfp);
void OldFitFitFpToIDL(struct OldFitFp *fitfp,struct OldFitIDLFp *idlfitfp);
int32 OldFitIDLOpen(int argc,char *argv[]);
int32 OldFitIDLRead(int argc,char *argv[]);
int32 OldFitIDLWrite(int argc,char *argv[]);
int32 OldFitIDLWriteHeader(int argc,char *argv[]);
int32 OldFitIDLSeek(int argc,char *argv[]);

int32 OldFitIDLInxWriteHeader(int argc,char *argv[]);
int32 OldFitIDLInxWrite(int argc,char *argv[]);
int32 OldFitIDLInxClose(int argc,char *argv[]);

#endif
