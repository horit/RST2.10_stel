/* plot_time.c
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
#include "rmath.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rmap.h"
#include "text_box.h"

/*
 $Log: plot_time.c,v $
 Revision 1.8  2004/06/21 22:50:19  barnes
 Added missing header.

 Revision 1.7  2004/05/10 21:58:59  barnes
 Removed redundant code.

 Revision 1.6  2004/04/27 22:49:56  barnes
 Audit - Enforced warning set to "all".

 Revision 1.5  2003/03/01 03:20:11  barnes
 Major modifications to support base libraries and Rplot XML output.

 Revision 1.4  2001/06/27 21:08:56  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.2  1999/06/15 18:07:22  barnes
 Changed text_ps_box to float.

 Revision 1.1  1998/11/12 18:10:38  barnes
 Initial revision

*/


void plot_time(struct Rplot *rplot,
               float xoff,float yoff,float wdt,float hgt,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {
  float txbox[3];
  char txt[16];
  int i;

  float px,py;
  float lon;
  float rad=wdt/2;
  double x,y;
  float xtxt=0,ytxt=0;
  
  for (i=0;i<24;i+=6) {  
     sprintf(txt,"%.2d",i);
    
     lon=i*360/24;
   
     x=sin(lon*PI/180.0);
     y=cos(lon*PI/180.0);
     
     px=(xoff+wdt/2)+x*rad;
     py=(yoff+hgt/2)+y*rad; 

     txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);

     ytxt=0;
     xtxt=0;
     
     if (i==0) ytxt=txbox[2];
     else if (i==12) ytxt=txbox[1];
     else if (i==18) xtxt=txbox[0]; 
    
     px=(xoff+wdt/2)+x*(rad+2+xtxt);
     py=(yoff+hgt/2)+y*(rad+2+ytxt); 

     if ((i % 12)==0) px=px-txbox[0]/2;
     else py=py+(txbox[2]-txbox[1])/2; 

     RplotText(rplot,NULL,fontname,fontsize,px,py,strlen(txt),txt,
               color,mask,1);

  }
}















