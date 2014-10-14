/* raw_read.c
   ==========
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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "lmt.h"
#include "raw.h"
#include "raw_read.h"

/*
 $Log: raw_read.c,v $
 Revision 1.1  2004/05/10 21:04:37  barnes
 Initial revision

*/

int raw_read(struct rawfp *fp,struct rawdata *raw_data) { 
  if (fp==NULL) return -1;
  return (fp->rawread)(fp,raw_data);
}
 
int raw_read_data(struct rawfp *fp,struct rawdata *raw_data) {

  struct rawdata *raw_tmp;


 /* reads only the power and ACFs without altering the 
    radar parameters */



  if (fp==NULL) return -1;
  raw_tmp=malloc(sizeof(struct rawdata));
  if (raw_tmp==NULL) return -1;
  if ( (fp->rawread)(fp,raw_tmp) !=0) return -1;

  raw_data->PARMS.NOISE=raw_tmp->PARMS.NOISE;
  raw_data->PARMS.TFREQ=raw_tmp->PARMS.TFREQ;

  memcpy(raw_data->pwr0,raw_tmp->pwr0,
         sizeof(int32)*ORIG_MAX_RANGE);

  memcpy(raw_data->acfd,raw_tmp->acfd,
		 sizeof(int32)*2*LAG_TAB_LEN*ORIG_MAX_RANGE);
  memcpy(raw_data->xcfd,raw_tmp->xcfd,
		 sizeof(int32)*2*LAG_TAB_LEN*ORIG_MAX_RANGE);
  free(raw_tmp); 
  return 0;
}




