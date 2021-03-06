/* filter.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "scandata.h"
#include "filter.h"

/*
 $Log: filter.c,v $
 Revision 1.6  2006/03/14 22:59:49  barnes
 Fixed bug in undercounting the number of beams.

 Revision 1.5  2006/03/14 22:12:12  barnes
 Modification to deal with radars with extra beams.

 Revision 1.4  2004/07/13 22:34:10  barnes
 Removed redundant headers.

 Revision 1.3  2004/05/04 18:24:36  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2003/09/13 22:37:47  barnes
 Modification to use the new data structures.

 Revision 1.1  2003/08/05 15:30:02  barnes
 Initial revision

*/

int FilterCmpVel(const void *x,const void *y) {
  struct RadarCell *a,*b;
  a=*((struct RadarCell **) x);
  b=*((struct RadarCell **) y);
 
  if (a->v<b->v) return -1;
  if (a->v>b->v) return 1;
  return 0;
}

int FilterCmpPwr(const void *x,const void *y) {
  struct RadarCell *a,*b;
  a=*((struct RadarCell **) x);
  b=*((struct RadarCell **) y);
  if (a->p_l<b->p_l) return -1;
  if (a->p_l>b->p_l) return 1;
  return 0;
}

int FilterCmpWdt(const void *x,const void *y) {
  struct RadarCell *a,*b;
  a=*((struct RadarCell **) x);
  b=*((struct RadarCell **) y);
  if (a->w_l<b->w_l) return -1;
  if (a->w_l>b->w_l) return 1;
  return 0;
}



