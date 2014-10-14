/* key.c
   =====
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
#include "key.h"

/*
 $Log: key.c,v $
 Revision 1.3  2004/06/21 22:22:31  barnes
 Added missing header.

 Revision 1.2  2004/04/27 20:31:55  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/03 18:54:22  barnes
 Initial revision

*/

int load_key(FILE *fp,struct key *key) {

  int i,j=0;
  unsigned int colval;
  int max=0;
  char line[1024];
  char *tkn;
  while (fgets(line,1024,fp) !=0) {
  
    for (i=0; (line[i] !=0) && 
              ((line[i] ==' ') || (line[i]=='\n'));i++);

   
    if ((line[i]==0) || (line[i]=='#')) continue;
    tkn=line+i;
    if (max==0) {
      sscanf(tkn,"%d",&max);
      if (max !=0) {
        key->r=malloc(max);
        key->g=malloc(max);  
        key->b=malloc(max);
        memset(key->r,0,max);
        memset(key->g,0,max);
        memset(key->b,0,max);
        key->max=max;
        j=0;
      }
    } else if (j<max) {
      sscanf(tkn,"%x",&colval);
      key->r[j]=(colval>>16) & 0xff;
      key->g[j]=(colval>>8) & 0xff;
      key->b[j]=colval & 0xff;
      j++;
    }
  }
  return max;
}
