/* simagseek.c
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
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "dmap.h"
#include "simageindex.h"

/*
 $Log: simageseek.c,v $
 Revision 1.4  2004/06/16 17:00:01  barnes
 Added missing header.

 Revision 1.3  2004/05/04 18:10:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:26:59  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/04/26 15:57:45  barnes
 Initial revision

*/


double SImageGetStartTime(struct DataMap *ptr) {
  struct DataMapScalar *s;
  double tme=-1;
  int c;
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];

    if ((strcmp(s->name,"start.time")==0) && (s->type==DATADOUBLE))
      tme=*(s->data.dptr);
   }
   return tme; 
}


double SImageGetEndTime(struct DataMap *ptr) {
  struct DataMapScalar *s;
  double tme=-1;
  int c;
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];

    if ((strcmp(s->name,"end.time")==0) && (s->type==DATADOUBLE))
      tme=*(s->data.dptr);
   }
   return tme; 
}


int SImageSeek(int fid,
	    int yr,int mo,int dy,int hr,int mt,double sc,double *atme,
            struct SImageIndex *inx) {

  int fptr=0;
  struct DataMap *ptr;
  double tfile=0,tval;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (inx !=NULL) {
    int rec=0,prec=-1;
    int srec;
    int erec;

    /* search using index records */

    double stme,etme;
    stme=inx->time.start[0];
    etme=inx->time.end[inx->num-1];

    srec=0;
    erec=inx->num;

    if (tval<stme) { /* before start of file */
      if (atme !=NULL) *atme=stme;
      lseek(fid,inx->offset[srec],SEEK_SET);
      return 0;
    } else if (tval>etme) { /* after end of file */
      if (atme !=NULL) *atme=stme;
      lseek(fid,inx->offset[erec-1],SEEK_SET);
      return -1;
    }

    do {
      prec=rec;
      rec=srec+(int) ((tval-stme)*(erec-srec))/(etme-stme);
      if (inx->time.start[rec]==tval) break;
      if (inx->time.start[rec]<tval) {
         srec=rec;
         stme=inx->time.start[rec];
      } else {
         erec=rec;
         etme=inx->time.end[rec];
      }
    } while (prec != rec);
    if (atme !=NULL) *atme=inx->time.start[rec]; 
    lseek(fid,inx->offset[rec],SEEK_SET);
    return 0;
  } else {
    ptr=DataMapRead(fid);
    if (ptr !=NULL) {
      tfile=SImageGetStartTime(ptr);
      DataMapFree(ptr);
      if (tfile>tval) fptr=lseek(fid,0,SEEK_SET);
      else fptr=lseek(fid,0,SEEK_CUR);
    } else fptr=lseek(fid,0,SEEK_SET);
    if (atme!=NULL) *atme=tfile;
    do {
      ptr=DataMapRead(fid);
      if (ptr==NULL) break;
      tfile=SImageGetStartTime(ptr);
      DataMapFree(ptr);
      if (tval>tfile) fptr=lseek(fid,0,SEEK_CUR);
      if (atme !=NULL) *atme=tfile;
    } while (tval>tfile);
    if (tval>tfile) return -1;
    lseek(fid,fptr,SEEK_SET);
  }
  return 0;
}



int SImageFseek(FILE *fp,
	    int yr,int mo,int dy,int hr,int mt,double sc,double *atme,
            struct SImageIndex *inx) {
  return SImageSeek(fileno(fp),yr,mo,dy,hr,mt,sc,atme,inx);
}
