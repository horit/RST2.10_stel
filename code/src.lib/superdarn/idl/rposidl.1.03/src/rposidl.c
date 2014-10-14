/* rposidl.c
   =========== 
   Author: R.J.Barnes & D.Andre
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
#include "idl_export.h"
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "radar.h"
#include "rpos.h"
#include "invmag.h"

/*
 $Log: rposidl.c,v $
 Revision 1.3  2007/01/22 15:01:55  code
 Fix to deal with radars with more than 16 beams or ranges.

 Revision 1.2  2005/04/12 21:10:54  barnes
 Added Dieter Andre's RposGeochrtIDL routine.

 Revision 1.1  2004/08/09 18:10:27  barnes
 Initial revision

*/

int32 RPosIDL(int argc,char *argv[]) {

  int32 center;
  int32 bmcrd;
  int32 rncrd;
  int32 frang;
  int32 rsep;
  int32 rxrise;
  double hgt;
  double *rho,*lat,*lng;
  struct RadarSite *pos;
      
  center = *( (int32 *) argv[0]);
  bmcrd = *( (int32 *) argv[1]);
  rncrd = *( (int32 *) argv[2]);
  pos=(struct RadarSite *) argv[3];  
  frang = *( (int32 *) argv[4]);
  rsep = *( (int32 *) argv[5]);
  rxrise = *( (int32 *) argv[6]);
  hgt = *( (double *) argv[7]);
  rho= (double *) argv[8];
  lat = (double *) argv[9];
  lng = (double *) argv[10];

  RPosGeo(center,bmcrd,rncrd,pos,frang,rsep,rxrise,hgt,
          rho,lat,lng);


  return 0;
}



int32 RPosGSIDL(int argc,char *argv[]) {

  int32 center;
  int32 bmcrd;
  int32 rncrd;
  int32 frang;
  int32 rsep;
  int32 rxrise;
  double hgt;
  double *rho,*lat,*lng;
  struct RadarSite *pos;
      
  center = *( (int32 *) argv[0]);
  bmcrd = *( (int32 *) argv[1]);
  rncrd = *( (int32 *) argv[2]);
  pos=(struct RadarSite *) argv[3];  
  frang = *( (int32 *) argv[4]);
  rsep = *( (int32 *) argv[5]);
  rxrise = *( (int32 *) argv[6]);
  hgt = *( (double *) argv[7]);
  rho= (double *) argv[8];
  lat = (double *) argv[9];
  lng = (double *) argv[10];

  RPosGeoGS(center,bmcrd,rncrd,pos,frang,rsep,rxrise,hgt,
          rho,lat,lng);


  return 0;
}


int32 RPosGeochrtIDL(int argc,char *argv[]) {

  int32 center;
  int32 bmcrd;
  int32 rncrd;
  int32 yr;
  int32 frang;
  int32 rsep;
  int32 rxrise;
  double hgt;
  double rho_int, lat_int, lng_int, azm_int, elv_int;

  float *rho, *lat,*lng,*azm,*elv;
  struct RadarSite *pos;
      
  center = 1;
  pos=(struct RadarSite *) argv[0];  
  yr = *( (int32 *) argv[1]);
  frang = *( (int32 *) argv[2]);
  rsep = *( (int32 *) argv[3]);
  rxrise = *( (int32 *) argv[4]);
  hgt = *( (float *) argv[5]);
  rho = (float *) argv[6];
  lat = (float *) argv[7];
  lng = (float *) argv[8];
  azm = (float *) argv[9];
  elv = (float *) argv[10];

  for ( rncrd=0; rncrd<pos->maxrange; rncrd++) {
    for ( bmcrd=0; bmcrd<pos->maxbeam; bmcrd++) {
      RPosGeo(center, bmcrd, rncrd, pos, frang, rsep, rxrise, 
              hgt, &rho_int, &lat_int, &lng_int);
      RPosRngBmAzmElv( bmcrd, rncrd, yr, pos, frang, rsep, 
                       rxrise, hgt, &azm_int, &elv_int);
      rho[ pos->maxbeam* rncrd + bmcrd]= rho_int;
      lat[ pos->maxbeam* rncrd + bmcrd]= lat_int;
      lng[ pos->maxbeam* rncrd + bmcrd]= lng_int;
      azm[ pos->maxbeam* rncrd + bmcrd]= azm_int;
      elv[ pos->maxbeam* rncrd + bmcrd]= elv_int;
     }
  }


  return 0;
}


