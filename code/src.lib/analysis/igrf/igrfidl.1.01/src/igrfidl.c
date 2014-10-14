/* igrfidl.c
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

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "idl_export.h"
#include "rtypes.h"
#include "igrfcall.h"
#include "magcmp.h"

/*
 $Log: igrfidl.c,v $
 Revision 1.1  2004/08/17 19:50:16  barnes
 Initial revision

*/

int IGRFIDLCall(int argc,void *argv[]) {
  
  int s=0;
  double *date;
  double *flat;
  double *flon;
  double *elev;
  double *bx;
  double *by;
  double *bz;

  if (argc < 7) return -1;
  
  date  = (double *) argv[0];
  flat = (double *) argv[1];
  flon = (double *) argv[2];
  elev =  (double *) argv[3];
  bx = (double *) argv[4];
  by = (double *) argv[5];
  bz = (double *) argv[6];

  s=IGRFCall(*date,*flat,*flon,*elev,bx,by,bz);
  if (s !=0) return -1;
  return 0;

}

int IGRFIDLMagCmp(int argc,void *argv[]) {
  
  int s=0;
  double *date;
  double *rho;
  double *flat;
  double *flon;
  double *bx;
  double *by;
  double *bz;
  double *bt;

  if (argc < 8) return -1;
  
  date  = (double *) argv[0];
  rho =  (double *) argv[1];
  flat = (double *) argv[2];
  flon = (double *) argv[3];
  bx = (double *) argv[4];
  by = (double *) argv[5];
  bz = (double *) argv[6];
  bt = (double *) argv[7];

  s=IGRFMagCmp(*date,*rho,*flat,*flon,bx,by,bz,bt);
  if (s !=0) return -1;
  return 0;

}



