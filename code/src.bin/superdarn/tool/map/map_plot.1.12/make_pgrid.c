/* make_pgrid.c
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
#include "cnvgrid.h"

/*
 $Log: make_pgrid.c,v $
 Revision 1.2  2004/05/10 22:10:30  barnes
 Removed redundant code.

 Revision 1.1  2003/03/15 21:19:49  barnes
 Initial revision

*/

int make_pgrid(double latmin,struct CnvGrid *ptr) {
  
  int i,j;

  double lat_step;
  double lon_step;
 
  int nlat,nlon;
  int num;
  int poly; 

  ptr->type=0;
  ptr->num=0;
 
  lat_step=1.0;
  lon_step=2.0;
  num=0;  
  poly=0;


  if (latmin>0)  nlat=(int) (90.0-latmin)/lat_step;
  else  nlat=(int) (90.0+latmin)/lat_step;
  nlon=(int) (360.0/lon_step); 

  if (ptr->vertex !=NULL) free(ptr->vertex);
  if (ptr->lat !=NULL) free(ptr->lat);
  if (ptr->lon !=NULL) free(ptr->lon);
  if (ptr->mag !=NULL) free(ptr->mag);

  ptr->vertex=malloc(sizeof(int)*nlat*nlon*4);
  ptr->lat=malloc(sizeof(double)*nlat*nlon);
  ptr->lon=malloc(sizeof(double)*nlat*nlon);
  ptr->mag=malloc(sizeof(double)*nlat*nlon);

  if ((ptr->lat==NULL) || (ptr->lon==NULL) || 
      (ptr->mag==NULL) || (ptr->vertex==NULL)) return -1;

 if (latmin>0) {
    for (i=0;i<nlat;i++) { 
      for (j=0;j<nlon;j++) {
         if (i != nlat-1) {
           ptr->vertex[4*poly]=num;
           if (j !=nlon-1) ptr->vertex[4*poly+1]=num+1;
           else ptr->vertex[4*num+1]=num-nlon+1;
           ptr->vertex[4*poly+2]=ptr->vertex[4*poly+1]+nlon;
           ptr->vertex[4*poly+3]=num+nlon;     
           poly++;  
         } 
         ptr->lat[num]=i*lat_step+latmin;
         ptr->lon[num]=j*lon_step;
         ptr->mag[num]=0;
         num++;
      }
    }
  } else {
    for (i=0;i<nlat;i++) { 
      for (j=0;j<nlon;j++) {
         if (i != nlat-1) {
           ptr->vertex[4*poly]=num;
           if (j !=nlon-1) ptr->vertex[4*poly+1]=num+1;
           else ptr->vertex[4*num+1]=num-nlon+1;
           ptr->vertex[4*poly+2]=ptr->vertex[4*poly+1]+nlon;
           ptr->vertex[4*poly+3]=num+nlon;     
           poly++;  
         } 
         ptr->lat[num]=-i*lat_step+latmin;
         ptr->lon[num]=j*lon_step;
         ptr->mag[num]=0;
         num++;
      }
    }
  }

  ptr->num=num;
  ptr->nlat=nlat;
  ptr->nlon=nlon;
  ptr->poly=poly;
  return 0;
}
 






