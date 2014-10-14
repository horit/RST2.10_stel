/* cfit.c
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
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "cfitdata.h"

#include "cfitread.h"
#include "tplot.h"

/*
 $Log: cfit.c,v $
 Revision 1.2  2004/05/10 21:57:49  barnes
 Removed redundant code.

 Revision 1.1  2003/05/02 14:42:46  barnes
 Initial revision

*/

double cfit_find(struct CFitfp *cfitfp,struct CFitdata *cfit,double sdate,
                double stime) {
  int status;
  int yr,mo,dy,hr,mt;
  double sc;
  double atime;

  do {
    status=CFitRead(cfitfp,cfit);
    if (status==-1) break;
  } while (status !=0);
  if (status==-1) return -1;
  
  atime=cfit->time;

  if ((stime==-1) && (sdate==-1)) return atime;
  if (stime==-1) stime= ( (int) atime % (24*3600));
  if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
  else stime+=sdate;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  if (atime>=stime) return stime;

  status=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL);
  if (status==-1) return -1;
  
  do {
    status=CFitRead(cfitfp,cfit);
    if (status==-1) break;
  } while (status !=0);
  if (status==-1) return -1;
  return stime;
}

double cfit_scan(double ctime,struct CFitfp *cfitfp,int rflg,
                struct CFitdata *cfit,
                int bmnum,int chnum,int cpid,int sflg,int scan) {

  int s=0;
  double atime;
  int flg=1;
  if (rflg) {
    do {
      s=CFitRead(cfitfp,cfit);
      if (s==-1) break;
    } while (s !=0);
    if (s==-1) return -1;
  }
  while (1) {   
    atime=cfit->time;
    if ((bmnum !=-1) && (cfit->bmnum !=bmnum)) flg=0;
    if ((sflg) && (cfit->scan !=scan)) flg=0;
    if (chnum !=-1) {
       if ((chnum==2) && (cfit->channel!=2)) flg=0;
       if ((chnum==1) && (cfit->channel==2)) flg=0;
    }
    if ((cpid !=-1) && (cfit->cp !=cpid)) flg=0;
    if (flg==1) break;
    do {
      s=CFitRead(cfitfp,cfit);
      if (s==-1) break;
    } while (s !=0);
    if (s==-1) return -1;
    flg=1;
  }
  
  return atime;
}

void cfit_tplot(struct CFitdata *cfit,struct tplot *tptr) {
  int i;
  int rng;
  tptr->bmnum=cfit->bmnum;
  tptr->channel=cfit->channel;
  tptr->cpid=cfit->cp;
  tptr->scan=cfit->scan;
  tptr->nrang=cfit->nrang;
  tptr->nave=cfit->nave;
  tptr->intt.sc=cfit->intt.sc;
  tptr->intt.us=cfit->intt.us;
  tptr->frang=cfit->frang;
  tptr->rsep=cfit->rsep;
  tptr->rxrise=cfit->rxrise;
  tptr->noise=cfit->noise;
  tptr->tfreq=cfit->tfreq;
  tptr->atten=cfit->atten;
  
  for (i=0;i<MAX_RANGE;i++) {
    tptr->qflg[i]=0;
    tptr->gsct[i]=0;
  }
  
  for (i=0;i<cfit->num;i++) {
    rng=cfit->rng[i];
    tptr->qflg[rng]=1;
    tptr->gsct[rng]=cfit->data[i].gsct;
    tptr->v[rng]=cfit->data[i].v;
    tptr->p_l[rng]=cfit->data[i].p_l;
    tptr->w_l[rng]=cfit->data[i].w_l;
    tptr->v_e[rng]=cfit->data[i].v;
    tptr->p_l_e[rng]=cfit->data[i].p_l_e;
    tptr->w_l_e[rng]=cfit->data[i].w_l_e;
  }

}
