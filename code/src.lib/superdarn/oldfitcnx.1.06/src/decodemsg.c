/* decodemsg.c
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
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cnxstr.h"

/*
 $Log: decodemsg.c,v $
 Revision 1.4  2004/06/16 21:45:02  barnes
 Added missing header.

 Revision 1.3  2004/05/05 15:48:04  barnes
 Added missing header.

 Revision 1.2  2004/05/04 18:33:07  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/03/15 14:02:27  barnes
 Initial revision

*/

#define read_bit(t,n) (((t)[n/8] & (1<<(n%8))) !=0)
#define ORIG_MAX_RANGE 75

int OldFitCnxDecodeIPMsg(struct RadarParm *prm,struct FitData *fit,
                         unsigned char *buffer) {
  unsigned char *buf;
  unsigned char *gsct;
  unsigned char *dflg;
  struct radar_parms oldprm;

  int pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
  
  int i;
  int rng_num=0;
  int index;

  if (buffer[0] !='d') return 0;
  buf=buffer+1;

  ConvertBlock(buf,pat);
  memcpy(&oldprm,buf,sizeof(struct radar_parms));
  buf+=sizeof(struct radar_parms);

  gsct=buf;
  dflg=buf+10;
  buf+=20;

  prm->revision.major=oldprm.REV.MAJOR;
  prm->revision.minor=oldprm.REV.MINOR;
  prm->cp=oldprm.CP;
  prm->stid=oldprm.ST_ID;
  prm->time.yr=oldprm.YEAR;
  prm->time.mo=oldprm.MONTH;
  prm->time.dy=oldprm.DAY;
  prm->time.hr=oldprm.HOUR;
  prm->time.mt=oldprm.MINUT;
  prm->time.sc=oldprm.SEC;
  prm->time.us=0;

  prm->txpow=oldprm.TXPOW;
  prm->nave=oldprm.NAVE;
  prm->atten=oldprm.ATTEN;
  prm->lagfr=oldprm.LAGFR;
  prm->smsep=oldprm.SMSEP;
  prm->ercod=oldprm.ERCOD;
  prm->stat.agc=oldprm.AGC_STAT;
  prm->stat.lopwr=oldprm.LOPWR_STAT;
  prm->noise.search=oldprm.NOISE;
  prm->noise.mean=oldprm.NOISE_MEAN;
      
  prm->channel=oldprm.CHANNEL;
  prm->bmnum=oldprm.BMNUM;
  prm->scan=oldprm.SCAN;
  
  prm->offset=oldprm.usr_resL1; 
  prm->rxrise=oldprm.RXRISE;
  prm->intt.sc=oldprm.INTT;
  prm->intt.us=0;
  prm->txpl=oldprm.TXPL;
    
  prm->mpinc=oldprm.MPINC;
  prm->mppul=oldprm.MPPUL;
  prm->mplgs=oldprm.MPLGS;
  prm->nrang=oldprm.NRANG;
  prm->frang=oldprm.FRANG;
  prm->rsep=oldprm.RSEP;
  prm->xcf=oldprm.XCF; 
  prm->tfreq=oldprm.TFREQ;
  prm->mxpwr=oldprm.MXPWR;
  prm->lvmax=oldprm.LVMAX;   

  for (i=0;i<MAX_RANGE;i++) {
    fit->rng[i].qflg=0;
    fit->rng[i].gsct=0;
    fit->rng[i].v=0;
    fit->rng[i].v_err=0;
    fit->rng[i].p_0=0;
    fit->rng[i].p_l=0;
    fit->rng[i].p_l_err=0;
    fit->rng[i].p_s=0;
    fit->rng[i].p_s_err=0;
    fit->rng[i].w_l=0;
    fit->rng[i].w_l_err=0;
    fit->rng[i].w_s=0;
    fit->rng[i].w_s_err=0;
    fit->rng[i].phi0=0;
    fit->rng[i].phi0_err=0;
    fit->rng[i].sdev_l=0;
    fit->rng[i].sdev_s=0;
    fit->rng[i].sdev_phi=0;
    fit->rng[i].nump=0;
  }
  for (i=0;i<ORIG_MAX_RANGE;i++) {
    if (read_bit(dflg,i) !=0) {
      fit->rng[i].qflg=1;
      rng_num++;
    }
  }

  if (rng_num == 0) return 1;
  index=0;
  for (i=0;i<ORIG_MAX_RANGE;i++) {
    if (fit->rng[i].qflg !=0) {
      if (read_bit(gsct,i) !=0) fit->rng[i].gsct=1;
      ConvertToDouble(buf+8*index,&fit->rng[i].p_l);
      ConvertToDouble(buf+8*(index+rng_num),&fit->rng[i].v);
      ConvertToDouble(buf+8*(index+2*rng_num),&fit->rng[i].w_l);
      index++;
    }
  } 
  return 1;
}







