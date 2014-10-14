/* load.c
   ====== 
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
#include "rmath.h"
#include "rmap.h"

/*
 $Log: load.c,v $
 Revision 1.6  2004/06/22 17:25:22  barnes
 Changed references of Long to Int.

 Revision 1.5  2004/05/05 15:04:10  barnes
 Added missing header.

 Revision 1.4  2004/05/04 22:44:58  barnes
 Changed math header name.

 Revision 1.3  2004/05/04 18:00:17  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:03:12  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/08/06 20:06:31  barnes
 Initial revision

*/


struct PolygonData *MapFread(FILE *fp) {
  float pnt[2];
  struct PolygonData *ptr=NULL;
  int32 n;
  int s,i;
  

  ptr=PolygonMake(2*sizeof(float),NULL);
  if (ptr==NULL) return NULL;

  while(ConvertFreadInt(fp,&n)==0) {
    s=fgetc(fp);
    PolygonAddPolygon(ptr,s);
    for (i=0;i<n;i++) {
      ConvertFreadFloat(fp,&pnt[0]);
      ConvertFreadFloat(fp,&pnt[1]);
      PolygonAdd(ptr,pnt,0);
    }
  } 
  return ptr;
}

struct PolygonData *MapBndFread(FILE *fp) {
  float pnt[2];
  struct PolygonData *ptr=NULL;
  int32 n;
  int32 id;
  int i;
  

  ptr=PolygonMake(2*sizeof(float),NULL);
  if (ptr==NULL) return NULL;

  while(ConvertFreadInt(fp,&n)==0) {
    ConvertFreadInt(fp,&id);
    PolygonAddPolygon(ptr,id);
    for (i=0;i<n;i++) {
      ConvertFreadFloat(fp,&pnt[0]);
      ConvertFreadFloat(fp,&pnt[1]);
      PolygonAdd(ptr,pnt,0);
    }
  } 
  return ptr;
}
