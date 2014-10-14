/* grdidl.c
   ======== 
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
#include <unistd.h>
#include "idl_export.h"
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rtime.h"
#include "griddata.h"
#include "gridindex.h"
#include "gridseek.h"
#include "gridread.h"
#include "gridwrite.h"
#include "griddataidl.h"
#include "grdidl.h"

/*
 $Log: grdidl.c,v $
 Revision 1.1  2004/11/01 23:01:09  barnes
 Initial revision

*/

int32 GridIDLPoint(int argc,char *argv[]) {
  int fildes;
  int unit;
  IDL_FILE_STAT stat;
  FILE *fp;
  int32 offset;
  int s;

  unit = *( (int32 *) argv[0]);
  IDL_FileStat(unit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);

  offset=*( (int32 *) argv[1]);

  fildes=fileno(fp);
  s=lseek(fildes,offset,SEEK_SET);

  return s;

}

int32 GridIDLRead(int argc,char *argv[]) {

  int32 *idlstnum;
  int32 *idlvcnum;
  IDL_STRING *idlptr=NULL;
  struct GridData grd;

  IDL_FILE_STAT stat;
  FILE *fp;
  int unit; 
  int s;

  unit = *( (int32 *) argv[0]);

  /* Make sure that the "STDIO" keyword was used to
   * open the file in IDL.
   */

  s=IDL_FileEnsureStatus(IDL_MSG_RET,unit,IDL_EFS_STDIO);
  if (s==FALSE) return -1;

  /* Get information about the file */

  IDL_FileStat(unit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);
 
  /* Read in the record */

  memset(&grd,0,sizeof(struct GridData));
  s=GridFread(fp,&grd);
  if (s==-1) return -1;

  /* get the output pointers */

  idlstnum=(int32 *) argv[1];
  idlvcnum=(int32 *) argv[2];
  idlptr=(IDL_STRING *) argv[3];
  
  IDL_StrEnsureLength(idlptr,sizeof(struct GridData)+
                      grd.stnum*sizeof(struct GridSVec)+
                      grd.vcnum*sizeof(struct GridGVec));
  memcpy(idlptr->s,&grd,sizeof(struct GridData)); 
  memcpy(idlptr->s+sizeof(struct GridData),grd.sdata,
         grd.stnum*sizeof(struct GridSVec));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec),
         grd.data,grd.vcnum*sizeof(struct GridGVec));
  
  *idlstnum=grd.stnum;
  *idlvcnum=grd.vcnum;
  return s;
}


