/* plot_logo.c
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

#include "rfbuffer.h"
#include "rplot.h"
#include "rtime.h"
#include "text_box.h"

/*
 $Log: plot_logo.c,v $
 Revision 1.2  2004/06/21 22:45:05  barnes
 Added missing header.

 Revision 1.1  2003/04/25 14:49:00  barnes
 Initial revision

*/

#define CREDIT "JHU/APL.Software by R.J.Barnes"


void plot_logo(struct Rplot *rplot,
               float xoff,float yoff,float wdt,float hgt,
	       unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {

  char txt[256];
  float txbox[3];
  
  sprintf(txt,"SuperDARN");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  RplotText(rplot,NULL,fontname,fontsize,xoff,yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);

  
}
  

void plot_web(struct Rplot *rplot,
               float xoff,float yoff,float wdt,float hgt,
	       unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {

  char txt[256];
  float txbox[3];
   
  sprintf(txt,"http://superdarn.jhuapl.edu");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  RplotText(rplot,NULL,fontname,fontsize,xoff+wdt-txbox[0],yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);

  
}
  
void plot_credit(struct Rplot *rplot,
               float xoff,float yoff,float wdt,float hgt,
	       unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {

  char txt[256];
  float txbox[3];
 
  sprintf(txt,CREDIT);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  RplotText(rplot,NULL,fontname,fontsize,xoff,yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);

  
}
  
