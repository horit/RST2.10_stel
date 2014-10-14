/* copygrid.c
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"

/*
 $Log: copygrid.c,v $
 Revision 1.5  2004/10/08 20:09:11  barnes
 Changed griddata structure.

 Revision 1.4  2004/06/16 17:06:44  barnes
 Added missing header.

 Revision 1.3  2004/05/05 15:47:01  barnes
 Added missing header.

 Revision 1.2  2004/05/04 18:29:58  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

void GridCopy(struct GridData *a,struct GridData *b) {

  a->st_time=b->st_time;
  a->ed_time=b->ed_time;

  a->stnum=b->stnum;
  a->vcnum=b->vcnum;
  a->xtd=b->xtd;

  if (b->stnum>0) {
    if (a->sdata==NULL) a->sdata=malloc(sizeof(struct GridSVec)*b->stnum);
    else a->sdata=realloc(a->sdata,sizeof(struct GridSVec)*b->stnum);
  } else if (a->sdata !=NULL) {
    free(a->sdata);
    a->sdata=NULL;
  }

  if (b->vcnum>0) {
  if (a->data==NULL) a->data=malloc(sizeof(struct GridGVec)*b->vcnum);
  else a->data=realloc(a->data,sizeof(struct GridGVec)*b->vcnum);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->stnum !=0) memcpy(a->sdata,b->sdata,
                             sizeof(struct GridSVec)*b->stnum);
  if (b->vcnum !=0) memcpy(a->data,b->data,
                             sizeof(struct GridGVec)*b->vcnum);
}
