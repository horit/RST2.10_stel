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
#include <unistd.h>
#include <math.h>

#include "rmath.h"
#include "rtypes.h"
#include "rconvert.h"
#include "eldata.h"
  
/*
 $Log: load.c,v $
 Revision 1.3  2004/06/08 20:31:48  barnes
 Added missing UNIX header.

 Revision 1.2  2004/05/14 19:45:28  barnes
 Changed header names.

 Revision 1.1  2003/08/06 20:10:23  barnes
 Initial revision

*/


int ElevationLoad(FILE *fp,struct ElevationData *ptr) {
  int pattern[8]={0,0,0,0,0,0,0,0};
  unsigned char *hptr;

  pattern[0]=2;
  pattern[1]=4;
  pattern[2]=4;
  pattern[3]=4;
  pattern[4]=2;
  pattern[5]=2;
  pattern[6]=0;
  pattern[7]=0;

  hptr=(unsigned char *) ptr;

  if (fread(hptr,28,1,fp) !=1) return -1;
  ConvertBlock(hptr,pattern);
  if (ptr->data==NULL) ptr->data=malloc(sizeof(int16)*ptr->numlat*ptr->numlon);
  else {
   char *tmp;
   tmp=realloc(ptr->data,sizeof(int16)*ptr->numlat*ptr->numlon);
   if (tmp==NULL) free(ptr->data);
   ptr->data=NULL;
  }

  if (ptr->data==NULL) return -1;

  pattern[1]=ptr->numlat*ptr->numlon;
  pattern[2]=0;
  pattern[3]=0;
 
  if (fread(ptr->data,2*ptr->numlat*ptr->numlon,1,fp) !=1) return -1;
  ConvertBlock( (unsigned char *) ptr->data,pattern);
  return 0;
}




