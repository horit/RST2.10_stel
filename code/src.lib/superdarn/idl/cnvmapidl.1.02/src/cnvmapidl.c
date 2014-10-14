/* cnvmapidl.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "idl_export.h"
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rtime.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapread.h"
#include "cnvmapwrite.h"
#include "griddataidl.h"
#include "cnvmapdataidl.h"
#include "cnvmapidl.h"

/*
 $Log: cnvmapidl.c,v $
 Revision 1.2  2004/11/03 23:35:22  barnes
 Fixed minor cosmetic bug in setting the time of the record in the map
 structure.

 Revision 1.1  2004/11/03 22:54:56  barnes
 Initial revision

*/

int32 CnvMapIDLPoint(int argc,char *argv[]) {
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

int32 CnvMapIDLRead(int argc,char *argv[]) {

  int32 *idlstnum;
  int32 *idlvcnum;
  int32 *idlmodnum;
  int32 *idlcoefnum;
  int32 *idlbndnum;


  IDL_STRING *idlptr=NULL;
  struct CnvMapData map;
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
  memset(&map,0,sizeof(struct CnvMapData));

  s=CnvMapFread(fp,&map,&grd);
  if (s==-1) return -1;

  /* get the output pointers */

  idlstnum=(int32 *) argv[1];
  idlvcnum=(int32 *) argv[2];
  idlmodnum=(int32 *) argv[3];
  idlcoefnum=(int32 *) argv[4];
  idlbndnum=(int32 *) argv[5];

  idlptr=(IDL_STRING *) argv[6];
  
  IDL_StrEnsureLength(idlptr,sizeof(struct GridData)+
                      grd.stnum*sizeof(struct GridSVec)+
                      grd.vcnum*sizeof(struct GridGVec)+
                      sizeof(struct CnvMapData)+
                      map.num_model*sizeof(struct GridGVec)+
                      map.num_coef*4*sizeof(double)+ 
                      map.num_bnd*2*sizeof(double));
  memcpy(idlptr->s,&grd,sizeof(struct GridData)); 
  memcpy(idlptr->s+sizeof(struct GridData),grd.sdata,
         grd.stnum*sizeof(struct GridSVec));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec),
         grd.data,grd.vcnum*sizeof(struct GridGVec));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec)+
          grd.vcnum*sizeof(struct GridGVec),
          &map,sizeof(struct CnvMapData));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec)+
         grd.vcnum*sizeof(struct GridGVec)+sizeof(struct CnvMapData),
         map.model,map.num_model*sizeof(struct GridGVec));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec)+
         grd.vcnum*sizeof(struct GridGVec)+sizeof(struct CnvMapData)+
         map.num_model*sizeof(struct GridGVec),map.coef,
         map.num_coef*4*sizeof(double));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec)+
         grd.vcnum*sizeof(struct GridGVec)+sizeof(struct CnvMapData)+
         map.num_model*sizeof(struct GridGVec)+map.num_coef*4*sizeof(double),
         map.bnd_lat,map.num_bnd*sizeof(double));
  memcpy(idlptr->s+sizeof(struct GridData)+grd.stnum*sizeof(struct GridSVec)+
         grd.vcnum*sizeof(struct GridGVec)+sizeof(struct CnvMapData)+
         map.num_model*sizeof(struct GridGVec)+map.num_coef*4*sizeof(double)+
         map.num_bnd*sizeof(double),map.bnd_lon,map.num_bnd*sizeof(double));

  *idlstnum=grd.stnum;
  *idlvcnum=grd.vcnum;
  *idlmodnum=map.num_model;
  *idlcoefnum=map.num_coef;
  *idlbndnum=map.num_bnd;

  return s;
}