int FilterRadarScan(int mode,int depth,int inx,struct RadarScan *src,
                    struct RadarScan *dst,int prm) {

  
  int thresh[2] = {12,24};
  double us;
  int bmin,bmax,rmin,rmax,bbox,rbox;
  int i,w,n,x,y,z,c,f,bm,rng,mxbm=1;
  struct RadarBeam *b;
  struct RadarBeam **tmp;
  struct RadarBeam **bmptr[MAX_BEAM][FILTER_DEPTH];
  int bmcnt[MAX_BEAM][FILTER_DEPTH];

  int weight[FILTER_WIDTH][FILTER_HEIGHT][FILTER_DEPTH];
  struct RadarCell **cell=NULL;
  struct RadarCell **median=NULL;
  int cnum=0,cnt=0;
  int maxbeam=16;
  int maxrange=75;
  double variance,mean,sigma;  

  if (depth>FILTER_DEPTH) depth=FILTER_DEPTH;

  for (i=0;i<depth;i++) {
    for (n=0;n<src[i].num;n++) {
      bm=src[i].bm[n].bm;
      if (bm>maxbeam) maxbeam=bm;
      rng=src[i].bm[n].nrang;
      if (rng>maxrange) maxrange=rng;

    }
  }
  maxbeam++;
  for (z=0;z<depth;z++) {
    for (y=0;y<FILTER_HEIGHT;y++) {
      for (x=0;x<FILTER_WIDTH;x++) {
          f=(x !=0) & (x !=FILTER_WIDTH-1) & 
            (y !=0) & (y !=FILTER_HEIGHT-1);
          w=1+f;
          f=(z !=0) & (z !=FILTER_DEPTH-1);
          weight[x][y][z]=w*(1+f);
       
      }   
    }
  }

  RadarScanReset(dst);
  dst->bm=malloc(maxbeam*sizeof(struct RadarBeam));
  for (bm=0;bm<maxbeam;bm++) {
     dst->bm[bm].bm=-1;
     for (y=0;y<maxrange;y++) dst->bm[bm].sct[y]=0;
     for (z=0;z<depth;z++)  {
       bmptr[bm][z]=NULL;
       bmcnt[bm][z]=0;
     }
  }
  
  for (z=0;z<depth;z++) {
    i=(inx-(depth-1)+z);
    if (i<0) i+=depth;
    for (n=0;n<src[i].num;n++) {
      bm=src[i].bm[n].bm;
      c=bmcnt[bm][z];
      if (bmptr[bm][z]==NULL) bmptr[bm][z]=malloc(sizeof(struct RadarBeam **));
      else {
        tmp=realloc(bmptr[bm][z],sizeof(struct RadarBeam **)*(c+1));
        if (tmp==NULL) break;
        bmptr[bm][z]=tmp;
      }
      bmptr[bm][z][c]=&src[i].bm[n];
      bmcnt[bm][z]++;
      if (bmcnt[bm][z]>mxbm) mxbm=bmcnt[bm][z];
    }
    if (n !=src[i].num) break;
  }

  if (z !=depth) {
    for (z=0;z<depth;z++) {
      for (bm=0;bm<maxbeam;bm++) if (bmptr[bm][z] !=NULL) free(bmptr[bm][z]);
    }
    return -1;
  }

  dst->num=maxbeam;
  i=inx-1;
  if (i<0) i+=depth;
  dst->stid=src[i].stid;
  dst->version.major=src[i].version.major;
  dst->version.minor=src[i].version.minor;
  dst->st_time=src[i].st_time;
  dst->ed_time=src[i].ed_time;
  
  if ((mode & 4)==4) {
    for (bm=0;bm<maxbeam;bm++) {
      if (bmcnt[bm][depth/2]==0) continue;
      b=bmptr[bm][depth/2][0];
      dst->bm[bm].bm=bm;
      dst->bm[bm].cpid=b->cpid;
      dst->bm[bm].time=b->time;
      dst->bm[bm].intt=b->intt;
      dst->bm[bm].nave=b->nave;
      dst->bm[bm].frang=b->frang;
      dst->bm[bm].rsep=b->rsep;
      dst->bm[bm].rxrise=b->rxrise;
      dst->bm[bm].freq=b->freq;
      dst->bm[bm].noise=b->noise;
      dst->bm[bm].atten=b->atten;
      dst->bm[bm].channel=b->channel;
      dst->bm[bm].nrang=b->nrang;    
    }
  } else {
    for (n=0;n<maxbeam;n++) {
      dst->bm[n].cpid=-1;
      dst->bm[n].time=0;
      dst->bm[n].intt.sc=0;
      dst->bm[n].intt.us=0;
      dst->bm[n].frang=0;
      dst->bm[n].rsep=0;
      dst->bm[n].rxrise=0;
      dst->bm[n].freq=0;
      dst->bm[n].noise=0;
      dst->bm[n].atten=0;
      dst->bm[n].channel=-1;
      dst->bm[n].nrang=-1;
    }
    for (z=0;z<depth;z++) {
      for (bm=0;bm<maxbeam;bm++) {
        if (bmcnt[bm][z]==0) continue;
        dst->bm[bm].bm=bm;
        for (c=0;c<bmcnt[bm][z];c++) {
          b=bmptr[bm][z][c];
          if (dst->bm[bm].cpid==-1) dst->bm[bm].cpid=b->cpid;
          dst->bm[bm].time+=b->time;
          dst->bm[bm].intt.sc+=b->intt.sc;
          dst->bm[bm].intt.us+=b->intt.us;
          if (dst->bm[bm].intt.us>1e6) {
            dst->bm[bm].intt.sc++;
            dst->bm[bm].intt.us-=1e6;
	  }
          dst->bm[bm].nave+=b->nave;
          dst->bm[bm].frang+=b->frang;
          dst->bm[bm].rsep+=b->rsep;
          dst->bm[bm].rxrise+=b->rxrise;
          dst->bm[bm].freq+=b->freq;
          dst->bm[bm].noise+=b->noise;
          dst->bm[bm].atten+=b->atten;
          if (dst->bm[bm].channel==0) dst->bm[bm].channel=b->channel;
          if (dst->bm[bm].nrang==-1) dst->bm[bm].nrang=b->nrang;    
	}
      }
    }
    for (n=0;n<maxbeam;n++) {
      cnt=0;
      for (z=0;z<depth;z++) cnt+=bmcnt[n][z];
      if (cnt==0) continue; 
      dst->bm[n].time=dst->bm[n].time/cnt;
      dst->bm[n].intt.us=dst->bm[n].intt.us/cnt;
      us=(1e6*dst->bm[n].intt.sc)/cnt;
      dst->bm[n].intt.sc=dst->bm[n].intt.sc/cnt;
      us=us-1e6*dst->bm[n].intt.sc;
      dst->bm[n].intt.us+=us;
      dst->bm[n].nave=dst->bm[n].nave/cnt;
      dst->bm[n].frang=dst->bm[n].frang/cnt;
      dst->bm[n].rsep=dst->bm[n].rsep/cnt;
      dst->bm[n].rxrise=dst->bm[n].rxrise/cnt;
      dst->bm[n].freq=dst->bm[n].freq/cnt;
      dst->bm[n].noise=dst->bm[n].noise/cnt;
      dst->bm[n].atten=dst->bm[n].atten/cnt;
    }
  }

  cell=malloc(sizeof(struct RadarCell *)*FILTER_WIDTH*
              FILTER_HEIGHT*depth*mxbm);
  median=malloc(sizeof(struct RadarCell *)*FILTER_WIDTH*
              FILTER_HEIGHT*depth*mxbm);


  for (bm=0;bm<maxbeam;bm++) {
    for (rng=0;rng<maxrange;rng++) {
      cnum=0;
      bmin=bm-FILTER_WIDTH/2;
      bbox=bm-FILTER_WIDTH/2;
      bmax=bm+FILTER_WIDTH/2;
      rmin=rng-FILTER_HEIGHT/2;
      rbox=rng-FILTER_HEIGHT/2;
      rmax=rng+FILTER_HEIGHT/2;

      if (bmin<0) bmin=0;
      if (bmax>=maxbeam) bmax=maxbeam-1;
      if (rmin<0) rmin=0;
      if (rmax>=maxrange) rmax=maxrange-1;
      w=0;
      for (x=bmin;x<=bmax;x++) {
        for (y=rmin;y<=rmax;y++) {
	  for (z=0;z<depth;z++) {
            for (c=0;c<bmcnt[x][z];c++) {
	      b=bmptr[x][z][c];
              w+=weight[x-bbox][y-rbox][z]*b->sct[y];         
              if (b->sct[y] !=0) {
                cell[cnum]=&b->rng[y];
                cnum++;
	      }
	    }
	  }
	} 
      }

      if (cnum==0) continue;
      if ((bm==0) || (bm==maxbeam-1)) w=w*1.5;
      if (w<=thresh[mode % 2]) continue;

      dst->bm[bm].sct[rng]=1;
      dst->bm[bm].rng[rng].gsct=0;
      dst->bm[bm].rng[rng].p_l=0;
      dst->bm[bm].rng[rng].w_l=0; 
      dst->bm[bm].rng[rng].v=0;
 
      if (prm & 0x01) {
        mean=0;
        variance=0;
        sigma=0;
        cnt=0;
        for (c=0;c<cnum;c++) mean+=cell[c]->v;
        mean=mean/cnum;
        for (c=0;c<cnum;c++) 
           variance+=(cell[c]->v-mean)*(cell[c]->v-mean);
        variance=variance/cnum;
        if (variance>0) sigma=sqrt(variance);
        for (c=0;c<cnum;c++) {
          if (fabs(cell[c]->v-mean)>2*sigma) continue;
	  median[cnt]=cell[c]; 
          cnt++;
	}
        qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);
        dst->bm[bm].rng[rng].v=median[cnt/2]->v;
        mean=0;
        variance=0;
        for (c=0;c<cnt;c++) mean+=median[c]->v;
        mean=mean/cnt;
        for (c=0;c<cnt;c++) 
           variance+=(median[c]->v-mean)*(median[c]->v-mean);
        variance=variance/cnt;
        if (variance>0) sigma=sqrt(variance);
        else sigma=0;
        dst->bm[bm].rng[rng].v_e=sigma;
      }
      
      if (prm & 0x02) {
        mean=0;
        variance=0;
        sigma=0;
        cnt=0;
        for (c=0;c<cnum;c++) mean+=cell[c]->p_l;
        mean=mean/cnum;
        for (c=0;c<cnum;c++) 
           variance+=(cell[c]->p_l-mean)*(cell[c]->p_l-mean);
        variance=variance/cnum;
        if (variance>0) sigma=sqrt(variance);
        for (c=0;c<cnum;c++) {
          if (fabs(cell[c]->p_l-mean)>2*sigma) continue;
	  median[cnt]=cell[c]; 
          cnt++;
	}
        qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);
        dst->bm[bm].rng[rng].p_l=median[cnt/2]->p_l;
        mean=0;
        variance=0;
        for (c=0;c<cnt;c++) mean+=median[c]->p_l;
        mean=mean/cnt;
        for (c=0;c<cnt;c++) 
           variance+=(median[c]->p_l-mean)*(median[c]->p_l-mean);
        variance=variance/cnt;
        if (variance>0) sigma=sqrt(variance);
        else sigma=0;
        dst->bm[bm].rng[rng].p_l_e=sigma;
      }

      if (prm & 0x04) {
        mean=0;
        variance=0;
        sigma=0;
        cnt=0;
        for (c=0;c<cnum;c++) mean+=cell[c]->w_l;
        mean=mean/cnum;
        for (c=0;c<cnum;c++) 
           variance+=(cell[c]->w_l-mean)*(cell[c]->w_l-mean);
        variance=variance/cnum;
        if (variance>0) sigma=sqrt(variance);
        for (c=0;c<cnum;c++) {
          if (fabs(cell[c]->w_l-mean)>2*sigma) continue;
	  median[cnt]=cell[c]; 
          cnt++;
	}
        qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);
        dst->bm[bm].rng[rng].w_l=median[cnt/2]->w_l;
        mean=0;
        variance=0;
        for (c=0;c<cnt;c++) mean+=median[c]->w_l;
        mean=mean/cnt;
        for (c=0;c<cnt;c++) 
           variance+=(median[c]->w_l-mean)*(median[c]->w_l-mean);
        variance=variance/cnt;
        if (variance>0) sigma=sqrt(variance);
        else sigma=0;
        dst->bm[bm].rng[rng].w_l_e=sigma;
      }
      if (prm & 0x08) {
        mean=0;
        variance=0;
        sigma=0;
        cnt=0;
        for (c=0;c<cnum;c++) mean+=cell[c]->p_0;
        mean=mean/cnum;
        for (c=0;c<cnum;c++) 
           variance+=(cell[c]->p_0-mean)*(cell[c]->p_0-mean);
        variance=variance/cnum;
        if (variance>0) sigma=sqrt(variance);
        for (c=0;c<cnum;c++) {
          if (fabs(cell[c]->p_0-mean)>2*sigma) continue;
	  median[cnt]=cell[c]; 
          cnt++;
	}
        qsort(median,cnt,sizeof(struct RadarCell *), FilterCmpVel);
        dst->bm[bm].rng[rng].p_0=median[cnt/2]->p_0;
        mean=0;
        variance=0;
        for (c=0;c<cnt;c++) mean+=median[c]->p_0;
        mean=mean/cnt;
        for (c=0;c<cnt;c++) 
           variance+=(median[c]->p_0-mean)*(median[c]->p_0-mean);
        variance=variance/cnt;
        if (variance>0) sigma=sqrt(variance);
        else sigma=0;
        dst->bm[bm].rng[rng].p_0_e=sigma;
      }
      

    }
  }


  free(median);  
  free(cell);
  for (x=0;x<depth;x++) {
    for (bm=0;bm<maxbeam;bm++) if (bmptr[bm][x] !=NULL) free(bmptr[bm][x]);
  }
  return 0;

}


