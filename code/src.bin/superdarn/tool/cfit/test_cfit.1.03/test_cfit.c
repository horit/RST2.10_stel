/* test_cfit.c
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

/*Demonstration program that opens and reads fit files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "limit.h"
#include "cfitdata.h"
#include "version.h"

#include "cfitread.h"

#include "errstr.h"
#include "hlpstr.h"



/* 
 $Log: test_cfit.c,v $
 Revision 1.3  2006/08/18 18:02:17  code
 Added beam azimuth.

 Revision 1.2  2005/08/10 17:10:35  barnes
 Added option to dump the command line options.

 Revision 1.1  2004/05/10 21:49:32  barnes
 Initial revision

*/


struct CFitdata cfit;
struct OptionData opt;

int main (int argc,char *argv[]) {

  int arg;
  int yr,mo,dy,hr,mt;
  double sc;
  unsigned char help=0;
  unsigned char option=0;

  int i;
  struct CFitfp *cfitfp=NULL;
  int c;
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

 
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (arg==argc) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  for (c=arg;c<argc;c++) {
    cfitfp=CFitOpen(argv[c]); 
    fprintf(stderr,"Opening file %s\n",argv[c]);
    if (cfitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[c]);
      continue;
    }
    while (CFitRead(cfitfp,&cfit) !=-1) {
      TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,
        "%d-%d-%d %d:%d:%d beam=%d bmazm=%f channel=%d cpid=%d scan=%d\n",
         yr,mo,dy,hr,mt,(int) sc,cfit.bmnum,cfit.bmazm,
                        cfit.channel,cfit.cp,cfit.scan);

      fprintf(stdout,"v:\n");
      for (i=0;i<cfit.num;i++) {
        fprintf(stdout,"%.2d\t%.4g",cfit.rng[i],cfit.data[i].v);
        if (((i+1) % 4)==0) fprintf(stdout,"\n");
        else fprintf(stdout,"\t");
      }
      fprintf(stdout,"\n");
    } 
    CFitClose(cfitfp);
  }

  return 0;
} 






















