/* make_efieldinx.c
   ================
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: make_efieldinx.c,v $
 Revision 1.3  2005/08/10 22:00:22  barnes
 Added option to dump command line options.

 Revision 1.2  2004/12/04 20:26:35  barnes
 Fixed missing header.

 Revision 1.1  2004/11/19 14:15:15  barnes
 Initial revision

*/


struct OptionData opt;

int main (int argc,char *argv[]) {

  struct DataMap *ptr;
  struct DataMapScalar *s;
  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
 
  FILE *fp;
  
  int sze=0,c,x;
  int yr,mo,dy,hr,mt;
  double sc,tval; 

  char *sname[]={"start.year","start.month","start.day","start.hour",
                 "start.minute","start.second",NULL};
  int stype[]={DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATASHORT,DATADOUBLE};
  struct DataMapScalar *sdata[6];


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

    
  while ((ptr=DataMapFread(fp)) !=NULL) {
    
    for (c=0;sname[c] !=0;c++) sdata[c]=NULL;
    for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    for (x=0;sname[x] !=0;x++) 
      if ((strcmp(s->name,sname[x])==0) && (s->type==stype[x])) {
        sdata[x]=s;
        break;
      }
    }
    for (x=0;sname[x] !=0;x++) if (sdata[x]==NULL) break;
    if (sdata[x]==NULL) break;
   
    yr=*(sdata[0]->data.sptr);
    mo=*(sdata[1]->data.sptr);
    dy=*(sdata[2]->data.sptr);
    hr=*(sdata[3]->data.sptr);
    mt=*(sdata[4]->data.sptr);
    sc=*(sdata[5]->data.dptr);
    tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);


    if (vb) {
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
              yr,mo,dy,hr,mt,(int) sc);
    }
    ConvertFwriteDouble(stdout,tval);
    ConvertFwriteInt(stdout,sze);
    
    sze+=DataMapSize(ptr);
    DataMapFree(ptr);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















