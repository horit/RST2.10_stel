/* gtable.c
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
#include <math.h>
#include "limit.h"
#include "radar.h"
#include "rpos.h" 
#include "invmag.h"

#include "rmath.h"
#include "rtime.h"
#include "scandata.h"
#include "gtable.h"

double v_e_min=100;
double p_l_e_min=1;
double w_l_e_min=1;

/*
 $Log: gtable.c,v $
 Revision 1.4  2006/04/28 21:06:15  barnes
 Fixed bug in dealing with missing beams in scans.

 Revision 1.3  2006/03/14 23:19:16  barnes
 Fixed bug in confusing maxrange with maxbeam.

 Revision 1.2  2006/03/14 22:25:23  barnes
 Modification to handle radars with extra beams.

 Revision 1.1  2004/08/20 13:19:21  barnes
 Initial revision

*/

int GridTableZero(int pnum,struct GridPnt *ptr) {

  int i;
  
  for (i=0;i<pnum;i++) {
      ptr[i].azm=0;
      ptr[i].vel.median=0;
      ptr[i].vel.sd=0;
      ptr[i].pwr.median=0;
      ptr[i].pwr.sd=0;
      ptr[i].wdt.median=0;
      ptr[i].wdt.sd=0;
      ptr[i].cnt=0;
  }
  return 0;
}

int GridTableSort(const void *a,const void *b) {
  struct GridPnt *ap;
  struct GridPnt *bp;
  ap=(struct GridPnt *) a;
  bp=(struct GridPnt *) b;
 
  if (ap->ref<bp->ref) return -1;
  if (ap->ref>bp->ref) return 1;
  return 0;
}

struct GridPnt *GridTableMake(struct RadarSite *pos,double tval,
                       double alt,
                       int *frang,int *rsep,int *rxrise,
                       int  *ltab,int *pnum,
                       double *azmt,double *itab) {

  struct GridPnt *ptr=NULL;
  int yr,mo,dy,hr,mt;
  double sc;
  double lat,lon,azm,geoazm,elv,lspc;
  int bm,rn,i=0,j,k=0;
  double velco;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

  ptr=malloc(sizeof(struct GridPnt)*
	     (pos->maxbeam*pos->maxrange)); /* space for everything */

 
  velco=(2*PI/86400.0)*6356.779*1000*cos(PI*pos->geolat/180.0);
 
  for (bm=0;bm<pos->maxbeam;bm++) {
  
    if ((frang[bm] !=-1) && (rsep[bm] !=0)) {
      for (rn=0;rn<pos->maxrange;rn++) {
         
         RPosRngBmAzmElv(bm,rn,yr,pos,
                 frang[bm],rsep[bm],rxrise[bm],
                 alt,&geoazm,&elv);

         

         RPosInvMag(bm,rn,yr,pos,
                 frang[bm],rsep[bm],rxrise[bm],
                 alt,&lat,&lon,&azm);
        

         if (lon<0) lon+=360;
         if ((lat>0) && (lat<50)) lat=50;
         if ((lat<0) && (lat>-50)) lat=-50;

         /* fix lat and lon to grid spacing */
         
         if (lat>0) ptr[i].mlat=(int) (lat)+0.5;
         else ptr[i].mlat=(int) (lat)-0.5;

         lspc=((int) (360*cos(fabs(ptr[i].mlat)*PI/180)+0.5))/(360.0);
  
         ptr[i].mlon=((int) (lon*lspc)+0.5)/lspc;

         if (lat>0)      
           ptr[i].ref=1000*( (int) lat-50)+( (int) (lon*lspc) ); 
         else ptr[i].ref=-1000*( (int) -lat-50)-( (int) (lon*lspc) ); 
                    /* grid reference */   
         ptr[i].cnt=bm*MAX_RANGE+rn;
         azmt[bm*MAX_RANGE+rn]=azm;
         itab[bm*MAX_RANGE+rn]=velco*cos(PI*(90+geoazm)/180.0);
        

         i++;
      }
    }
  }
  /* sort the grid table into ascending order */

  qsort(ptr,i,sizeof(struct GridPnt),GridTableSort);


  /* now sift through and remove redundancy */


  ltab[ptr[0].cnt]=0; 
  ptr[0].npix=1;
  for (j=1;j<i;j++) {
    if (ptr[k].ref==ptr[j].ref) { /* repeat */
      ltab[ptr[j].cnt]=k;
      ptr[k].npix++;
    } else {
      k++;
      memcpy(&ptr[k],&ptr[j],sizeof(struct GridPnt));
      ltab[ptr[j].cnt]=k;
      ptr[k].npix=1;
    }    
  }

  *pnum=k+1;
  ptr=realloc(ptr,sizeof(struct GridPnt)*(k+1));
  return ptr;  
}

