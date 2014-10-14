/* plot_model_name.c
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

#include "rfbuffer.h"
#include "rplot.h"
#include "rtime.h"
#include "text_box.h"

/*
 $Log: plot_model_name.c,v $
 Revision 1.2  2004/06/21 22:37:11  barnes
 Added missing header.

 Revision 1.1  2003/03/15 21:20:45  barnes
 Initial revision

*/

void plot_model_name(struct Rplot *rplot,
                    float xoff,float yoff,
	            char *dir,char *mag,
                    unsigned int color,unsigned char mask,
                    char *fontname,float fontsize,
                    void *txtdata) {

  char txt[256];
  float txbox[3];
  float lhgt;

  sprintf(txt,"Xy");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  lhgt=4+txbox[2]-txbox[1];

  sprintf(txt,"APL MODEL");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  RplotText(rplot,NULL,fontname,fontsize,xoff-txbox[0]/2,yoff,
            strlen(txt),txt,color,mask,1);

  txtbox(fontname,fontsize,strlen(mag),mag,txbox,txtdata);
  RplotText(rplot,NULL,fontname,fontsize,xoff-txbox[0]/2,yoff+lhgt,
            strlen(mag),mag,color,mask,1);

  txtbox(fontname,fontsize,strlen(dir),dir,txbox,txtdata);
  RplotText(rplot,NULL,fontname,fontsize,xoff-txbox[0]/2,yoff+2*lhgt,
            strlen(dir),dir,color,mask,1);
}
  
