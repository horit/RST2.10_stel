/* fit.c
   =====
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
#include <sys/stat.h>
#include <math.h>
#include "rtime.h"
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitindex.h"
#include "fitseek.h"
#include "tplot.h"

/*
 $Log: fit.c,v $
 Revision 1.2  2004/10/28 14:02:33  barnes
 Added missing header.

 Revision 1.1  2004/05/10 21:59:33  barnes
 Initial revision

*/

double fit_find(FILE *fp,struct RadarParm *prm,
                struct FitData *fit,double sdate,
                double stime,struct FitIndex *inx) {
  int status;
  int yr,mo,dy,hr,mt;
  double sc;
  double atime;
  if (FitFread(fp,prm,fit)==-1) return -1;
  atime=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                          prm->time.hr,prm->time.mt,
                          prm->time.sc+prm->time.us/1.0e6);
       	                 

  if ((stime==-1) && (sdate==-1)) return atime;
  if (stime==-1) stime= ( (int) atime % (24*3600));
  if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
  else stime+=sdate;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  if (atime>=stime) return stime;
  status=FitFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx);
  if (status==-1) return -1;
  if (FitFread(fp,prm,fit)==-1) return -1;
  return stime;
}

double fit_scan(double ctime,FILE *fp,int rflg,
                struct RadarParm *prm,struct FitData *fit,
                int bmnum,int chnum,int cpid,int sflg,int scan) {

 
  double atime;
  int flg=1;
  if (rflg) {
    if (FitFread(fp,prm,fit)==-1) return -1;
  }
  while (1) {
   
    atime=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                          prm->time.hr,prm->time.mt,
                          prm->time.sc+prm->time.us/1.0e6);
  
    if (atime<ctime) flg=0;
    if ((bmnum !=-1) && (prm->bmnum !=bmnum)) flg=0;
    if ((sflg) && (prm->scan !=scan)) flg=0;
    if (chnum !=-1) {
       if ((chnum==2) && (prm->channel!=2)) flg=0;
       if ((chnum==1) && (prm->channel==2)) flg=0;
    }
    if ((cpid !=-1) && (prm->cp !=cpid)) flg=0;
    if (flg==1) break;
    if (FitFread(fp,prm,fit)==-1) return -1;
    flg=1;
  }
  
  return atime;
}

void fit_tplot(struct RadarParm *prm,struct FitData *fit,struct tplot *tptr) {
  int i;

  tptr->bmnum=prm->bmnum;
  tptr->channel=prm->channel;
  tptr->cpid=prm->cp;
  tptr->scan=prm->scan;
  tptr->nrang=prm->nrang;
  tptr->nave=prm->nave;
  tptr->intt.sc=prm->intt.sc;
  tptr->intt.us=prm->intt.us;
  tptr->frang=prm->frang;
  tptr->rsep=prm->rsep;
  tptr->rxrise=prm->rxrise;
  tptr->noise=prm->noise.search;
  tptr->tfreq=prm->tfreq;
  tptr->atten=prm->atten;

  for (i=0;i<MAX_RANGE;i++) {
    tptr->qflg[i]=0;
    tptr->gsct[i]=0;
  }
  for (i=0;i<prm->nrang;i++) {
    tptr->qflg[i]=fit->rng[i].qflg;
    tptr->gsct[i]=fit->rng[i].gsct;
    tptr->p_l[i]=fit->rng[i].p_l;
    tptr->v[i]=fit->rng[i].v;
    tptr->w_l[i]=fit->rng[i].w_l;
    tptr->p_l_e[i]=fit->rng[i].p_l_err;
    tptr->v_e[i]=fit->rng[i].v_err;
    tptr->w_l_e[i]=fit->rng[i].w_l_err;
  }
}
