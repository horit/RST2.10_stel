/* clip.c
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
 $Log: clip.c,v $
 Revision 1.4  2004/05/05 15:04:10  barnes
 Added missing header.

 Revision 1.3  2004/05/04 22:44:58  barnes
 Changed math header name.

 Revision 1.2  2004/05/04 18:00:17  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/08/06 20:06:45  barnes
 Initial revision

*/

struct PolygonData *MapCircleClip(float step) {
  float p[2],r;
  struct PolygonData *clip;
  
  clip=PolygonMake(2*sizeof(float),PolygonXYbbox);
  if (clip==NULL) return NULL;
  PolygonAddPolygon(clip,1);
  if (step<1) step=1; 
  if (step>45) step=45;
  for (r=0;r<360;r+=step) {
    p[0]=0.5+0.5*cosd(r);
    p[1]=0.5+0.5*sind(r);
    PolygonAdd(clip,p,0);
  } 
  return clip;
}

struct PolygonData *MapSquareClip() {
  int n=4;
  int i;
  float p[]={0,0, 1,0,  1,1, 0,1};
  struct PolygonData *clip;
  
  clip=PolygonMake(2*sizeof(float),PolygonXYbbox);
  if (clip==NULL) return NULL;
  PolygonAddPolygon(clip,1);

  for (i=0;i<n;i++) PolygonAdd(clip,&p[2*i],0);
  return clip;
}

