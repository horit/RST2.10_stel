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
#include <math.h>

#include "rfbuffer.h"

/*
 $Log: clip.c,v $
 Revision 1.1  2003/03/19 01:20:50  barnes
 Initial revision

*/


struct FrameBufferClip *fbclip(int wdt,int hgt) {

  int i;
  float sf=0.55;
  int px[12];
  int py[12];
  int t[12];

  px[0]=0.5*wdt;
  py[0]=0;
  px[1]=0.5*wdt;
  py[1]=0.5*hgt*sf;
  px[2]=0.5*wdt*sf;
  py[2]=0.5*hgt;
  px[3]=0;
  py[3]=0.5*hgt;
  px[4]=-0.5*wdt*sf;
  py[4]=0.5*hgt;
  px[5]=-0.5*wdt;
  py[5]=0.5*hgt*sf;
  px[6]=-0.5*wdt;
  py[6]=0;
  px[7]=-0.5*wdt;
  py[7]=-0.5*hgt*sf;
  px[8]=-0.5*wdt*sf;
  py[8]=-0.5*hgt;
  px[9]=0;
  py[9]=-0.5*hgt;
  px[10]=0.5*wdt*sf;
  py[10]=-0.5*hgt;
  px[11]=0.5*wdt;
  py[11]=-0.5*hgt*sf;

  for (i=0;i<12;i++) {
    t[i]=1;
    px[i]+=0.5*wdt;
    py[i]+=0.5*hgt;
  }
  return FrameBufferMakeClip(wdt,hgt,12,px,py,t);
}

