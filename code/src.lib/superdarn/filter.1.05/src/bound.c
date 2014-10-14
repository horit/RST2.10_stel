/* bound.c
   ======= 
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
 $Log: bound.c,v $
 Revision 1.3  2004/07/13 22:34:10  barnes
 Removed redundant headers.

 Revision 1.2  2004/05/04 18:24:36  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/08/05 15:30:02  barnes
 Initial revision

*/


int FilterBoundType(struct RadarScan *ptr,int type) {
  int bm,rng;
  for (bm=0;bm<ptr->num;bm++) {
    for (rng=0;rng<MAX_RANGE;rng++) {
      if (ptr->bm[bm].sct[rng]==0) continue;
      if (ptr->bm[bm].rng[rng].gsct==type) ptr->bm[bm].sct[rng]=0;  
        
    }
  }
  return 0;
}

int FilterBound(int prm,struct RadarScan *ptr,double *min,double *max) {
  int bm,rng;
  for (bm=0;bm<ptr->num;bm++) {
    for (rng=0;rng<MAX_RANGE;rng++) {
       
      if (ptr->bm[bm].sct[rng]==0) continue;
     
      if (fabs(ptr->bm[bm].rng[rng].v)<min[0]) ptr->bm[bm].sct[rng]=0;
      if (fabs(ptr->bm[bm].rng[rng].v)>max[0]) ptr->bm[bm].sct[rng]=0;

      if (ptr->bm[bm].rng[rng].p_l<min[1]) ptr->bm[bm].sct[rng]=0;
      if (ptr->bm[bm].rng[rng].p_l>max[1]) ptr->bm[bm].sct[rng]=0;
 
      if (ptr->bm[bm].rng[rng].w_l<min[2]) ptr->bm[bm].sct[rng]=0;
      if (ptr->bm[bm].rng[rng].w_l>max[2]) ptr->bm[bm].sct[rng]=0;

      if (ptr->bm[bm].rng[rng].v_e<min[3]) ptr->bm[bm].sct[rng]=0;
      if (ptr->bm[bm].rng[rng].v_e>max[3]) ptr->bm[bm].sct[rng]=0;
        
   }
  }
  return 0;
}
