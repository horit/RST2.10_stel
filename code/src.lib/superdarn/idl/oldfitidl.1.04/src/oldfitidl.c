/* oldfitidl.c
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
#include "rprm.h"
#include "fitdata.h"
#include "rprmidl.h"
#include "fitdataidl.h"
#include "oldfitread.h"
#include "oldfitwrite.h"
#include "oldfitidl.h"

/*
 $Log: oldfitidl.c,v $
 Revision 1.5  2006/08/17 14:40:06  code
 Added origin code.

 Revision 1.4  2006/07/12 17:06:44  code
 Modification to deal with origin code.

 Revision 1.3  2004/08/27 19:32:01  barnes
 Added function to set the file pointer.

 Revision 1.2  2004/08/12 03:16:21  barnes
 Forced streams to be flushed before any operations.

 Revision 1.1  2004/08/04 23:32:57  barnes
 Initial revision

*/

void OldFitIDLToFitFp(struct OldFitIDLFp *idlfitfp,struct OldFitFp *fitfp) {

  fitfp->fit_recl=idlfitfp->fit_recl;
  fitfp->inx_recl=idlfitfp->inx_recl;
  fitfp->blen=idlfitfp->blen;
  fitfp->inx_srec=idlfitfp->inx_srec;
  fitfp->inx_erec=idlfitfp->inx_erec;
  fitfp->ctime=idlfitfp->ctime;
  fitfp->stime=idlfitfp->stime;
  fitfp->etime=idlfitfp->etime;
  fitfp->time=idlfitfp->time;
  strncpy(fitfp->header,idlfitfp->header,80);
  strncpy(fitfp->date,idlfitfp->date,80);
  strncpy(fitfp->extra,idlfitfp->extra,256);
  fitfp->major_rev=idlfitfp->major_rev;
  fitfp->minor_rev=idlfitfp->minor_rev;
  fitfp->fitread=idlfitfp->fitread;


}

void OldFitFitFpToIDL(struct OldFitFp *fitfp,struct OldFitIDLFp *idlfitfp) {

  idlfitfp->fit_recl=fitfp->fit_recl;
  idlfitfp->inx_recl=fitfp->inx_recl;
  idlfitfp->blen=fitfp->blen;
  idlfitfp->inx_srec=fitfp->inx_srec;
  idlfitfp->inx_erec=fitfp->inx_erec;
  idlfitfp->ctime=fitfp->ctime;
  idlfitfp->stime=fitfp->stime;
  idlfitfp->etime=fitfp->etime;
  idlfitfp->time=fitfp->time;
  strncpy(idlfitfp->header,fitfp->header,80);
  strncpy(idlfitfp->date,fitfp->date,80);
  strncpy(idlfitfp->extra,fitfp->extra,256);
  idlfitfp->major_rev=fitfp->major_rev;
  idlfitfp->minor_rev=fitfp->minor_rev;
  idlfitfp->fitread=fitfp->fitread;



}


int32 OldFitIDLPoint(int argc,char *argv[]) {
  int fildes;
  struct OldFitIDLFp *idlfitfp;
  IDL_FILE_STAT stat;
  FILE *fp;
  int32 offset;
  int s;

  idlfitfp=(struct OldFitIDLFp *) argv[0];  
  IDL_FileStat(idlfitfp->fitunit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);

  offset=*( (int32 *) argv[1]);

  fildes=fileno(fp);
  s=lseek(fildes,offset,SEEK_SET);

  return s;

}

