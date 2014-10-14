/* make_cfit.c
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "option.h"
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "fitread.h"
#include "fitcfit.h"
#include "cfitwrite.h"

#include "oldfitread.h"

#include "hlpstr.h"
#include "errstr.h"

struct RadarParm prm;
struct FitData fit;
struct CFitdata cfit;

struct OptionData opt;

/*
 $Log: make_cfit.c,v $
 Revision 1.6  2007/03/20 21:58:51  code
 Added hidden function to restrict a file to a single day.

 Revision 1.5  2006/08/18 17:50:20  code
 Set default filtering to zero.

 Revision 1.4  2006/01/10 15:14:32  barnes
 Changed behaviour for new file formats to allow concatenation on the
 command line.

 Revision 1.3  2005/08/10 16:49:40  barnes
 Added option to dump the command line options.

 Revision 1.2  2004/10/28 13:55:18  barnes
 Added missing header.

 Revision 1.1  2004/05/10 21:38:26  barnes
 Initial revision

*/


double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}


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
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;

  char *dtestr=NULL;
  double dtval=-1;

  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  int c;
  double minpwr=0.0; /* default behaviour is not to filter */

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"p",'d',&minpwr);

  OptionAdd(&opt,"d",'t',&dtestr); /* hidden d option to filter out data
                                      not from this day 
				   */
 
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


  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  if (dtestr !=NULL) dtval=strdate(dtestr);


  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
 
      while (OldFitRead(fitfp,&prm,&fit) !=-1) {
        FitToCFit(minpwr,&cfit,&prm,&fit);
        if (  (dtval !=-1) && 
	      ( ((cfit.time-dtval)<0) || ((cfit.time-dtval)>24*3600) )) {
          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d excluded\n",
                   prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
                   prm.time.sc);
          continue;
	}  

        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d ranges=%d\n",
                   prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
                   prm.time.sc,prm.bmnum,cfit.num);

        CFitFwrite(stdout,&cfit);
      } 
      OldFitClose(fitfp);
    }
  } else {
    if (arg==argc) {
      while (FitFread(stdin,&prm,&fit) !=-1) {
        FitToCFit(minpwr,&cfit,&prm,&fit);
        if (  (dtval !=-1) && 
	      ( ((cfit.time-dtval)<0) || ((cfit.time-dtval)>24*3600) )) {
          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d excluded\n",
                   prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
                   prm.time.sc);
          continue;
	}  

   
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d ranges=%d\n",
                   prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
                   prm.time.sc,prm.bmnum,cfit.num);

        CFitFwrite(stdout,&cfit);
      }
    } else {
      for (c=arg;c<argc;c++) {
        fprintf(stderr,"Opening file %s\n",argv[c]);
        fp=fopen(argv[c],"r");
        if (fp==NULL) {
          fprintf(stderr,"Could not open file.\n");
          continue;
	}
        while (FitFread(fp,&prm,&fit) !=-1) {
          FitToCFit(minpwr,&cfit,&prm,&fit);

          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d ranges=%d\n",
                   prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
                   prm.time.sc,prm.bmnum,cfit.num);

          CFitFwrite(stdout,&cfit);
	}
        fclose(fp);
      }
    }
  }
  return 0;
} 






















