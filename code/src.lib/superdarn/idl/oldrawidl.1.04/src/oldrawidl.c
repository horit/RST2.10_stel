/* oldrawidl.c
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
#include "rawdata.h"
#include "rprmidl.h"
#include "rawdataidl.h"
#include "oldrawread.h"
#include "oldrawwrite.h"
#include "oldrawidl.h"

/*
 $Log: oldrawidl.c,v $
 Revision 1.5  2006/08/17 14:39:22  code
 Added origin code.

 Revision 1.4  2006/07/12 17:04:55  code
 Modification to deal with origin code.

 Revision 1.3  2004/08/27 19:12:02  barnes
 Added a function to set the file pointer.

 Revision 1.2  2004/08/12 03:12:47  barnes
 Forces the stream buffer to be flushed before any operation.

 Revision 1.1  2004/08/04 19:54:12  barnes
 Initial revision

*/

void OldRawIDLToRawFp(struct OldRawIDLFp *idlrawfp,struct OldRawFp *rawfp) {
  rawfp->ctime=idlrawfp->ctime;
  rawfp->stime=idlrawfp->stime;
  rawfp->frec=idlrawfp->frec;
  rawfp->rlen=idlrawfp->rlen;
  rawfp->ptr=idlrawfp->ptr;
  rawfp->thr=idlrawfp->thr;
  rawfp->major_rev=idlrawfp->major_rev;
  rawfp->minor_rev=idlrawfp->minor_rev;
  rawfp->rawread=idlrawfp->rawread;
}

void OldRawRawFpToIDL(struct OldRawFp *rawfp,struct OldRawIDLFp *idlrawfp) {
  idlrawfp->ctime=rawfp->ctime;
  idlrawfp->stime=rawfp->stime;
  idlrawfp->frec=rawfp->frec;
  idlrawfp->rlen=rawfp->rlen;
  idlrawfp->ptr=rawfp->ptr;
  idlrawfp->thr=rawfp->thr;
  idlrawfp->major_rev=rawfp->major_rev;
  idlrawfp->minor_rev=rawfp->minor_rev;
  idlrawfp->rawread=rawfp->rawread;
}

int32 OldRawIDLPoint(int argc,char *argv[]) {
  int fildes;
  struct OldRawIDLFp *idlrawfp;
  IDL_FILE_STAT stat;
  FILE *fp;
  int32 offset;
  int s;

  idlrawfp=(struct OldRawIDLFp *) argv[0];  
  IDL_FileStat(idlrawfp->rawunit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);

  offset=*( (int32 *) argv[1]);

  fildes=fileno(fp);
  s=lseek(fildes,offset,SEEK_SET);

  return s;

}


int32 OldRawIDLOpen(int argc,char *argv[]) {
  struct OldRawIDLFp *idlrawfp;
  struct OldRawFp *rawfp;
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

  rawfp=OldRawOpenFd(fileno(fp),0);
  idlrawfp=(struct OldRawIDLFp *) argv[1];
  idlrawfp->rawunit=unit;
  idlrawfp->inxunit=0;
  OldRawRawFpToIDL(rawfp,idlrawfp);
  free(rawfp);
  return s;
}





int32 OldRawIDLRead(int argc,char *argv[]) {

  int n;
  int x;

  struct OldRawIDLFp *idlrawfp;
  struct OldRawFp rawfp;
  char combftmp[COMBF_SIZE+1];
  struct RadarIDLParm *idlprm;
  struct RawIDLData *idlraw;
  struct RadarParm prm;
  struct RawData raw;

  IDL_FILE_STAT stat;
  FILE *fp;
  int s;

  /* zero out the data structures */

  memset(&prm,0,sizeof(struct RadarParm));
  memset(&raw,0,sizeof(struct RawData));
  memset(&combftmp,0,COMBF_SIZE+1);
  
  idlrawfp=(struct OldRawIDLFp *) argv[0];
  
  IDL_FileStat(idlrawfp->rawunit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);
   
  rawfp.rawfp=fileno(fp);
  rawfp.inxfp=0;
 
  OldRawIDLToRawFp(idlrawfp,&rawfp);

  s=OldRawRead(&rawfp,&prm,&raw);

  OldRawRawFpToIDL(&rawfp,idlrawfp);

  if (s==-1) return -1;
   
  /* get the structure pointers */

  idlprm=(struct RadarIDLParm *) argv[1];
  idlraw=(struct RawIDLData *) argv[2];

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
  
  idlraw->revision.major=raw.revision.major;
  idlraw->revision.minor=raw.revision.minor;
  idlraw->thr=raw.thr;

  for (n=0;n<prm.nrang;n++) {
    idlraw->pwr0[n]=raw.pwr0[n];
    for (x=0;x<prm.mplgs;x++) {
      idlraw->acfd[x*MAX_RANGE+n]=raw.acfd[n][x][0];
      idlraw->acfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n]=raw.acfd[n][x][1];
      if (prm.xcf !=0) {
        idlraw->xcfd[x*MAX_RANGE+n]=raw.xcfd[n][x][0];
        idlraw->xcfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n]=raw.xcfd[n][x][1];
      }
    }
  } 
  return s;
}




