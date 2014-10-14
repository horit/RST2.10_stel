/* mapseek.c
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "dmap.h"
#include "cnvmapindex.h"

/*
 $Log: mapseek.c,v $
 Revision 1.4  2007/04/05 18:12:20  code
 Fixed meaningless use of an expression.

 Revision 1.3  2007/03/09 14:15:59  code
 Fixed search.

 Revision 1.2  2004/10/12 23:20:45  barnes
 Fixed header name.

 Revision 1.1  2004/10/12 23:19:44  barnes
 Initial revision

*/


double CnvMapGetTime(struct DataMap *ptr) {
  struct DataMapScalar *s;
  int c;
  int yr=0,mo=0,dy=0,hr=0,mt=0;
  double sc=0;  
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    if ((strcmp(s->name,"start.year")==0) && (s->type==DATASHORT)) 
      yr=*(s->data.sptr);
    if ((strcmp(s->name,"start.month")==0) && (s->type==DATASHORT))
      mo=*(s->data.sptr);
    if ((strcmp(s->name,"start.day")==0) && (s->type==DATASHORT))
      dy=*(s->data.sptr);
    if ((strcmp(s->name,"start.hour")==0) && (s->type==DATASHORT))
      hr=*(s->data.sptr);
    if ((strcmp(s->name,"start.minute")==0) && (s->type==DATASHORT))
      mt=*(s->data.sptr);
    if ((strcmp(s->name,"start.second")==0) && (s->type==DATADOUBLE))
      sc=*(s->data.sptr);
   }
   if (yr==0) return -1;
   return TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc); 
}


int CnvMapSeek(int fid,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
            struct CnvMapIndex *inx) {

  int fptr=0,tptr=0;
  struct DataMap *ptr;
  double tfile=0,tval;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (inx !=NULL) {
    int rec=0,prec=-1;
    int srec;
    int erec;

    /* search using index records */

    double stime,etime;
    stime=inx->tme[0];
    etime=inx->tme[inx->num-1];

    srec=0;
    erec=inx->num;
    if (tval<stime) { /* before start of file */
      if (atme !=NULL) *atme=stime;
      lseek(fid,inx->inx[srec],SEEK_SET);
      return 0;
    } else if (tval>etime) { /* after end of file */
      if (atme !=NULL) *atme=stime;
      lseek(fid,inx->inx[erec-1],SEEK_SET);
      return -1;
    }
    do {
      prec=rec;
      rec=srec+(int) ((tval-stime)*(erec-srec))/(etime-stime);
      if (inx->tme[rec]==tval) break;
      if (inx->tme[rec]<tval) {
         srec=rec;
         stime=inx->tme[rec];
      } else {
         erec=rec;
         etime=inx->tme[rec];
      }
    } while (prec != rec);
    if (atme !=NULL) *atme=inx->tme[rec]; 
    lseek(fid,inx->inx[rec],SEEK_SET);
    return 0;
  } else {
    fptr=lseek(fid,0,SEEK_CUR);
    ptr=DataMapRead(fid);
    if (ptr !=NULL) {
      tfile=CnvMapGetTime(ptr);
      DataMapFree(ptr);
      if (tfile>tval) fptr=lseek(fid,0,SEEK_SET);
    } else fptr=lseek(fid,0,SEEK_SET);
    if (atme!=NULL) *atme=tfile;
    while (tval>tfile) {
      tptr=lseek(fid,0,SEEK_CUR);
      ptr=DataMapRead(fid);
      if (ptr==NULL) break;
      tfile=CnvMapGetTime(ptr);
      DataMapFree(ptr);
      if (tval>tfile) fptr=tptr;
      if (atme !=NULL) *atme=tfile;
    } 
    if (tval>tfile) return -1;
    lseek(fid,fptr,SEEK_SET);
  }
  return 0;
}



int CnvMapFseek(FILE *fp,
	    int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
            struct CnvMapIndex *inx) {
  return CnvMapSeek(fileno(fp),yr,mo,dy,hr,mt,sc,atme,inx);
}
