/* geobeam.c
   ========= */


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
#include "rmath.h"
#include "limit.h"
#include "radar.h" 
#include "rpos.h"
#include "invmag.h"
#include "scandata.h"
#include "geobeam.h"
#include "aacgm.h"

/*
 $Log: geobeam.c,v $
 Revision 1.1  2004/05/10 21:54:01  barnes
 Initial revision

*/



int GeoLocCenter(struct RadarSite *site,int mag,float *lat,float *lon) {

  int s;
  double glat,glon,mlat,mlon,rho;
  RPosGeo(0,8,35,site,180,45,100,
            300.0,&rho,&glat,&glon);

  if (mag) { 
    s=AACGMConvert(glat,glon,300,&mlat,&mlon,&rho,0);
    *lat=mlat;
    *lon=mlon;
  } else {
    *lat=glat;
    *lon=glon;
  }
 return 0;
}

int GeoLocBeam(struct RadarSite *site,int year,
               struct GeoLocScan *geol,struct RadarBeam *bm) {
  int n,s;
  int rng;
  double rho,lat,lon,glat,glon,mlat,mlon;
  double geoazm,elv,magazm; 
  for (n=0;n<geol->num;n++) {
    if (geol->bm[n].bm !=bm->bm) continue;
    if (geol->bm[n].frang !=bm->frang) continue;
    if (geol->bm[n].rsep !=bm->rsep) continue;
    if (geol->bm[n].rxrise !=bm->rxrise) continue;
    break;
  }
 
  if (n<geol->num) return n;
   
  /* we've got a new beam */
   
  if (geol->bm==NULL) geol->bm=malloc(sizeof(struct GeoLocBeam));
  else {
    struct GeoLocBeam *tmp=NULL;
    tmp=realloc(geol->bm,(geol->num+1)*sizeof(struct GeoLocBeam));
    if (tmp==NULL) return -1;
    geol->bm=tmp;
  }
  geol->bm[n].bm=bm->bm;
  geol->bm[n].frang=bm->frang;
  geol->bm[n].rsep=bm->rsep;
  geol->bm[n].rxrise=bm->rxrise;
 
  for (rng=0;rng<=MAX_RANGE;rng++) {
    RPosGeo(0,bm->bm,rng,site,bm->frang,bm->rsep,bm->rxrise,
               300.0,&rho,&lat,&lon);
    geol->bm[n].glat[rng][0]=lat;
    geol->bm[n].glon[rng][0]=lon;
    RPosGeo(0,bm->bm+1,rng,site,bm->frang,bm->rsep,bm->rxrise,
               300.0,&rho,&lat,&lon);
    geol->bm[n].glat[rng][2]=lat;
    geol->bm[n].glon[rng][2]=lon;
    if (rng<MAX_RANGE) {
      RPosGeo(1,bm->bm,rng,site,bm->frang,bm->rsep,bm->rxrise,
                 300.0,&rho,&lat,&lon);
      geol->bm[n].glat[rng][1]=lat;
      geol->bm[n].glon[rng][1]=lon;
      RPosRngBmAzmElv(bm->bm,rng,year,site,
                   bm->frang,bm->rsep,bm->rxrise,
                   300.0,&geoazm,&elv);
      geol->bm[n].gazm[rng]=geoazm;
    }
    
    glat=geol->bm[n].glat[rng][0];
    glon=geol->bm[n].glon[rng][0];
    s=AACGMConvert(glat,glon,300,&mlat,&mlon,&rho,0);
    geol->bm[n].mlat[rng][0]=mlat;
    geol->bm[n].mlon[rng][0]=mlon;
    
    glat=geol->bm[n].glat[rng][2];
    glon=geol->bm[n].glon[rng][2];
    s=AACGMConvert(glat,glon,300,&mlat,&mlon,&rho,0);
    geol->bm[n].mlat[rng][2]=mlat;
    geol->bm[n].mlon[rng][2]=mlon;
    if (rng<MAX_RANGE) {
      RPosInvMag(bm->bm,rng,year,site,bm->frang,bm->rsep,bm->rxrise,
              300.0,&mlat,&mlon,&magazm);
      geol->bm[n].mlat[rng][1]=mlat;
      geol->bm[n].mlon[rng][1]=mlon;
      geol->bm[n].mazm[rng]=magazm;
    }
  }
  geol->num++;
 
  return n;
}


  




