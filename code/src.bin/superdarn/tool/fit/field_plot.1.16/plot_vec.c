/* plot_vec.c
   ========== 
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
#include "calcvector.h"
#include "geobeam.h"
#include "plot_cell.h"

/*
 $Log: plot_vec.c,v $
 Revision 1.2  2004/08/25 21:40:21  barnes
 Fixes for MacOS 10 (Darwin)

 Revision 1.1  2004/05/18 18:11:28  barnes
 Initial revision

*/



void plot_vec(struct Rplot *rplot,struct RadarBeam *sbm,
               struct GeoLocBeam *gbm,float latmin,int magflg,
	      float xoff,float yoff,float wdt,float hgt,float sf,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
               unsigned int(*cfn)(double,void *),void *cdata,
               unsigned int gscol,unsigned char gsflg,float width,float rad) {

  int rng;
  unsigned int color=0;
  float ax=0,ay=0,bx=0,by=0;
  int s=0;
  float map[2],pnt[2];
  double lat,lon;
  for (rng=0;rng<sbm->nrang;rng++) {
    if (sbm->sct[rng]==0) continue;

    if (cfn !=NULL) color=(*cfn)(fabs(sbm->rng[rng].v),cdata);
    if ((gsflg) && (sbm->rng[rng].gsct !=0)) color=gscol;
     
    if (magflg) {
       map[0]=gbm->mlat[rng][1];
       map[1]=gbm->mlon[rng][1];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1];     

       if (sbm->rng[rng].gsct==0) {
         RPosCalcVector(map[0],map[1],sbm->rng[rng].v*sf,
                      gbm->mazm[rng],&lat,&lon);


         map[0]=lat;
         map[1]=lon;
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) continue;
         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];
       }     
     } else {
       map[0]=gbm->glat[rng][1];
       map[1]=gbm->glon[rng][1];
       s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
       if (s !=0) continue;
       ax=xoff+wdt*pnt[0];
       ay=yoff+hgt*pnt[1];     
       if (sbm->rng[rng].gsct==0) {
      
         RPosCalcVector(map[0],map[1],sbm->rng[rng].v*sf,
                        gbm->gazm[rng],&lat,&lon);


         map[0]=lat;
         map[1]=lon;
         s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
         if (s !=0) continue;
         bx=xoff+wdt*pnt[0];
         by=yoff+hgt*pnt[1];     
       }

    }
   
    if (sbm->rng[rng].gsct==0)
      RplotLine(rplot,ax,ay,bx,by,color,0x0f,width,NULL);    
    RplotEllipse(rplot,NULL,ax,ay,
                 rad,rad,1,color,0x0f,0,NULL);
    
        
    
  } 
}
