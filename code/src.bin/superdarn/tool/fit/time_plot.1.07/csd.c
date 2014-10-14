/* csd.c
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
#include "csddata.h"

#include "csdread.h"
#include "tplot.h"

/*
 $Log: csd.c,v $
 Revision 1.2  2004/05/10 21:59:33  barnes
 Removed redundant code.

 Revision 1.1  2003/05/02 14:42:46  barnes
 Initial revision

*/

double csd_find(struct CSDfp *csdfp,struct CSDdata *csd,double sdate,
                double stime) {
  int status;
  int yr,mo,dy,hr,mt;
  double sc;
  double atime;

  do {
    status=CSDRead(csdfp,csd);
    if (status==-1) break;
  } while (status !=0);
  if (status==-1) return -1;
  
  atime=csd->time;

  if ((stime==-1) && (sdate==-1)) return atime;
  if (stime==-1) stime= ( (int) atime % (24*3600));
  if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
  else stime+=sdate;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  if (atime>=stime) return stime;
  status=CSDSeek(csdfp,yr,mo,dy,hr,mt,sc,NULL);
  
  if (status==-1) return -1;
   
  do {
    status=CSDRead(csdfp,csd);
    if (status==-1) break;
  } while (status !=0);


  if (status==-1) return -1;
  return stime;
}

double csd_scan(double ctime,struct CSDfp *csdfp,int rflg,
                struct CSDdata *csd,
                int bmnum,int chnum,int cpid,int sflg,int scan) {

  int s=0;
  double atime;
  int flg=1;
  if (rflg) {
    do {
      s=CSDRead(csdfp,csd);
      if (s==-1) break;
    } while (s !=0);
    if (s==-1) return -1;
  }

  while (1) {   
    atime=csd->time;

    if ((bmnum !=-1) && (csd->bmnum !=bmnum)) flg=0;
    if ((sflg) && (csd->scan !=scan)) flg=0;
    if (chnum !=-1) {
       if ((chnum==2) && (csd->channel!=2)) flg=0;
       if ((chnum==1) && (csd->channel==2)) flg=0;
    }
    if ((cpid !=-1) && (csd->cp !=cpid)) flg=0;
    if (flg==1) break;
    do {
      s=CSDRead(csdfp,csd);
      if (s==-1) break;
    } while (s !=0);
    if (s==-1) return -1;
    flg=1;
  }

  
  return atime;
}

void csd_tplot(struct CSDdata *csd,struct tplot *tptr) {
  int i;
  tptr->bmnum=csd->bmnum;
  tptr->channel=csd->channel;
  tptr->cpid=csd->cp;
  tptr->scan=csd->scan;
  tptr->nrang=MAX_RANGE;
  tptr->nave=csd->nave;
  tptr->intt.sc=csd->intt.sc;
  tptr->intt.us=csd->intt.us;
  tptr->frang=csd->frang;
  tptr->rsep=csd->rsep;
  tptr->rxrise=csd->rxrise;
  tptr->noise=csd->noise;
  tptr->tfreq=csd->tfreq;
  tptr->atten=csd->atten;

  for (i=0;i<MAX_RANGE;i++) {
    tptr->qflg[i]=0;
    tptr->gsct[i]=0;
  }

  for (i=0;i<csd->nrang;i++) {
    
    tptr->qflg[i]=csd->store[i];
    tptr->gsct[i]=csd->store[MAX_RANGE+i];
    if (csd->stored & 0x01) tptr->p_l[i]=csd->pmax*
                                         csd->store[2*MAX_RANGE+i]/256.0;
    if (csd->stored & 0x02) tptr->v[i]=csd->vmax*
                                       (csd->store[3*MAX_RANGE+i]-128)/128.0;
    if (csd->stored & 0x04) tptr->w_l[i]=csd->wmax*
                                         csd->store[4*MAX_RANGE+i]/256.0;
    tptr->p_l_e[i]=0;
    tptr->v_e[i]=0;
    tptr->w_l_e[i]=0;
   
  }
}