int32 GridIDLDecode(int argc,char *argv[]) {
  int n;
  int yr,mo,dy,hr,mt;
  double sc;
  unsigned char *ptr;
  struct GridIDLPrm *prm;
  struct GridIDLStVec *stvec;
  struct GridIDLGVec *gvec;
  struct GridData *grd;
  ptr=((IDL_STRING *) argv[0])->s;
  grd=(struct GridData *) (ptr);
  grd->sdata=(struct GridSVec *) (ptr+sizeof(struct GridData));
  grd->data=(struct GridGVec *) (ptr+sizeof(struct GridData)+
                                 sizeof(struct GridSVec)*grd->stnum);

  prm=(struct GridIDLPrm *) argv[1];
  stvec=(struct GridIDLStVec *) argv[2];
  gvec=(struct GridIDLGVec *) argv[3];

  prm->stnum=grd->stnum;
  prm->vcnum=grd->vcnum;
  prm->xtd=grd->xtd;
  TimeEpochToYMDHMS(grd->st_time,&yr,&mo,
                    &dy,&hr,&mt,
                    &sc);
  prm->start.yr=yr;
  prm->start.mo=mo;
  prm->start.dy=dy;
  prm->start.hr=hr;
  prm->start.mt=mt;
  prm->start.sc=sc;

  TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,
                    &dy,&hr,&mt,
                    &sc);

  prm->end.yr=yr;
  prm->end.mo=mo;
  prm->end.dy=dy;
  prm->end.hr=hr;
  prm->end.mt=mt;
  prm->end.sc=sc;

  for (n=0;n<grd->stnum;n++) {
    stvec[n].stid=grd->sdata[n].st_id;
    stvec[n].chn=grd->sdata[n].chn;
    stvec[n].npnt=grd->sdata[n].npnt;
    stvec[n].freq=grd->sdata[n].freq0;
    stvec[n].major_revision=grd->sdata[n].major_revision;
    stvec[n].minor_revision=grd->sdata[n].minor_revision;
    stvec[n].prog_id=grd->sdata[n].prog_id;
    stvec[n].gsct=grd->sdata[n].gsct;
    stvec[n].noise.mean=grd->sdata[n].noise.mean;
    stvec[n].noise.sd=grd->sdata[n].noise.sd;
    stvec[n].vel.min=grd->sdata[n].vel.min;
    stvec[n].vel.max=grd->sdata[n].vel.max;
    stvec[n].pwr.min=grd->sdata[n].pwr.min;
    stvec[n].pwr.max=grd->sdata[n].pwr.max;
    stvec[n].wdt.min=grd->sdata[n].wdt.min;
    stvec[n].wdt.max=grd->sdata[n].wdt.max;
    stvec[n].verr.min=grd->sdata[n].verr.min;
    stvec[n].verr.max=grd->sdata[n].verr.max;
  }

  for (n=0;n<grd->vcnum;n++) {
     gvec[n].stid=grd->data[n].st_id;
     gvec[n].chn=grd->data[n].chn;
     gvec[n].index=grd->data[n].index;
     gvec[n].mlat=grd->data[n].mlat;
     gvec[n].mlon=grd->data[n].mlon;
     gvec[n].azm=grd->data[n].azm;
     gvec[n].vel.median=grd->data[n].vel.median;
     gvec[n].vel.sd=grd->data[n].vel.sd;
     if (grd->xtd !=0) {
       gvec[n].pwr.median=grd->data[n].pwr.median;
       gvec[n].pwr.sd=grd->data[n].pwr.sd;
       gvec[n].wdt.median=grd->data[n].wdt.median;
       gvec[n].wdt.sd=grd->data[n].wdt.sd;
     }
  }


  return 0;

}