int32 CnvMapIDLDecode(int argc,char *argv[]) {
  int n;
  int yr,mo,dy,hr,mt;
  double sc;
  unsigned char *ptr;
  struct CnvMapIDLPrm *prm;
  struct GridIDLStVec *stvec;
  struct GridIDLGVec *gvec;
  struct CnvMapData *map;
  struct GridData *grd;
  struct GridIDLGVec *mvec;
  double *coef;
  struct CnvMapIDLBnd *bnd;
  char tmp[257];
  
  memset(tmp,0,257);

  ptr=((IDL_STRING *) argv[0])->s;
  grd=(struct GridData *) (ptr);
  ptr+=sizeof(struct GridData);
  grd->sdata=(struct GridSVec *) (ptr);
  ptr+=grd->stnum*sizeof(struct GridSVec);
  grd->data=(struct GridGVec *) (ptr);
  ptr+=grd->vcnum*sizeof(struct GridGVec);
  map=(struct CnvMapData *) (ptr);
  ptr+=sizeof(struct CnvMapData);
  map->model=(struct GridGVec *) (ptr);
  ptr+=map->num_model*sizeof(struct GridGVec);
  map->coef=(double *) (ptr);
  ptr+=map->num_coef*4*sizeof(double);
  map->bnd_lat=(double *) (ptr);
  ptr+=map->num_bnd*sizeof(double);
  map->bnd_lon=(double *) (ptr);

  prm=(struct CnvMapIDLPrm *) argv[1];
  stvec=(struct GridIDLStVec *) argv[2];
  gvec=(struct GridIDLGVec *) argv[3];
  mvec=(struct GridIDLGVec *) argv[4];
  coef=(double *) argv[5];
  bnd=(struct CnvMapIDLBnd *) argv[6];


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


  prm->major_rev=map->major_rev;
  prm->minor_rev=map->minor_rev;

  IDL_StrDelete(&prm->source,1);
  if (map->source !=NULL) {
    strncpy(tmp,map->source,256);
    IDL_StrStore(&prm->source,tmp);
  }

  prm->modnum=map->num_model;
  prm->doping_level=map->doping_level;
  prm->model_wt=map->model_wt;
  prm->error_wt=map->error_wt;
  prm->imf_flag=map->imf_flag;
  prm->imf_delay=map->imf_delay;
  prm->Bx=map->Bx;
  prm->By=map->By;
  prm->Bz=map->Bz;
  
  IDL_StrDelete(&prm->model[0],1);
  if (map->imf_model[0] !=NULL) {
     strncpy(tmp,map->imf_model[0],64);
     IDL_StrStore(&prm->model[0],tmp);
  }
  IDL_StrDelete(&prm->model[1],1);
  if (map->imf_model[1] !=NULL) {
    strncpy(tmp,map->imf_model[1],64);
    IDL_StrStore(&prm->model[1],tmp);
 }

  prm->hemisphere=map->hemisphere;
  prm->fit_order=map->fit_order;
  prm->latmin=map->latmin;
  prm->coefnum=map->num_coef;
  prm->chi_sqr=map->chi_sqr;
  prm->chi_sqr_dat=map->chi_sqr_dat;
  prm->rms_err=map->rms_err;
  prm->lon_shft=map->lon_shft;
  prm->lat_shft=map->lat_shft;
  prm->mlt.st=map->mlt.start;
  prm->mlt.ed=map->mlt.end;
  prm->mlt.av=map->mlt.av;
  prm->pot_drop=map->pot_drop;
  prm->pot_drop_err=map->pot_drop_err;
  prm->pot_max=map->pot_max;
  prm->pot_max_err=map->pot_max_err;
  prm->pot_min=map->pot_min;
  prm->pot_min_err=map->pot_min_err;
  prm->bndnum=map->num_bnd;

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

  for (n=0;n<map->num_model;n++) {
    mvec[n].mlat=map->model[n].mlat;
    mvec[n].mlon=map->model[n].mlon;
    mvec[n].azm=map->model[n].azm;
    mvec[n].vel.median=map->model[n].vel.median;
  } 

  for (n=0;n<map->num_coef;n++) {
     coef[n]=map->coef[4*n];
     coef[map->num_coef+n]=map->coef[4*n+1];
     coef[2*map->num_coef+n]=map->coef[4*n+2];
     coef[3*map->num_coef+n]=map->coef[4*n+3];
  }
 

  for (n=0;n<map->num_bnd;n++) {
    bnd[n].lat=map->bnd_lat[n];
    bnd[n].lon=map->bnd_lon[n];
 
  } 


  return 0;

}



