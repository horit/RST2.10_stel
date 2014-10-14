/* image_table.c
   =============
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
#include "rtime.h"
#include "image_table.h"

/*
 $Log: image_table.c,v $
 Revision 1.1  2003/03/15 00:13:53  barnes
 Initial revision

*/

int find_image(struct imgtable *ptr,double tval) {
  int i;
  for (i=0;i<ptr->num;i++) {
    if (ptr->img[i].st_time > tval) break;
  }
  return i;
}


struct imgtable *read_imgtable(FILE *fp) {
  char *delim=" \t\n";
  int yr,mo,dy,hr,mt;
  double sc;
  int n,i;

  struct imgtable *ptr;
  struct imgtime *tmp;
 
  char line[1024];
  char *tkn;
  char *fname=NULL;
  ptr=malloc(sizeof(struct imgtable));
  ptr->num=0;
  ptr->img=NULL;

  while (fgets(line,1024,fp) !=0) {
  
    for (i=0; (line[i] !=0) && 
              ((line[i] ==' ') || (line[i]=='\n'));i++);

   
    if ((line[i]==0) || (line[i]=='#')) continue;
    tkn=line+i;
    n=ptr->num;
    if (ptr->img==NULL) tmp=malloc(sizeof(struct imgtime));
    else tmp=realloc(ptr->img,sizeof(struct imgtime)*(n+1));

    yr=atoi(strtok(tkn,delim));
    mo=atoi(strtok(NULL,delim));
    dy=atoi(strtok(NULL,delim));
    hr=atoi(strtok(NULL,delim));
    mt=atoi(strtok(NULL,delim));
    sc=atof(strtok(NULL,delim));

    tmp[n].st_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

    yr=atoi(strtok(NULL,delim));
    mo=atoi(strtok(NULL,delim));
    dy=atoi(strtok(NULL,delim));
    hr=atoi(strtok(NULL,delim));
    mt=atoi(strtok(NULL,delim));
    sc=atof(strtok(NULL,delim));

    tmp[n].ed_time=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    fname=strtok(NULL,delim);
    tmp[n].fname=malloc(strlen(fname)+1);
    strcpy(tmp[n].fname,fname);
    ptr->img=tmp;
    ptr->num++;
  }
  return ptr;
}

