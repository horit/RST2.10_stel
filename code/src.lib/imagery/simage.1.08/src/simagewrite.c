/* simagewrite.c
   =============
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
 $Log: simagewrite.c,v $
 Revision 1.4  2004/06/22 17:32:17  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/06/17 20:43:31  barnes
 Fixed memory leak.

 Revision 1.2  2004/05/04 18:10:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/03/03 18:48:56  barnes
 Initial revision

*/

int SImageWrite(int fid,struct SImage *img) {

  struct DataMap *data;
  int *x=NULL,*y=NULL,*sec=NULL,*usec=NULL;
  char *flg=NULL;
  float *glat=NULL,*glon=NULL,*mlat=NULL,*mlon=NULL,*alt=NULL;
  int s=0,n;
  
  char *srcptr;
 
  if (img->numpnt !=0) {
    x=malloc(sizeof(int)*img->numpnt);
    if (x==NULL) s=-1;
    if (s==0) y=malloc(sizeof(int)*img->numpnt);
    if (y==NULL) s=-1;
    if (s==0) sec=malloc(sizeof(int)*img->numpnt);
    if (sec==NULL) s=-1;
    if (s==0) usec=malloc(sizeof(int)*img->numpnt);
    if (usec==NULL) s=-1;
    if (s==0) flg=malloc(img->numpnt);
    if (flg==NULL) s=-1;
    if (s==0) glat=malloc(sizeof(float)*img->numpnt);
    if (glat==NULL) s=-1;
    if (s==0) glon=malloc(sizeof(float)*img->numpnt);
    if (glon==NULL) s=-1;
    if (s==0) mlat=malloc(sizeof(float)*img->numpnt);
    if (mlat==NULL) s=-1;
    if (s==0) mlon=malloc(sizeof(float)*img->numpnt);
    if (mlon==NULL) s=-1;
    if (s==0) alt=malloc(sizeof(float)*img->numpnt);
    if (alt==NULL) s=-1;
  }
  if (s!=0) {
    if (x !=NULL) free(x);
    if (y !=NULL) free(y);
    if (sec !=NULL) free(sec);
    if (usec !=NULL) free(usec);
    if (flg !=NULL) free(flg);
    if (glat !=NULL) free(glat);
    if (glon !=NULL) free(glon);
    if (mlat !=NULL) free(mlat);
    if (mlon !=NULL) free(mlon);
     return -1;
  }


  data=DataMapMake();

  DataMapAddScalar(data,"version.major",DATAINT,&img->version.major);
  DataMapAddScalar(data,"version.minor",DATAINT,&img->version.minor);


  srcptr=img->source;
  
  if (srcptr !=NULL) DataMapAddScalar(data,"source",DATASTRING,&srcptr);
  DataMapAddScalar(data,"start.time",DATADOUBLE,&img->st_time);
  DataMapAddScalar(data,"end.time",DATADOUBLE,&img->ed_time);


  DataMapAddScalar(data,"code",DATAINT,&img->code);
  DataMapAddScalar(data,"wdt",DATAINT,&img->wdt);
  DataMapAddScalar(data,"hgt",DATAINT,&img->hgt);


  DataMapAddScalar(data,"center.flg",DATACHAR,&img->c.flg);
  DataMapAddScalar(data,"center.geo.lat",DATAFLOAT,&img->c.glat);
  DataMapAddScalar(data,"center.geo.lon",DATAFLOAT,&img->c.glon);
  DataMapAddScalar(data,"center.mag.lat",DATAFLOAT,&img->c.mlat);
  DataMapAddScalar(data,"center.mag.lon",DATAFLOAT,&img->c.mlon);

  
  DataMapAddArray(data,"min",DATAFLOAT,1,&img->colors,img->min); 
  DataMapAddArray(data,"max",DATAFLOAT,1,&img->colors,img->max); 

  if (img->numpnt !=0) {
    DataMapAddArray(data,"pnt.flag",DATACHAR,1,&img->numpnt,flg); 
    DataMapAddArray(data,"pnt.x",DATAINT,1,&img->numpnt,x); 
    DataMapAddArray(data,"pnt.y",DATAINT,1,&img->numpnt,y); 
    DataMapAddArray(data,"pnt.sec",DATAINT,1,&img->numpnt,sec); 
    DataMapAddArray(data,"pnt.usec",DATAINT,1,&img->numpnt,usec); 
    DataMapAddArray(data,"pnt.geo.lat",DATAFLOAT,1,&img->numpnt,glat); 
    DataMapAddArray(data,"pnt.geo.lon",DATAFLOAT,1,&img->numpnt,glon); 
    DataMapAddArray(data,"pnt.mag.lat",DATAFLOAT,1,&img->numpnt,mlat); 
    DataMapAddArray(data,"pnt.mag.lon",DATAFLOAT,1,&img->numpnt,mlon); 
    DataMapAddArray(data,"pnt.alt",DATAFLOAT,1,&img->numpnt,alt); 
    for (n=0;n<img->numpnt;n++) {
      flg[n]=img->pnt[n].flg;
      x[n]=img->pnt[n].x;
      y[n]=img->pnt[n].y;
      sec[n]=img->pnt[n].time.sec;
      usec[n]=img->pnt[n].time.usec;
      glat[n]=img->pnt[n].glat;
      glon[n]=img->pnt[n].glon;
      mlat[n]=img->pnt[n].mlat;
      mlon[n]=img->pnt[n].mlon;
      alt[n]=img->pnt[n].alt;
    }

  }
  if (img->numpix !=0) 
    DataMapAddArray(data,"data",DATAFLOAT,1,&img->numpix,img->data); 
 
  if (fid !=-1) s=DataMapWrite(fid,data);
  else s=DataMapSize(data);
  DataMapFree(data);

  if (x !=NULL) free(x);
  if (y !=NULL) free(y);
  if (sec !=NULL) free(sec);
  if (usec !=NULL) free(usec);
  if (flg !=NULL) free(flg);
  if (glat !=NULL) free(glat);
  if (glon !=NULL) free(glon);
  if (mlat !=NULL) free(mlat);
  if (mlon !=NULL) free(mlon);
  if (alt !=NULL) free(alt);
  return s;
}

int SImageFwrite(FILE *fp,struct SImage *img) {
  return SImageWrite(fileno(fp),img);
}
