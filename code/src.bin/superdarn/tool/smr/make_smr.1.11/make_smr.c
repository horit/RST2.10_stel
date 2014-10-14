/* make_smr.c
   ==========
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "smrwrite.h"
#include "fitread.h"

#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm prm;
struct FitData fit;
struct OptionData opt;

/* 
 $Log: make_smr.c,v $
 Revision 1.12  2005/08/10 15:47:53  barnes
 Added option to dump the command line options.

 Revision 1.11  2004/10/28 14:04:08  barnes
 Added missing header.

 Revision 1.10  2004/05/10 21:47:30  barnes
 Removed redundant code.

 Revision 1.9  2003/04/25 14:52:31  barnes
 Added missing header.

 Revision 1.8  2003/02/25 18:51:16  barnes
 Modifications to use the base libraries.

 Revision 1.7  2001/07/24 12:48:53  barnes
 Modification to take input from standard input.

 Revision 1.6  2001/06/27 21:07:22  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.4  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.3  2000/04/25 20:06:59  barnes
 Moved "opening file message" to verbose only mode.

 Revision 1.2  1999/04/10 19:16:17  barnes
 Added help message system.

 Revision 1.1  1999/03/18 16:35:12  barnes
 Initial revision

 */

int main (int argc,char *argv[]) {

 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  int arg;
  int i;
  int c=1;
  char *bstr=NULL;
  struct OldFitFp *fitfp=NULL;
  FILE *fp;
  double min_pwr=3.0;
  char bflg[16];
  int bnum=0,b;
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'d',&min_pwr);
  OptionAdd(&opt,"b",'t',&bstr); 

  arg=OptionProcess(1,argc,argv,&opt,NULL);   
  old=!new;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (min_pwr<0) min_pwr=0;
  if (min_pwr>30) min_pwr=30;

  for (i=0;i<16;i++) bflg[i]=0;
  bnum=0;

  if (bstr !=NULL) {
    char *tmp;
    tmp=strtok(bstr,",");
    do {
       b=atoi(tmp);
       if (bflg[b]==0) {
         bflg[b]=1;
         bnum++;
       }
     } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  if ((old) && (arg==argc)) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }
    
  if (bnum==0) {
     bflg[8]=1;
     bnum=1;
  }
  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      if (vb) fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        if (vb) fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
 
      SmrHeaderFwrite(stdout,"1.00","make_smr","make_smr");
    

      while (OldFitRead(fitfp,&prm,&fit) !=-1) {

        if (bflg[prm.bmnum]==0) continue;
 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
		      prm.time.dy,prm.time.mo,prm.time.dy,prm.time.hr,
                      prm.time.mt,prm.time.sc,prm.bmnum);
        SmrFwrite(stdout,&prm,&fit,min_pwr);            
      } 
      OldFitClose(fitfp);
    }
  } else {
    if (arg==argc) fp=stdin;
    else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"Could not open file.\n");
        exit(-1);
      }
    }
    SmrHeaderFwrite(stdout,"1.00","make_smr","make_smr");
    while (FitFread(fp,&prm,&fit) !=-1) {
      if (bflg[prm.bmnum]==0) continue;
 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
		      prm.time.dy,prm.time.mo,prm.time.dy,prm.time.hr,
                      prm.time.mt,prm.time.sc,prm.bmnum);
        SmrFwrite(stdout,&prm,&fit,min_pwr);            
      }
    if (fp !=stdin) fclose(fp);
  } 
  return 0;
} 






















