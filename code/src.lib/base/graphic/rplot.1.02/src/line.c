/* line.c
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
#include "rfbuffer.h"
#include "rplot.h"

/*
 $Log: line.c,v $
 Revision 1.1  2003/03/01 18:38:45  barnes
 Initial revision

*/


int RplotLine(struct Rplot *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,unsigned char mask,float width,
              struct RplotDash *dash) {

  int s=0;
  char quot='"';
  char buf[4096];
  char lbuf[256];

  if (ptr==NULL) return -1;

  sprintf(lbuf,"<line color=%c%.8x%c mask=%c%.2x%c width=%c%g%c",
               quot,color,quot,quot,mask,quot,quot,width,quot);
  strcpy(buf,lbuf);
  
  if (dash !=NULL) {
    int i;
    sprintf(lbuf," dash=%c%g",quot,dash->phase);
    strcat(buf,lbuf);
    for (i=0;i<dash->sze;i++) {
      sprintf(lbuf," %g",dash->p[i]);
      strcat(buf,lbuf);
    }
    sprintf(lbuf,"%c",quot);
    strcat(buf,lbuf);
  }


  strcat(buf,">\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;
  
  sprintf(buf,"<s><p>");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;

  sprintf(buf,"<x>%g</x><y>%g</y>",ax,ay);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;
  
  sprintf(buf,"</p></s>\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;

  sprintf(buf,"<e><p>");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;

  sprintf(buf,"<x>%g</x><y>%g</y>",bx,by);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;

  sprintf(buf,"</p></e>\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;

  sprintf(buf,"</line>\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  return s;
} 



