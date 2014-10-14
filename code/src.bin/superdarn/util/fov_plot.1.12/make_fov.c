/* make_fov.c
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
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "radar.h"
#include "rpos.h"
#include "polygon.h"

/*
 $Log: make_fov.c,v $
 Revision 1.5  2006/07/14 15:31:50  code
 Modification to allow altitude to be specified on the command line.

 Revision 1.4  2006/03/14 21:41:55  barnes
 Modification to deal with radars with extra beams.

 Revision 1.3  2005/09/09 16:12:49  barnes
 The maximum range gate is now taken from the hardware table.

 Revision 1.2  2004/05/10 21:55:45  barnes
 Removed redundant code.

 Revision 1.1  2003/04/25 14:45:33  barnes
 Initial revision

*/


struct PolygonData *make_fov(double tval,struct RadarNetwork *network,
                             float alt) {
  double rho,lat,lon;
  int i,rn,bm;
  float pnt[2];
  int yr,mo,dy,hr,mt;
  double sc;
  int frang=180;
  int rsep=45;
  struct PolygonData *ptr=NULL;
  struct RadarSite *site=NULL;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

  ptr=PolygonMake(sizeof(float)*2,NULL);
         
  for (i=0;i<network->rnum;i++) {
 
    site=RadarYMDHMSGetSite(&(network->radar[i]),yr,mo,dy,hr,mt,(int) sc);
    if (site==NULL) continue;
    PolygonAddPolygon(ptr,i);
    
    for (rn=0;rn<=site->maxrange;rn++) {
      RPosGeo(0,0,rn,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt,0);
    }
    for (bm=1;bm<=site->maxbeam;bm++) {
      RPosGeo(0,bm,site->maxrange,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt,0);
    }
    for (rn=site->maxrange-1;rn>=0;rn--) {
      RPosGeo(0,site->maxbeam,rn,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt,0);
    }
    for (bm=site->maxbeam-1;bm>0;bm--) {
      RPosGeo(0,bm,0,site,frang,rsep,
                          site->recrise,alt,&rho,&lat,&lon);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt,0);
    }
  }
  return ptr;
}