int GridTableTest(struct GridTable *ptr,struct RadarScan *scan,int tlen) {

  double tm;
  int i;
  tm=(scan->st_time+scan->ed_time)/2.0;
  if (ptr->st_time==-1) return 0;

  if (tm>ptr->ed_time) {
    /* average the grid */
   
    ptr->npnt=0;
    ptr->freq=ptr->freq/ptr->nscan;
    for (i=0;i<ptr->pnum;i++) {
      if (ptr->pnt[i].cnt==0) continue;
      if (ptr->pnt[i].cnt<=(0.25*ptr->nscan*ptr->pnt[i].npix)) {
        ptr->pnt[i].cnt=0;
        continue;
      }
      ptr->npnt++;
      ptr->pnt[i].azm=ptr->pnt[i].azm/ptr->pnt[i].cnt;
      ptr->pnt[i].vel.median=ptr->pnt[i].vel.median/ptr->pnt[i].vel.sd;
      ptr->pnt[i].wdt.median=ptr->pnt[i].wdt.median/ptr->pnt[i].wdt.sd;
      ptr->pnt[i].pwr.median=ptr->pnt[i].pwr.median/ptr->pnt[i].pwr.sd;
      
     
      ptr->pnt[i].vel.sd=1/sqrt(ptr->pnt[i].vel.sd);
      ptr->pnt[i].wdt.sd=1/sqrt(ptr->pnt[i].wdt.sd);
      ptr->pnt[i].pwr.sd=1/sqrt(ptr->pnt[i].pwr.sd);

    }
    ptr->status=1;
    return 1;
  }
  return 0;
}



void GridTableMap(struct GridTable *ptr,struct RadarScan *scan,
                struct RadarSite *pos,int tlen,int iflg,double alt) {
  int gflg,i,j,l,fcnt=0;
  double freq=0,noise=0;
  double variance=0;
  double tm;

  tm=(scan->st_time+scan->ed_time)/2.0;
   
  /* test to see if we need to remake the grid */
 
  for (gflg=0;gflg<scan->num;gflg++) {
    if (scan->bm[gflg].bm !=-1) {  
      if ((ptr->frang[gflg] !=scan->bm[gflg].frang) ||
          (ptr->rsep[gflg] !=scan->bm[gflg].rsep)) break;
    }
  }
 

  if ((ptr->st_time==-1) || (gflg<pos->maxbeam)) {

    /* generate grid */
  
    ptr->st_id=scan->stid;
    for (i=0;i<scan->num;i++) {
      if ((scan->bm[i].bm !=-1) && (scan->bm[i].frang !=-1)) 
         ptr->frang[i]=scan->bm[i].frang;
      else ptr->frang[i]=-1;
      if ((scan->bm[i].bm !=-1) && (scan->bm[i].rsep !=0)) 
         ptr->rsep[i]=scan->bm[i].rsep;
      else ptr->rsep[i]=0;
    }
    ptr->nscan=0;
    if (ptr->pnt !=NULL) free (ptr->pnt);
    ptr->pnt=GridTableMake(pos,scan->st_time,alt,
                         ptr->frang,ptr->rsep,ptr->rxrise,
                         ptr->ltable,&ptr->pnum,
                         ptr->azm,ptr->itab);
    ptr->st_time=tlen*( (int) (tm/tlen)); 
    ptr->ed_time=ptr->st_time+tlen;
    GridTableZero(ptr->pnum,ptr->pnt);
  }
 

  /* if last grid was completed then reset the arrays */
  if (ptr->status==1) {
     ptr->status=0;
     ptr->freq=0;
     ptr->nscan=0;
     GridTableZero(ptr->pnum,ptr->pnt);
     ptr->st_time=tlen*( (int) (tm/tlen)); 
     ptr->ed_time=ptr->st_time+tlen;
  }

  /* okay map the pixels */

  for (i=0;i<scan->num;i++) {
    if (scan->bm[i].bm==-1) continue;
    
    ptr->prog_id=scan->bm[i].cpid;
    freq+=scan->bm[i].freq;
    noise+=scan->bm[i].noise;
   
    fcnt++;
    for (j=0;j<pos->maxrange;j++) {
      if (scan->bm[i].sct[j] !=0) {
        double v_e; 
        double p_l_e;
        double w_l_e;
           
        v_e=scan->bm[i].rng[j].v_e;
        p_l_e=scan->bm[i].rng[j].p_l_e;
        w_l_e=scan->bm[i].rng[j].w_l_e;
   
        /* apply floor on uncertainties */
      

        if (v_e<v_e_min) v_e=v_e_min;
        if (p_l_e<p_l_e_min) p_l_e=p_l_e_min;
        if (w_l_e<w_l_e_min) w_l_e=w_l_e_min;
 
        l=ptr->ltable[i*MAX_RANGE+j];
       
         
        ptr->pnt[l].azm+=ptr->azm[i*MAX_RANGE+j];

	  
        if (iflg !=0) ptr->pnt[l].vel.median+=
                        -(scan->bm[i].rng[j].v+ptr->itab[i*MAX_RANGE+j])*
                         1/(v_e*v_e);
        else 
          ptr->pnt[l].vel.median+=-scan->bm[i].rng[j].v*1/(v_e*v_e);

        ptr->pnt[l].pwr.median+=scan->bm[i].rng[j].p_l*1/(p_l_e*p_l_e);
        ptr->pnt[l].wdt.median+=scan->bm[i].rng[j].w_l*1/(w_l_e*w_l_e);

        ptr->pnt[l].vel.sd+=1/(v_e*v_e);
        ptr->pnt[l].pwr.sd+=1/(p_l_e*p_l_e);
        ptr->pnt[l].wdt.sd+=1/(w_l_e*w_l_e);
        ptr->pnt[l].cnt++;
      }
    }
  }

  freq=freq/fcnt;
  noise=noise/fcnt;
  

  /* find variance of the noise */
  
  for (i=0;i<scan->num;i++) {
    if (scan->bm[i].bm==-1) continue;
    variance+=(scan->bm[i].noise-noise)*(scan->bm[i].noise-noise);
  }

  ptr->noise.mean=noise;
  ptr->noise.sd=sqrt(variance/fcnt);
  ptr->freq=freq;
  ptr->nscan++;
}














