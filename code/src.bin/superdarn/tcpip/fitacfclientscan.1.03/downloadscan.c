/* downloadscan.c
   ==============
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
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "fitcnx.h"
#include "scandata.h"
#include "fitscan.h"

/*
 $Log: downloadscan.c,v $
 Revision 1.1  2004/05/10 22:15:52  barnes
 Initial revision

 */

int downloadscan(int sock,struct RadarParm *prm,struct FitData *fit,
				struct RadarScan *ptr) {


   int c=0;
   int flag,status=0;

   ptr->st_time=-1;
   ptr->ed_time=-1;
  
    
   /* clear out the arrays */

   RadarScanReset(ptr);

  while (prm->scan !=1) {  
    status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);
    fprintf(stderr,"+%d (%d)+",prm->bmnum,prm->scan); 
    fflush(stderr);
  } 
  fprintf(stderr,"\n");

  ptr->stid=prm->stid;
  ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                                 prm->time.hr,prm->time.mt,prm->time.sc+
                                 prm->time.us/1.0e6);


  prm->scan=0;

   while (prm->scan !=1)  {
     fprintf(stderr,"%.2d.",prm->bmnum);
     fflush(stderr);
     if (prm->scan==0) FitToRadarScan(ptr,prm,fit);
      status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);
     c++;
   }
   ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                                 prm->time.hr,prm->time.mt,prm->time.sc+
                                 prm->time.us/1.0e6);

   fprintf(stderr,"\n");
   if (c>0) return 0;
   return -1;
}
 










