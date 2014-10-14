/* simageread.c
   ============
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
#include <sys/types.h>
#include "rtypes.h"
#include "dmap.h"
#include "simage.h"


/*
 $Log: simageread.c,v $
 Revision 1.4  2005/10/19 16:42:05  barnes
 Fixed bug in the start and end times.

 Revision 1.3  2004/06/22 17:32:17  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 18:10:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/03/03 18:48:56  barnes
 Initial revision

*/

int SImageRead(int fid,struct SImage *img) {
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  int c,x;

  ptr=DataMapRead(fid);
  if (ptr==NULL) return -1;


  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];

    if ((strcmp(s->name,"version.major")==0) && (s->type==DATAINT))
      img->version.major=*(s->data.lptr);
    if ((strcmp(s->name,"version.minor")==0) && (s->type==DATAINT))
      img->version.minor=*(s->data.lptr);

    if ((strcmp(s->name,"source")==0) && (s->type==DATASTRING)) {
      char *tptr;
      tptr=*((char **) s->data.vptr);
      if (img->source==NULL) img->source=malloc(strlen(tptr)+1);
      else {
        char *tmp;
        tmp=realloc(img->source,strlen(tptr)+1);
        if (tmp==NULL) break;
        else img->source=tmp;
      }
      strcpy(img->source,tptr);
    }
    

    if ((strcmp(s->name,"start.time")==0) && (s->type==DATADOUBLE))
      img->st_time=*(s->data.dptr);
    if ((strcmp(s->name,"end.time")==0) && (s->type==DATADOUBLE))
      img->ed_time=*(s->data.dptr);
 
    if ((strcmp(s->name,"code")==0) && (s->type==DATAINT))
      img->code=*(s->data.lptr);
    if ((strcmp(s->name,"wdt")==0) && (s->type==DATAINT))
      img->wdt=*(s->data.lptr);
    if ((strcmp(s->name,"hgt")==0) && (s->type==DATAINT))
      img->hgt=*(s->data.lptr);

    if ((strcmp(s->name,"center.flg")==0) && (s->type==DATACHAR))
      img->c.flg=*(s->data.cptr);
    if ((strcmp(s->name,"center.geo.lat")==0) && (s->type==DATAFLOAT))
      img->c.glat=*(s->data.fptr);
    if ((strcmp(s->name,"center.geo.lon")==0) && (s->type==DATAFLOAT))
      img->c.glon=*(s->data.fptr);
    if ((strcmp(s->name,"center.mag.lat")==0) && (s->type==DATAFLOAT))
      img->c.mlat=*(s->data.fptr);
    if ((strcmp(s->name,"center.mag.lon")==0) && (s->type==DATAFLOAT))
      img->c.mlon=*(s->data.fptr);
  }

  for (c=0;c<ptr->anum;c++) {
     a=ptr->arr[c];
     if ((strcmp(a->name,"max")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
       img->colors=a->rng[0];
       if (img->max==NULL) img->max=malloc(sizeof(float)*img->colors);
       else {
         float *tmp;
         tmp=realloc(img->max,sizeof(float)*img->colors);
         if (tmp==NULL) break;
         else img->max=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->max[x]=a->data.fptr[x];
     }

     if ((strcmp(a->name,"min")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
       img->colors=a->rng[0];
       if (img->max==NULL) img->min=malloc(sizeof(float)*img->colors);
       else {
         float *tmp;
         tmp=realloc(img->min,sizeof(float)*img->colors);
         if (tmp==NULL) break;
         else img->min=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->min[x]=a->data.fptr[x];
     }

     if ((strcmp(a->name,"pnt.flag")==0) && (a->type==DATACHAR) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].flg=a->data.cptr[x];
     }


    if ((strcmp(a->name,"pnt.x")==0) && (a->type==DATAINT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].x=a->data.lptr[x];
     }


     if ((strcmp(a->name,"pnt.y")==0) && (a->type==DATAINT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].y=a->data.lptr[x];
     }

     if ((strcmp(a->name,"pnt.sec")==0) && (a->type==DATAINT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].time.sec=a->data.lptr[x];
     }

     if ((strcmp(a->name,"pnt.usec")==0) && (a->type==DATAINT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].time.usec=a->data.lptr[x];
     }


     if ((strcmp(a->name,"pnt.geo.lat")==0) && (a->type==DATAFLOAT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].glat=a->data.fptr[x];
     }


     if ((strcmp(a->name,"pnt.geo.lon")==0) && (a->type==DATAFLOAT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].glon=a->data.fptr[x];
     }


     if ((strcmp(a->name,"pnt.mag.lat")==0) && (a->type==DATAFLOAT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].mlat=a->data.fptr[x];
     }


     if ((strcmp(a->name,"pnt.mag.lon")==0) && (a->type==DATAFLOAT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].mlon=a->data.fptr[x];
     }

     if ((strcmp(a->name,"pnt.alt")==0) && (a->type==DATAFLOAT) &&
       (a->dim==1)) {
       img->numpnt=a->rng[0];
       if (img->pnt==NULL) img->pnt=malloc(sizeof(struct SImagePnt)*
					   img->numpnt);
       else {
         struct SImagePnt *tmp;
         tmp=realloc(img->pnt,sizeof(struct SImagePnt)*img->numpnt);
         if (tmp==NULL) break;
         else img->pnt=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->pnt[x].alt=a->data.fptr[x];
     }



     if ((strcmp(a->name,"data")==0) && (a->type==DATAFLOAT) &&
       (a->dim==1)) {
       img->numpix=a->rng[0];
       if (img->data==NULL) img->data=malloc(sizeof(float)*img->numpix);
       else {
         float *tmp;
         tmp=realloc(img->data,sizeof(float)*img->numpix);
         if (tmp==NULL) break;
         else img->data=tmp;
       }
       for (x=0;x<a->rng[0];x++) img->data[x]=a->data.fptr[x];
     }
  }

  DataMapFree(ptr);
  return 0;

}


int SImageFread(FILE *fp,struct SImage *img) {
  return SImageRead(fileno(fp),img);
}