int32 CnvMapIDLWrite(int argc,char *argv[]) {

  int n;
  struct CnvMapIDLPrm *prm;
  struct GridIDLStVec *stvec;
  struct GridIDLGVec *gvec;
  struct GridIDLGVec *mvec;
  double *coef;
  struct CnvMapIDLBnd *bnd;
  struct GridData grd;
  struct CnvMapData map;

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

  prm=(struct CnvMapIDLPrm *) argv[1];
  stvec=(struct GridIDLStVec *) argv[2];
  gvec=(struct GridIDLGVec *) argv[3];
  mvec=(struct GridIDLGVec *) argv[4];
  coef=(double *) argv[5];
  bnd=(struct CnvMapIDLBnd *) argv[6];


  memset(&grd,0,sizeof(struct GridData));
  memset(&map,0,sizeof(struct CnvMapData));

  /* load up the data structures */

  grd.st_time=TimeYMDHMSToEpoch(prm->start.yr,prm->start.mo,prm->start.dy,
                                prm->start.hr,prm->start.mt,prm->start.sc);
  grd.ed_time=TimeYMDHMSToEpoch(prm->end.yr,prm->end.mo,prm->end.dy,
                                prm->end.hr,prm->end.mt,prm->end.sc);

  grd.stnum=prm->stnum;
  grd.vcnum=prm->vcnum;
  grd.xtd=prm->xtd;

  map.num_model=prm->modnum;
  map.num_coef=prm->coefnum;
  map.num_bnd=prm->bndnum;

  grd.sdata=malloc(sizeof(struct GridSVec)*grd.stnum);
  if (grd.sdata==NULL) return -1;
  if (grd.vcnum !=0) {
    grd.data=malloc(sizeof(struct GridGVec)*grd.vcnum);
    if (grd.data==NULL) {
      free(grd.sdata);
      return -1;
    }
  } 

  if (map.num_model !=0) {
    map.model=malloc(sizeof(struct GridGVec)*map.num_model);
    if (map.model==NULL) {
      if (grd.data !=NULL) free(grd.data);
      free(grd.sdata);
      return -1;
    }
  } 

  if (map.num_coef !=0) {
    map.coef=malloc(sizeof(double)*4*map.num_coef);
    if (map.coef==NULL) {
      if (map.model !=NULL) free(map.model);
      if (grd.data !=NULL) free(grd.data);
      free(grd.sdata);
      return -1;
    }
  } 

  if (map.num_bnd !=0) {
    map.bnd_lat=malloc(sizeof(double)*map.num_bnd);
    if (map.bnd_lat==NULL) {
      if (map.coef !=NULL) free(map.coef);
      if (map.model !=NULL) free(map.model);
      if (grd.data !=NULL) free(grd.data);
      free(grd.sdata);
      return -1;
    }
  } 

  if (map.num_bnd !=0) {
    map.bnd_lon=malloc(sizeof(double)*map.num_bnd);
    if (map.bnd_lon==NULL) {
      if (map.bnd_lat !=NULL) free(map.bnd_lat);
      if (map.coef !=NULL) free(map.coef);
      if (map.model !=NULL) free(map.model);
      if (grd.data !=NULL) free(grd.data);
      free(grd.sdata);
      return -1;
    }
  } 

  map.st_time=grd.st_time;
  map.ed_time=grd.ed_time;
  map.major_rev=prm->major_rev;
  map.minor_rev=prm->minor_rev;

  if (prm->source.slen !=0) {
    n=prm->source.slen;
    if (n>256) n=256;
    strncpy(map.source,prm->source.s,n);
  }

  map.num_model=prm->modnum;
  map.doping_level=prm->doping_level;
  map.model_wt=prm->model_wt;
  map.error_wt=prm->error_wt;
  map.imf_flag=prm->imf_flag;
  map.imf_delay=prm->imf_delay;
  map.Bx=prm->Bx;
  map.By=prm->By;
  map.Bz=prm->Bz;
  
  if (prm->model[0].slen !=0) {
    n=prm->model[0].slen;
    if (n>64) n=64;
    strncpy(map.imf_model[0],prm->model[0].s,n);
  }

  if (prm->model[1].slen !=0) {
    n=prm->model[1].slen;
    if (n>64) n=64;
    strncpy(map.imf_model[1],prm->model[1].s,n);
  }

  map.hemisphere=prm->hemisphere;
  map.fit_order=prm->fit_order;
  map.latmin=prm->latmin;
  map.num_coef=prm->coefnum;
  map.chi_sqr=prm->chi_sqr;
  map.chi_sqr_dat=prm->chi_sqr_dat;
  map.rms_err=prm->rms_err;
  map.lon_shft=prm->lon_shft;
  map.lat_shft=prm->lat_shft;
  map.mlt.start=prm->mlt.st;
  map.mlt.end=prm->mlt.ed;
  map.mlt.av=prm->mlt.av;
  map.pot_drop=prm->pot_drop;
  map.pot_drop_err=prm->pot_drop_err;
  map.pot_max=prm->pot_max;
  map.pot_max_err=prm->pot_max_err;
  map.pot_min=prm->pot_min;
  map.pot_min_err=prm->pot_min_err;
  map.num_bnd=prm->bndnum;

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

  for (n=0;n<map.num_model;n++) {
    map.model[n].mlat=mvec[n].mlat;
    map.model[n].mlon=mvec[n].mlon;
    map.model[n].azm=mvec[n].azm;
    map.model[n].vel.median=mvec[n].vel.median;
  } 

  for (n=0;n<map.num_coef;n++) {
     map.coef[4*n]=coef[n];
     map.coef[4*n+1]=coef[map.num_coef+n];
     map.coef[4*n+2]=coef[2*map.num_coef+n];
     map.coef[4*n+3]=coef[3*map.num_coef+n];
  }
 

  for (n=0;n<map.num_bnd;n++) {
    map.bnd_lat[n]=bnd[n].lat;
    map.bnd_lon[n]=bnd[n].lon;
  } 


  s=CnvMapFwrite(fp,&map,&grd);
  

  free(grd.sdata);
  if (grd.data !=NULL) free(grd.data);
  if (map.model !=NULL) free(map.model);
  if (map.coef !=NULL) free(map.coef);
  if (map.bnd_lat !=NULL) free(map.bnd_lat);
  if (map.bnd_lon !=NULL) free(map.bnd_lon);



  return s;

}



int32 CnvMapIDLSeek(int argc,char *argv[]) {

  struct CnvMapIndex inx;
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


  if (inx.num !=0) s=CnvMapFseek(fp,yr,mo,dy,hr,mt,sc,atme,&inx);
  else s=CnvMapFseek(fp,yr,mo,dy,hr,mt,sc,atme,NULL);

  if (inx.num !=0) {
    free(inx.inx);
    free(inx.tme);
  }

  return s;
}
