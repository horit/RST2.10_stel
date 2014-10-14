/* simageinx.c
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


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rconvert.h"
#include "simageindex.h"

/*
 $Log: simageinx.c,v $
 Revision 1.4  2004/06/22 17:32:17  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/05/05 15:14:06  barnes
 Added missing header.

 Revision 1.2  2004/05/04 18:10:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/04/26 15:57:45  barnes
 Initial revision

*/


#define FILE_STEP 256

void SImageIndexFree(struct SImageIndex *ptr) {
  if (ptr==NULL) return;
  if (ptr->time.start !=NULL) free(ptr->time.start);
  if (ptr->time.end !=NULL) free(ptr->time.end);
  if (ptr->offset !=NULL) free(ptr->offset);
  free(ptr);
}

struct SImageIndex *SImageIndexLoad(int fid) {
  void *tmp;
  double stme,etme;
  int32 off;
  int st;
  struct SImageIndex *ptr;
  ptr=malloc(sizeof(struct SImageIndex));
  if (ptr==NULL) return NULL;

  ptr->time.start=malloc(sizeof(double)*FILE_STEP);
  ptr->time.end=malloc(sizeof(double)*FILE_STEP);
  ptr->offset=malloc(sizeof(int)*FILE_STEP);
  ptr->num=0;
 
  if ((ptr->time.start==NULL) || (ptr->time.end==NULL) || 
      (ptr->offset==NULL)) {
     if (ptr->time.start !=NULL) free(ptr->time.start);
     if (ptr->time.end !=NULL) free(ptr->time.end);
     if (ptr->offset !=NULL) free(ptr->offset);
     free(ptr);
     return NULL;
  }

  do {

    st=ConvertReadDouble(fid,&stme);
    if (st !=0) break;
    st=ConvertReadDouble(fid,&etme);
    if (st !=0) break;

    st=ConvertReadInt(fid,&off);
    if (st !=0) break;


    ptr->time.start[ptr->num]=stme;
    ptr->time.end[ptr->num]=etme;
    ptr->offset[ptr->num]=off;
    ptr->num++;
    st=1;
    if ((ptr->num % FILE_STEP)==0) {
      int inc;
      inc=FILE_STEP*(ptr->num/FILE_STEP+1);
      tmp=realloc(ptr->time.start,sizeof(double)*inc);
      if (tmp==NULL) break;
      ptr->time.start=(double *) tmp;
      tmp=realloc(ptr->time.end,sizeof(double)*inc);
      if (tmp==NULL) break;
      ptr->time.end=(double *) tmp;
      tmp=realloc(ptr->offset,sizeof(int)*inc);
      if (tmp==NULL) break;
      ptr->offset=(int *) tmp;
    }
  } while (1);
  if (st==1) {
    free(ptr->time.start);
    free(ptr->time.end);
    free(ptr->offset);
    free(ptr);
    return NULL;
  }


  tmp=realloc(ptr->time.start,sizeof(double)*ptr->num);
  if (tmp==NULL) {
    free(ptr->time.start);
    free(ptr->time.end);
    free(ptr->offset);
    free(ptr);
    return NULL;
  }
  ptr->time.start=tmp;
  tmp=realloc(ptr->time.end,sizeof(double)*ptr->num);
  if (tmp==NULL) {
    free(ptr->time.start);
    free(ptr->time.end);
    free(ptr->offset);
    free(ptr);
    return NULL;
  }
  ptr->time.end=tmp;


  tmp=realloc(ptr->offset,sizeof(int)*ptr->num);
  if (tmp==NULL) {
    free(ptr->time.start);
    free(ptr->time.end);
    free(ptr->offset);
    free(ptr);
    return NULL;
  }
  ptr->offset=tmp;
  return ptr;

}

struct SImageIndex *SImageIndexFload(FILE *fp) {
  return SImageIndexLoad(fileno(fp));
}