int32 GridIDLWrite(int argc,char *argv[]) {

  int n;
  struct GridIDLPrm *prm;
  struct GridIDLStVec *stvec;
  struct GridIDLGVec *gvec;
  struct GridData grd;
  IDL_FILE_STAT stat;
  FILE *fp;
  int unit; 
  int s;

  unit = *( (int32 *) argv[0]);

  /* Make sure that the "STDIO" keyword was used to
   * open the file in IDL.
   */

  s=IDL_FileEnsureStatus(IDL_MSG_RET,unit,IDL_EFS_STDIO);
  if (s==FALSE) return -1;

  /* Get information about the file */

  IDL_FileStat(unit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);

  /* get the structure pointers */

  prm=(struct GridIDLPrm *) argv[1];
  stvec=(struct GridIDLStVec *) argv[2];
  gvec=(struct GridIDLGVec *) argv[3];

  memset(&grd,0,sizeof(struct GridData));


  /* load up the data structures */

  grd.st_time=TimeYMDHMSToEpoch(prm->start.yr,prm->start.mo,prm->start.dy,
                                prm->start.hr,prm->start.mt,prm->start.sc);
  grd.ed_time=TimeYMDHMSToEpoch(prm->end.yr,prm->end.mo,prm->end.dy,
                                prm->end.hr,prm->end.mt,prm->end.sc);

  grd.stnum=prm->stnum;
  grd.vcnum=prm->vcnum;
  grd.xtd=prm->xtd;
  grd.sdata=malloc(sizeof(struct GridSVec)*grd.stnum);
  if (grd.sdata==NULL) return -1;
  if (grd.vcnum !=0) {
    grd.data=malloc(sizeof(struct GridGVec)*grd.vcnum);
    if (grd.data==NULL) {
      free(grd.sdata);
      return -1;
    }
  } 

  for (n=0;n<grd.stnum;n++) {
    grd.sdata[n].st_id=stvec[n].stid;
    grd.sdata[n].chn=stvec[n].chn;
    grd.sdata[n].npnt=stvec[n].npnt;
    grd.sdata[n].freq0=stvec[n].freq;
    grd.sdata[n].major_revision=stvec[n].major_revision;
    grd.sdata[n].minor_revision=stvec[n].minor_revision;
    grd.sdata[n].prog_id=stvec[n].prog_id;
    grd.sdata[n].gsct=stvec[n].gsct;
    grd.sdata[n].noise.mean=stvec[n].noise.mean;
    grd.sdata[n].noise.sd=stvec[n].noise.sd;
    grd.sdata[n].vel.min=stvec[n].vel.min;
    grd.sdata[n].vel.max=stvec[n].vel.max;
    grd.sdata[n].pwr.min=stvec[n].pwr.min;
    grd.sdata[n].pwr.max=stvec[n].pwr.max;
    grd.sdata[n].wdt.min=stvec[n].wdt.min;
    grd.sdata[n].wdt.max=stvec[n].wdt.max;
    grd.sdata[n].verr.min=stvec[n].verr.min;
    grd.sdata[n].verr.max=stvec[n].verr.max;
  }

  for (n=0;n<grd.vcnum;n++) {
     grd.data[n].st_id=gvec[n].stid;
     grd.data[n].chn=gvec[n].chn;
     grd.data[n].index=gvec[n].index;
     grd.data[n].mlat=gvec[n].mlat;
     grd.data[n].mlon=gvec[n].mlon;
     grd.data[n].azm=gvec[n].azm;
     grd.data[n].vel.median=gvec[n].vel.median;
     grd.data[n].vel.sd=gvec[n].vel.sd;
     if (grd.xtd !=0) {
       grd.data[n].pwr.median=gvec[n].pwr.median;
       grd.data[n].pwr.sd=gvec[n].pwr.sd;
       grd.data[n].wdt.median=gvec[n].wdt.median;
       grd.data[n].wdt.sd=gvec[n].wdt.sd;
     }
  }

  s=GridFwrite(fp,&grd);
  

  free(grd.sdata);
  if (grd.data !=NULL) free(grd.data);

  return s;

}



int32 GridIDLSeek(int argc,char *argv[]) {

  struct GridIndex inx;
  IDL_FILE_STAT stat;
  FILE *fp;
  int unit,s; 
  int32 yr,mo,dy,hr,mt;
  double sc;
  double *atme;

  inx.num=0;
  inx.tme=NULL;
  inx.inx=NULL;
  
  unit = *( (int32 *) argv[0]);

  /* Make sure that the "STDIO" keyword was used to
   * open the file in IDL.
   */

  s=IDL_FileEnsureStatus(IDL_MSG_RET,unit,IDL_EFS_STDIO);
  if (s==FALSE) return -1;

  /* Get information about the file */

  IDL_FileStat(unit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);

  yr=*( (int32 *) argv[1]);
  mo=*( (int32 *) argv[2]);
  dy=*( (int32 *) argv[3]);
  hr=*( (int32 *) argv[4]);
  mt=*( (int32 *) argv[5]);
  sc=*( (double *) argv[6]);
  atme=(double *) argv[7];

  if (argc>8) {
    inx.num=*( (int32 *) argv[8]);
    if (inx.num !=0) inx.tme=malloc(sizeof(double)*inx.num);
    if (inx.tme !=NULL) inx.inx=malloc(sizeof(int32)*inx.num);
    if (inx.inx==NULL) {
      if (inx.tme !=NULL) free(inx.tme);
      inx.tme=NULL;
      inx.num=0; 
    } 
    if ((inx.tme !=NULL) && (inx.inx !=NULL)) {
      memcpy(inx.tme,argv[9],sizeof(double)*inx.num);
      memcpy(inx.inx,argv[10],sizeof(int32)*inx.num);
    }
  }


  if (inx.num !=0) s=GridFseek(fp,yr,mo,dy,hr,mt,sc,atme,&inx);
  else s=GridFseek(fp,yr,mo,dy,hr,mt,sc,atme,NULL);

  if (inx.num !=0) {
    free(inx.inx);
    free(inx.tme);
  }

  return s;
}
