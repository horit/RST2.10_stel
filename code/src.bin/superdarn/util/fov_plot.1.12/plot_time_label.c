/* plot_time_label.c
   ================= 
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
#include "rmath.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rtime.h"
#include "text_box.h"

/*
 $Log: plot_time_label.c,v $
 Revision 1.3  2004/06/21 22:43:19  barnes
 Added missing header.

 Revision 1.2  2004/05/10 21:55:45  barnes
 Removed redundant code.

 Revision 1.1  2003/04/25 14:45:33  barnes
 Initial revision

*/

void plot_time_label(struct Rplot *rplot,
	             float xoff,float yoff,float wdt,float hgt,
                     float pole,int flip,char *sfx,float tme_shft,
	             float rad,float tick,
  	             unsigned int color,unsigned char mask,
                     char *fontname,float fontsize,
                     void *txtdata) {

  char txt[256];
  float txbox[3];
  float lon;
  float x,y,xtxt,ytxt,px,py;
  int i;
  
  if (flip==1) pole=-pole;

  for (i=0;i<24;i+=6) {  
 
     if ((i==0) || (i==12)) sprintf(txt,"%.2d %s",i,sfx);
     else sprintf(txt,"%.2d",i);
   

     lon=15*i-tme_shft;
     if (pole<0) lon=-lon;

     x=sin(lon*PI/180.0);
     y=cos(lon*PI/180.0);

 
     txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
     xtxt=0;
     ytxt=0; 

     if (pole>0) {
         if (i==0) ytxt=txbox[2];
         else if (i==12) ytxt=txbox[1];
         else if (i==18) xtxt=txbox[0]; 
     } else {
         if (i==0) ytxt=txbox[2];
         else if (i==6) xtxt=txbox[0]; 
         else if (i==12) ytxt=txbox[1];     
     }     

     px=(xoff+wdt/2)+x*(rad+tick+2+xtxt);
     py=(yoff+hgt/2)+y*(rad+tick+2+ytxt); 

     if ((i % 12)==0) px=px-txbox[0]/2;
     else py=py+(txbox[2]-txbox[1])/2; 

     RplotText(rplot,NULL,fontname,fontsize,px,py,
               strlen(txt),txt,color,mask,1);
  }
}
  

