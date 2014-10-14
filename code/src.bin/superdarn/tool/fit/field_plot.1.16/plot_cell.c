/* plot_cell.c
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
#include <math.h>
#include <sys/types.h>
#include "rmath.h"
#include "rtypes.h"
#include "aacgm.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rfile.h"
#include "limit.h"
#include "radar.h"
#include "scandata.h"
#include "geobeam.h"
#include "plot_cell.h"

/*
 $Log: plot_cell.c,v $
 Revision 1.2  2004/05/18 18:11:28  barnes
 Modified handling of ground scatter flag.

 Revision 1.1  2004/05/10 21:54:01  barnes
 Initial revision

*/



void plot_cell(struct Rplot *rplot,struct RadarBeam *sbm,
               struct GeoLocBeam *gbm,float latmin,int magflg,
               float xoff,float yoff,float wdt,float hgt,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
               unsigned int(*cfn)(double,void *),void *cdata,
               int prm,unsigned int gscol,unsigned char gsflg) {

  int rng;
  unsigned int color=0;
  float px[4],py[4];
  int s=0;
  int t[4]={0,0,0,0};
  float map[2],pnt[2];
  for (rng=0;rng<sbm->nrang;rng++) {
    if (sbm->sct[rng]==0) continue;

    if (cfn !=NULL) {
      if (prm==1) color=(*cfn)(sbm->rng[rng].p_l,cdata);
      else if (prm==2)  color=(*cfn)(sbm->rng[rng].v,cdata);
      else color=(*cfn)(sbm->rng[rng].w_l,cdata);
    }
    
    if ((prm==2) && (gsflg) && (sbm->rng[rng].gsct !=0)) color=gscol;
   
    if (magflg) {
       map[0]=gbm->mlat[rng][0];
       map[1]=gbm->mlon[rng][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[0]=xoff+wdt*pnt[0];
       py[0]=yoff+hgt*pnt[1];     
       map[0]=gbm->mlat[rng][2];
       map[1]=gbm->mlon[rng][2];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[1]=xoff+wdt*pnt[0];
       py[1]=yoff+hgt*pnt[1];   
       map[0]=gbm->mlat[rng+1][2];
       map[1]=gbm->mlon[rng+1][2];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[2]=xoff+wdt*pnt[0];
       py[2]=yoff+hgt*pnt[1];     
       map[0]=gbm->mlat[rng+1][0];
       map[1]=gbm->mlon[rng+1][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[3]=xoff+wdt*pnt[0];
       py[3]=yoff+hgt*pnt[1];     
    } else {
       map[0]=gbm->glat[rng][0];
       map[1]=gbm->glon[rng][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[0]=xoff+wdt*pnt[0];
       py[0]=yoff+hgt*pnt[1];     
       map[0]=gbm->glat[rng][2];
       map[1]=gbm->glon[rng][2];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[1]=xoff+wdt*pnt[0];
       py[1]=yoff+hgt*pnt[1];   
       map[0]=gbm->glat[rng+1][2];
       map[1]=gbm->glon[rng+1][2];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[2]=xoff+wdt*pnt[0];
       py[2]=yoff+hgt*pnt[1];     
       map[0]=gbm->glat[rng+1][0];
       map[1]=gbm->glon[rng+1][0];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       px[3]=xoff+wdt*pnt[0];
       py[3]=yoff+hgt*pnt[1];  
    }
   

    RplotPolygon(rplot,NULL,0,0,4,px,py,t,1,color,0x0f,0,NULL);
    
        
    
  } 
}
