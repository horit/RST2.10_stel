/* make_grid.c
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
#include "rtypes.h"
#include "rconvert.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rmap.h"

/*
 $Log: make_grid.c,v $
 Revision 1.4  2004/05/05 16:32:29  barnes
 Added missing header.

 Revision 1.3  2004/05/04 20:07:06  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 20:25:21  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/09 19:50:34  barnes
 Initial revision

*/

struct PolygonData *make_grid(float lonspc,float latspc,int max) {

  struct PolygonData *ptr=NULL;
  float lat,lon,l;
  float latmin,latmax;
  float pnt[2];


  ptr=PolygonMake(2*sizeof(float),NULL);
  if (ptr==NULL) return NULL;

  latmin=-90+latspc;
  latmax=90-latspc;

  if (max) latmin=-90;
  if (max) latmax=90;

  for (lat=latmin;lat<latmax;lat+=latspc) {
    for (lon=0;lon<360;lon+=lonspc) {
      PolygonAddPolygon(ptr,1);
      pnt[0]=lat;
      pnt[1]=lon;
      PolygonAdd(ptr,pnt,0);
      for (l=1;l<=latspc;l+=1) {
        pnt[0]=lat+l;
        PolygonAdd(ptr,pnt,0);
      }
      pnt[0]=lat+latspc;
      for (l=0;l<=lonspc;l+=1) {
        pnt[1]=lon+l;
        PolygonAdd(ptr,pnt,0);
      }
      pnt[1]=lon+lonspc;
      for (l=1;l<=latspc;l+=1) {
        pnt[0]=lat+latspc-l;
        PolygonAdd(ptr,pnt,0);
      }
      pnt[0]=lat;
      for (l=1;l<lonspc;l+=1) {
        pnt[1]=lon+lonspc-l;
        PolygonAdd(ptr,pnt,0);
      }

    }
  }
  return ptr;
}
