/* make_vgrid.c
   ============ 
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
#include "rfile.h"
#include "cnvgrid.h"
#include "griddata.h"
#include "rmath.h"

/*
 $Log: make_vgrid.c,v $
 Revision 1.5  2005/08/18 16:02:54  barnes
 Removed redundant code.

 Revision 1.4  2005/08/18 16:01:50  barnes
 Fixed bug in naming the plot in stdout mode.

 Revision 1.3  2004/10/13 15:38:14  barnes
 Changed grid structure.

 Revision 1.2  2004/05/10 22:10:30  barnes
 Removed redundant code.

 Revision 1.1  2003/03/15 21:19:49  barnes
 Initial revision

*/

void  make_vgrid(struct GridData *ptr,struct CnvGrid *vptr) {
 
  int i;
  int num=0;
  int sze;
  sze=sizeof(double)*(ptr->vcnum);
 
  if (vptr->vertex !=NULL) free(vptr->vertex);
  if (vptr->lat !=NULL) free(vptr->lat);
  if (vptr->lon !=NULL) free(vptr->lon);
  if (vptr->mag !=NULL) free(vptr->mag);
  if (vptr->azm !=NULL) free(vptr->azm);

  vptr->type=1;
  vptr->vertex=NULL;
  vptr->lon=malloc(sze);
  vptr->lat=malloc(sze); 
  vptr->mag=malloc(sze); 
  vptr->azm=malloc(sze); 

  for (i=0;i<ptr->vcnum;i++) {
    vptr->lon[num]=ptr->data[i].mlon;
    vptr->lat[num]=ptr->data[i].mlat;
    vptr->mag[i]=0;
    vptr->azm[i]=0;
    num++;
    
  }
  vptr->num=num;
  vptr->poly=0;
}