int32 OldRawIDLSeek(int argc,char *argv[]) {

  struct OldRawIDLFp *idlrawfp;
  struct OldRawFp rawfp;

  IDL_FILE_STAT stat;
  FILE *fp;
  int s;

  int32 yr,mo,dy,hr,mt;
  double sc;
  double *atme;

  idlrawfp=(struct OldRawIDLFp *) argv[0];
  
  IDL_FileStat(idlrawfp->rawunit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  fflush(fp);

  rawfp.rawfp=fileno(fp);
  rawfp.inxfp=0;

  yr=*( (int32 *) argv[1]);
  mo=*( (int32 *) argv[2]);
  dy=*( (int32 *) argv[3]);
  hr=*( (int32 *) argv[4]);
  mt=*( (int32 *) argv[5]);
  sc=*( (double *) argv[6]);
  atme=(double *) argv[7];
 
  OldRawIDLToRawFp(idlrawfp,&rawfp);

  s=OldRawSeek(&rawfp,yr,mo,dy,hr,mt,sc,atme);

  OldRawRawFpToIDL(&rawfp,idlrawfp);

  return s;
}




  
int32 OldRawIDLWrite(int argc,char *argv[]) {

  int n,x;
  struct RadarIDLParm *idlprm;
  struct RawIDLData *idlraw;
  struct RadarParm prm;
  struct RawData raw;
  int32 recnum;
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

  fflush(fp);
  
  if (fp==NULL) return -1;

  /* get the structure pointers */

  idlprm=(struct RadarIDLParm *) argv[1];
  idlraw=(struct RawIDLData *) argv[2];
  recnum=*((int32 *) argv[3]);
  memset(&prm,0,sizeof(struct RadarParm));
  memset(&raw,0,sizeof(struct RawData));

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

  raw.revision.major=idlraw->revision.major;
  raw.revision.minor=idlraw->revision.minor;
  raw.thr=idlraw->thr;

  for (n=0;n<prm.nrang;n++) {
    raw.pwr0[n]=idlraw->pwr0[n];

    for (x=0;x<prm.mplgs;x++) {
      raw.acfd[n][x][0]=idlraw->acfd[x*MAX_RANGE+n];
      raw.acfd[n][x][1]=idlraw->acfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n];
      if (prm.xcf !=0) {
        raw.xcfd[n][x][0]=idlraw->xcfd[x*MAX_RANGE+n];
        raw.xcfd[n][x][1]=idlraw->xcfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n];
      }
    }

  }
 
  s=OldRawFwrite(fp,"rawwrite",&prm,&raw,recnum,NULL);

  return s;

}



int32 OldRawIDLWriteHeader(int argc,char *argv[]) {

  struct RadarIDLParm *idlprm;
  struct RawIDLData *idlraw;

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

  fflush(fp);
  
  if (fp==NULL) return -1;

  idlprm=(struct RadarIDLParm *) argv[1];
  idlraw=(struct RawIDLData *) argv[2];

  sprintf(vstring,"%d.%.3d",idlraw->revision.major,idlraw->revision.minor);
  s=OldRawHeaderFwrite(fp,"rawwrite",vstring,idlraw->thr,"IDL output");

  return s;
}