int32 OldFitIDLOpen(int argc,char *argv[]) {
  struct OldFitIDLFp *idlfitfp;
  struct OldFitFp *fitfp;
  IDL_FILE_STAT stat;
  FILE *ffp=NULL,*ifp=NULL;
  int fitunit,inxunit; 
  int s;
  
  fitunit = *( (int32 *) argv[0]);
  inxunit = *( (int32 *) argv[1]);


  /* Make sure that the "STDIO" keyword was used to
   * open the file in IDL.
   */

  s=IDL_FileEnsureStatus(IDL_MSG_RET,fitunit,IDL_EFS_STDIO);
  if (s==FALSE) return -1;

  /* Get information about the file */

  IDL_FileStat(fitunit,&stat);
 
  /* Find the file pointer */

  ffp=stat.fptr; 
  
  if (ffp==NULL) return -1;

  fflush(ffp);

  s=IDL_FileEnsureStatus(IDL_MSG_RET,inxunit,IDL_EFS_STDIO);
  if (s==TRUE) {
    IDL_FileStat(inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  if (ifp !=NULL) fitfp=OldFitOpenFd(fileno(ffp),fileno(ifp));
  else fitfp=OldFitOpenFd(fileno(ffp),-1);
 
  idlfitfp=(struct OldFitIDLFp *) argv[2];
  idlfitfp->fitunit=fitunit;
  idlfitfp->inxunit=inxunit;
  OldFitFitFpToIDL(fitfp,idlfitfp);
  free(fitfp);
  return s;
}




int32 OldFitIDLRead(int argc,char *argv[]) {

  int n;

  struct OldFitIDLFp *idlfitfp;
  struct OldFitFp fitfp;
  char combftmp[COMBF_SIZE+1];
  struct RadarIDLParm *idlprm;
  struct FitIDLData *idlfit;
  struct RadarParm prm;
  struct FitData fit;

  IDL_FILE_STAT stat;
  FILE *ffp=NULL,*ifp=NULL;
  int s;

  /* zero out the data structures */

  memset(&prm,0,sizeof(struct RadarParm));
  memset(&fit,0,sizeof(struct FitData));
  memset(&combftmp,0,COMBF_SIZE+1);
  
  idlfitfp=(struct OldFitIDLFp *) argv[0];
  
  IDL_FileStat(idlfitfp->fitunit,&stat);
 
  /* Find the file pointer */

  ffp=stat.fptr; 

  fflush(ffp);
  
  if (ffp==NULL) return -1;

  if (idlfitfp->inxunit !=-1) {
    IDL_FileStat(idlfitfp->inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  fitfp.fitfp=fileno(ffp);
  if (ifp !=NULL) fitfp.inxfp=fileno(ifp);
  else fitfp.inxfp=-1;
 
  

  OldFitIDLToFitFp(idlfitfp,&fitfp);

  s=OldFitRead(&fitfp,&prm,&fit);

  OldFitFitFpToIDL(&fitfp,idlfitfp);

  if (s==-1) return -1;
   
  /* get the structure pointers */

  idlprm=(struct RadarIDLParm *) argv[1];
  idlfit=(struct FitIDLData *) argv[2];

  /* load up the structures.
   *
   * Note: We do not do a direct memcpy as the possibility
   * exists that the IDL data structure is different from 
   * the C data structure.
   *
   */

  idlprm->revision.major=prm.revision.major;
  idlprm->revision.minor=prm.revision.minor;
  idlprm->origin.code=prm.origin.code;
  IDL_StrDelete(&idlprm->origin.time,1);
  IDL_StrDelete(&idlprm->origin.command,1);
  idlprm->cp=prm.cp;
  idlprm->stid=prm.stid;
  idlprm->time.yr=prm.time.yr;
  idlprm->time.mo=prm.time.mo;
  idlprm->time.dy=prm.time.dy;
  idlprm->time.hr=prm.time.hr;
  idlprm->time.mt=prm.time.mt;
  idlprm->time.sc=prm.time.sc;
  idlprm->time.us=prm.time.us;
  idlprm->txpow=prm.txpow;
  idlprm->nave=prm.nave;
  idlprm->atten=prm.atten;
  idlprm->lagfr=prm.lagfr;
  idlprm->smsep=prm.smsep;
  idlprm->ercod=prm.ercod;
  idlprm->stat.agc=prm.stat.agc;
  idlprm->stat.lopwr=prm.stat.lopwr;
  idlprm->noise.search=prm.noise.search;
  idlprm->noise.mean=prm.noise.mean;
  idlprm->channel=prm.channel;
  idlprm->bmnum=prm.bmnum;
  idlprm->bmazm=prm.bmazm;
  idlprm->scan=prm.scan;
  idlprm->rxrise=prm.rxrise;
  idlprm->intt.sc=prm.intt.sc;
  idlprm->intt.us=prm.intt.us;
  idlprm->txpl=prm.txpl;
  idlprm->mpinc=prm.mpinc;
  idlprm->mppul=prm.mppul;
  idlprm->mplgs=prm.mplgs;
  idlprm->nrang=prm.nrang;
  idlprm->frang=prm.frang;
  idlprm->rsep=prm.rsep;
  idlprm->xcf=prm.xcf;
  idlprm->tfreq=prm.tfreq;
  idlprm->offset=prm.offset;
  idlprm->mxpwr=prm.mxpwr;
  idlprm->lvmax=prm.lvmax;

  for (n=0;n<prm.mppul;n++) idlprm->pulse[n]=prm.pulse[n];
  for (n=0;n<prm.mplgs;n++) {
    idlprm->lag[n]=prm.lag[n][0];
    idlprm->lag[LAG_SIZE+n]=prm.lag[n][1];
  }

  strncpy(combftmp,prm.combf,COMBF_SIZE);

  IDL_StrDelete(&idlprm->combf,1);
  IDL_StrStore(&idlprm->combf,combftmp);
  
  idlfit->revision.major=fit.revision.major;
  idlfit->revision.minor=fit.revision.minor;
  idlfit->noise.sky=fit.noise.skynoise;
  idlfit->noise.lag0=fit.noise.lag0;
  idlfit->noise.vel=fit.noise.vel;
  for (n=0;n<prm.nrang;n++) {
    idlfit->pwr0[n]=fit.rng[n].p_0;
    idlfit->nlag[n]=fit.rng[n].nump;
    idlfit->qflg[n]=fit.rng[n].qflg;
    idlfit->gflg[n]=fit.rng[n].gsct;
    idlfit->p_l[n]=fit.rng[n].p_l;
    idlfit->p_l_e[n]=fit.rng[n].p_l_err;
    idlfit->p_s[n]=fit.rng[n].p_s;
    idlfit->p_s_e[n]=fit.rng[n].p_s_err;
    idlfit->v[n]=fit.rng[n].v;
    idlfit->v_e[n]=fit.rng[n].v_err;
    idlfit->w_l[n]=fit.rng[n].w_l;
    idlfit->w_l_e[n]=fit.rng[n].w_l_err;
    idlfit->w_s[n]=fit.rng[n].w_s;
    idlfit->w_s_e[n]=fit.rng[n].w_s_err;
    idlfit->sd_l[n]=fit.rng[n].sdev_l;
    idlfit->sd_s[n]=fit.rng[n].sdev_s;
    idlfit->sd_phi[n]=fit.rng[n].sdev_phi;
    if (prm.xcf !=0) {
      idlfit->elv_low[n]=fit.elv[n].low;
      idlfit->elv[n]=fit.elv[n].normal;
      idlfit->elv_high[n]=fit.elv[n].high;
      idlfit->x_qflg[n]=fit.xrng[n].qflg;
      idlfit->x_gflg[n]=fit.xrng[n].gsct;
      idlfit->x_p_l[n]=fit.xrng[n].p_l;
      idlfit->x_p_l_e[n]=fit.xrng[n].p_l_err;
      idlfit->x_p_s[n]=fit.xrng[n].p_s;
      idlfit->x_p_s_e[n]=fit.xrng[n].p_s_err;
      idlfit->x_v[n]=fit.xrng[n].v;
      idlfit->x_v_e[n]=fit.xrng[n].v_err;
      idlfit->x_w_l[n]=fit.xrng[n].w_l;
      idlfit->x_w_l_e[n]=fit.xrng[n].w_l_err;
      idlfit->x_w_s[n]=fit.xrng[n].w_s;
      idlfit->x_w_s_e[n]=fit.xrng[n].w_s_err;
      idlfit->x_sd_l[n]=fit.xrng[n].sdev_l;
      idlfit->x_sd_s[n]=fit.xrng[n].sdev_s;
      idlfit->x_sd_phi[n]=fit.xrng[n].sdev_phi;
      idlfit->phi0[n]=fit.xrng[n].phi0;
      idlfit->phi0_e[n]=fit.xrng[n].phi0_err;
    }
  }
  return s;

}



int32 OldFitIDLSeek(int argc,char *argv[]) {

  struct OldFitIDLFp *idlfitfp;
  struct OldFitFp fitfp;

  IDL_FILE_STAT stat;
  FILE *ffp=NULL,*ifp=NULL;
  int s;

  int32 yr,mo,dy,hr,mt;
  double sc;
  double *atme;

  idlfitfp=(struct OldFitIDLFp *) argv[0];
  
  IDL_FileStat(idlfitfp->fitunit,&stat);
 
  /* Find the file pointer */

  ffp=stat.fptr; 
  
  if (ffp==NULL) return -1;

  fflush(ffp);

  if (idlfitfp->inxunit !=-1) {
    IDL_FileStat(idlfitfp->inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  fitfp.fitfp=fileno(ffp);
  if (ifp !=NULL) fitfp.inxfp=fileno(ifp);
  else fitfp.inxfp=-1;
 
  yr=*( (int32 *) argv[1]);
  mo=*( (int32 *) argv[2]);
  dy=*( (int32 *) argv[3]);
  hr=*( (int32 *) argv[4]);
  mt=*( (int32 *) argv[5]);
  sc=*( (double *) argv[6]);
  atme=(double *) argv[7];
 
  OldFitIDLToFitFp(idlfitfp,&fitfp);

  s=OldFitSeek(&fitfp,yr,mo,dy,hr,mt,sc,atme);

  OldFitFitFpToIDL(&fitfp,idlfitfp);

  return s;
}




int32 OldFitIDLWrite(int argc,char *argv[]) {

  int n;
  struct RadarIDLParm *idlprm;
  struct FitIDLData *idlfit;
  struct RadarParm prm;
  struct FitData fit;

  IDL_FILE_STAT stat;
  FILE *fp;
  int unit; 
  int s;

  unit = *( (int32 *) argv[0]);

  if (unit !=-1) { /* a real file */
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

  } else fp=NULL; /* dummy routine */

  /* get the structure pointers */

  idlprm=(struct RadarIDLParm *) argv[1];
  idlfit=(struct FitIDLData *) argv[2];
  
  memset(&prm,0,sizeof(struct RadarParm));
  memset(&fit,0,sizeof(struct FitData));

  /* load up the data structures */

  prm.revision.major=idlprm->revision.major;
  prm.revision.minor=idlprm->revision.minor;
  prm.cp=idlprm->cp;
  prm.stid=idlprm->stid;
  prm.time.yr=idlprm->time.yr;
  prm.time.mo=idlprm->time.mo;
  prm.time.dy=idlprm->time.dy;
  prm.time.hr=idlprm->time.hr;
  prm.time.mt=idlprm->time.mt;
  prm.time.sc=idlprm->time.sc;
  prm.time.us=idlprm->time.us;
  prm.txpow=idlprm->txpow;
  prm.nave=idlprm->nave;
  prm.atten=idlprm->atten;
  prm.lagfr=idlprm->lagfr;
  prm.smsep=idlprm->smsep;
  prm.ercod=idlprm->ercod;
  prm.stat.agc=idlprm->stat.agc;
  prm.stat.lopwr=idlprm->stat.lopwr;
  prm.noise.search=idlprm->noise.search;
  prm.noise.mean=idlprm->noise.mean;
  prm.channel=idlprm->channel;
  prm.bmnum=idlprm->bmnum;
  prm.scan=idlprm->scan;
  prm.rxrise=idlprm->rxrise;
  prm.intt.sc=idlprm->intt.sc;
  prm.intt.us=idlprm->intt.us;
  prm.txpl=idlprm->txpl;
  prm.mpinc=idlprm->mpinc;
  prm.mppul=idlprm->mppul;
  prm.mplgs=idlprm->mplgs;
  prm.nrang=idlprm->nrang;
  prm.frang=idlprm->frang;
  prm.rsep=idlprm->rsep;
  prm.xcf=idlprm->xcf;
  prm.tfreq=idlprm->tfreq;
  prm.offset=idlprm->offset;
  prm.mxpwr=idlprm->mxpwr;
  prm.lvmax=idlprm->lvmax;

  for (n=0;n<prm.mppul;n++) prm.pulse[n]=idlprm->pulse[n];
  for (n=0;n<prm.mplgs;n++) {
    prm.lag[n][0]=idlprm->lag[n];
    prm.lag[n][1]=idlprm->lag[LAG_SIZE+n];
  }

  if (strlen(IDL_STRING_STR(&idlprm->combf)) !=0) 
    strncpy(prm.combf,IDL_STRING_STR(&idlprm->combf),COMBF_SIZE);


  fit.revision.major=idlfit->revision.major;
  fit.revision.minor=idlfit->revision.minor;
  fit.noise.skynoise=idlfit->noise.sky;
  fit.noise.lag0=idlfit->noise.lag0;
  fit.noise.vel=idlfit->noise.vel;

  for (n=0;n<prm.nrang;n++) {
    fit.rng[n].p_0=idlfit->pwr0[n];
    fit.rng[n].nump=idlfit->nlag[n];
    fit.rng[n].qflg=idlfit->qflg[n];
    fit.rng[n].gsct=idlfit->gflg[n];
    fit.rng[n].p_l=idlfit->p_l[n];
    fit.rng[n].p_l_err=idlfit->p_l_e[n];
    fit.rng[n].p_s=idlfit->p_s[n];
    fit.rng[n].p_s_err=idlfit->p_s_e[n];
    fit.rng[n].v=idlfit->v[n];
    fit.rng[n].v_err=idlfit->v_e[n];
    fit.rng[n].w_l=idlfit->w_l[n];
    fit.rng[n].w_l_err=idlfit->w_l_e[n];
    fit.rng[n].w_s=idlfit->w_s[n];
    fit.rng[n].w_s_err=idlfit->w_s_e[n];
    fit.rng[n].sdev_l=idlfit->sd_l[n];
    fit.rng[n].sdev_s=idlfit->sd_s[n];
    fit.rng[n].sdev_phi=idlfit->sd_phi[n];
    if (prm.xcf !=0) {
      fit.elv[n].low=idlfit->elv_low[n];
      fit.elv[n].normal=idlfit->elv[n];
      fit.elv[n].high=idlfit->elv_high[n];
      fit.xrng[n].qflg=idlfit->x_qflg[n];
      fit.xrng[n].gsct=idlfit->x_gflg[n];
      fit.xrng[n].p_l=idlfit->x_p_l[n];
      fit.xrng[n].p_l_err=idlfit->x_p_l_e[n];
      fit.xrng[n].p_s=idlfit->x_p_s[n];
      fit.xrng[n].p_s_err=idlfit->x_p_s_e[n];
      fit.xrng[n].v=idlfit->x_v[n];
      fit.xrng[n].v_err=idlfit->x_v_e[n];
      fit.xrng[n].w_l=idlfit->x_w_l[n];
      fit.xrng[n].w_l_err=idlfit->x_w_l_e[n];
      fit.xrng[n].w_s=idlfit->x_w_s[n];
      fit.xrng[n].w_s_err=idlfit->x_w_s_e[n];
      fit.xrng[n].sdev_l=idlfit->x_sd_l[n];
      fit.xrng[n].sdev_s=idlfit->x_sd_s[n];
      fit.xrng[n].sdev_phi=idlfit->x_sd_phi[n];
      fit.xrng[n].phi0=idlfit->phi0[n];
      fit.xrng[n].phi0_err=idlfit->phi0_e[n];
    }
  }
  
  s=OldFitFwrite(fp,&prm,&fit,NULL);
  
  return s;

}


int32 OldFitIDLWriteHeader(int argc,char *argv[]) {

  struct RadarIDLParm *idlprm;
  struct FitIDLData *idlfit;

  IDL_FILE_STAT stat;
  FILE *fp;
  int unit; 
  int s;

  char vstring[256];

 
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

  idlprm=(struct RadarIDLParm *) argv[1];
  idlfit=(struct FitIDLData *) argv[2];

  sprintf(vstring,"%d.%.3d",idlfit->revision.major,idlfit->revision.minor);
  s=OldFitHeaderFwrite(fp,"IDL output","fitwrite",vstring);

  return s;
}



int32 OldFitIDLInxWriteHeader(int argc,char *argv[]) {

  int n;
  struct RadarIDLParm *idlprm;
  struct RadarParm prm;
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

  idlprm=(struct RadarIDLParm *) argv[1];

  memset(&prm,0,sizeof(struct RadarParm));

  /* load up the data structures */

  prm.revision.major=idlprm->revision.major;
  prm.revision.minor=idlprm->revision.minor;
  prm.cp=idlprm->cp;
  prm.stid=idlprm->stid;
  prm.time.yr=idlprm->time.yr;
  prm.time.mo=idlprm->time.mo;
  prm.time.dy=idlprm->time.dy;
  prm.time.hr=idlprm->time.hr;
  prm.time.mt=idlprm->time.mt;
  prm.time.sc=idlprm->time.sc;
  prm.time.us=idlprm->time.us;
  prm.txpow=idlprm->txpow;
  prm.nave=idlprm->nave;
  prm.atten=idlprm->atten;
  prm.lagfr=idlprm->lagfr;
  prm.smsep=idlprm->smsep;
  prm.ercod=idlprm->ercod;
  prm.stat.agc=idlprm->stat.agc;
  prm.stat.lopwr=idlprm->stat.lopwr;
  prm.noise.search=idlprm->noise.search;
  prm.noise.mean=idlprm->noise.mean;
  prm.channel=idlprm->channel;
  prm.bmnum=idlprm->bmnum;
  prm.scan=idlprm->scan;
  prm.rxrise=idlprm->rxrise;
  prm.intt.sc=idlprm->intt.sc;
  prm.intt.us=idlprm->intt.us;
  prm.txpl=idlprm->txpl;
  prm.mpinc=idlprm->mpinc;
  prm.mppul=idlprm->mppul;
  prm.mplgs=idlprm->mplgs;
  prm.nrang=idlprm->nrang;
  prm.frang=idlprm->frang;
  prm.rsep=idlprm->rsep;
  prm.xcf=idlprm->xcf;
  prm.tfreq=idlprm->tfreq;
  prm.offset=idlprm->offset;
  prm.mxpwr=idlprm->mxpwr;
  prm.lvmax=idlprm->lvmax;

  for (n=0;n<prm.mppul;n++) prm.pulse[n]=idlprm->pulse[n];
  for (n=0;n<prm.mplgs;n++) {
    prm.lag[n][0]=idlprm->lag[n];
    prm.lag[n][1]=idlprm->lag[LAG_SIZE+n];
  }

  if (strlen(IDL_STRING_STR(&idlprm->combf)) !=0) 
    strncpy(prm.combf,IDL_STRING_STR(&idlprm->combf),COMBF_SIZE);
  
  s=OldFitInxHeaderFwrite(fp,&prm);
  
  return s;

}


int32 OldFitIDLInxWrite(int argc,char *argv[]) {

  int n;
  struct RadarIDLParm *idlprm;
  struct RadarParm prm;
  int32 drec,dnum;
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

  drec=*((int32 *) argv[1]);
  dnum=*((int32 *) argv[2]);

  idlprm=(struct RadarIDLParm *) argv[3];

  memset(&prm,0,sizeof(struct RadarParm));

  /* load up the data structures */

  prm.revision.major=idlprm->revision.major;
  prm.revision.minor=idlprm->revision.minor;
  prm.cp=idlprm->cp;
  prm.stid=idlprm->stid;
  prm.time.yr=idlprm->time.yr;
  prm.time.mo=idlprm->time.mo;
  prm.time.dy=idlprm->time.dy;
  prm.time.hr=idlprm->time.hr;
  prm.time.mt=idlprm->time.mt;
  prm.time.sc=idlprm->time.sc;
  prm.time.us=idlprm->time.us;
  prm.txpow=idlprm->txpow;
  prm.nave=idlprm->nave;
  prm.atten=idlprm->atten;
  prm.lagfr=idlprm->lagfr;
  prm.smsep=idlprm->smsep;
  prm.ercod=idlprm->ercod;
  prm.stat.agc=idlprm->stat.agc;
  prm.stat.lopwr=idlprm->stat.lopwr;
  prm.noise.search=idlprm->noise.search;
  prm.noise.mean=idlprm->noise.mean;
  prm.channel=idlprm->channel;
  prm.bmnum=idlprm->bmnum;
  prm.scan=idlprm->scan;
  prm.rxrise=idlprm->rxrise;
  prm.intt.sc=idlprm->intt.sc;
  prm.intt.us=idlprm->intt.us;
  prm.txpl=idlprm->txpl;
  prm.mpinc=idlprm->mpinc;
  prm.mppul=idlprm->mppul;
  prm.mplgs=idlprm->mplgs;
  prm.nrang=idlprm->nrang;
  prm.frang=idlprm->frang;
  prm.rsep=idlprm->rsep;
  prm.xcf=idlprm->xcf;
  prm.tfreq=idlprm->tfreq;
  prm.offset=idlprm->offset;
  prm.mxpwr=idlprm->mxpwr;
  prm.lvmax=idlprm->lvmax;

  for (n=0;n<prm.mppul;n++) prm.pulse[n]=idlprm->pulse[n];
  for (n=0;n<prm.mplgs;n++) {
    prm.lag[n][0]=idlprm->lag[n];
    prm.lag[n][1]=idlprm->lag[LAG_SIZE+n];
  }

  if (strlen(IDL_STRING_STR(&idlprm->combf)) !=0) 
    strncpy(prm.combf,IDL_STRING_STR(&idlprm->combf),COMBF_SIZE);
  
  s=OldFitInxFwrite(fp,drec,dnum,&prm);
  
  return s;

}





int32 OldFitIDLInxClose(int argc,char *argv[]) {

  int n;
  struct RadarIDLParm *idlprm;
  struct RadarParm prm;
  int32 irec;
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

  idlprm=(struct RadarIDLParm *) argv[1];
  irec=*((int32 *) argv[2]);

  memset(&prm,0,sizeof(struct RadarParm));

  /* load up the data structures */

  prm.revision.major=idlprm->revision.major;
  prm.revision.minor=idlprm->revision.minor;
  prm.cp=idlprm->cp;
  prm.stid=idlprm->stid;
  prm.time.yr=idlprm->time.yr;
  prm.time.mo=idlprm->time.mo;
  prm.time.dy=idlprm->time.dy;
  prm.time.hr=idlprm->time.hr;
  prm.time.mt=idlprm->time.mt;
  prm.time.sc=idlprm->time.sc;
  prm.time.us=idlprm->time.us;
  prm.txpow=idlprm->txpow;
  prm.nave=idlprm->nave;
  prm.atten=idlprm->atten;
  prm.lagfr=idlprm->lagfr;
  prm.smsep=idlprm->smsep;
  prm.ercod=idlprm->ercod;
  prm.stat.agc=idlprm->stat.agc;
  prm.stat.lopwr=idlprm->stat.lopwr;
  prm.noise.search=idlprm->noise.search;
  prm.noise.mean=idlprm->noise.mean;
  prm.channel=idlprm->channel;
  prm.bmnum=idlprm->bmnum;
  prm.scan=idlprm->scan;
  prm.rxrise=idlprm->rxrise;
  prm.intt.sc=idlprm->intt.sc;
  prm.intt.us=idlprm->intt.us;
  prm.txpl=idlprm->txpl;
  prm.mpinc=idlprm->mpinc;
  prm.mppul=idlprm->mppul;
  prm.mplgs=idlprm->mplgs;
  prm.nrang=idlprm->nrang;
  prm.frang=idlprm->frang;
  prm.rsep=idlprm->rsep;
  prm.xcf=idlprm->xcf;
  prm.tfreq=idlprm->tfreq;
  prm.offset=idlprm->offset;
  prm.mxpwr=idlprm->mxpwr;
  prm.lvmax=idlprm->lvmax;

  for (n=0;n<prm.mppul;n++) prm.pulse[n]=idlprm->pulse[n];
  for (n=0;n<prm.mplgs;n++) {
    prm.lag[n][0]=idlprm->lag[n];
    prm.lag[n][1]=idlprm->lag[LAG_SIZE+n];
  }

  if (strlen(IDL_STRING_STR(&idlprm->combf)) !=0) 
    strncpy(prm.combf,IDL_STRING_STR(&idlprm->combf),COMBF_SIZE);

 
  s=OldFitInxFclose(fp,&prm,irec);
  
  return s;

}


