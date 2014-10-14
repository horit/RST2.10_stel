/* fit_read_inx.c
   ==============
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

/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "lmt.h"
#include "raw.h"
#include "fit.h"

#include "fit_read.h"
#include "fit_str.h"

/*
 $Log: fit_read_inx.c,v $
 Revision 1.2  2004/06/22 18:45:44  barnes
 Replaced references of integers with "int".

 Revision 1.1  2004/05/10 21:02:44  barnes
 Initial revision

*/

int fit_read_inx(struct fitfp *ptr,int32 *buffer,int recno) {
  int i,status=0;
  if (ptr->inxfp !=-1) {
     if (recno !=-1)  /* jump to this record */
       lseek(ptr->inxfp,(recno+1)*ptr->inx_recl,SEEK_SET);
     for (i=0;(i<4) && (status==0);i++) 
     status=ConvertReadInt(ptr->inxfp,&buffer[i]);
     return status;
  } else if (ptr->ibuf !=NULL) {
     if (recno !=-1)  ptr->iptr=(recno+1)*ptr->inx_recl;
     if ((ptr->istat.st_size-ptr->iptr)<(4*sizeof(int32))) return -1; 
     for (i=0;i<4;i++) 
        ConvertToInt(ptr->ibuf+ptr->iptr+sizeof(int32)*i,&buffer[i]);
     ptr->iptr+=sizeof(int32)*4;
     return 0;
  }
  return -1;
}


