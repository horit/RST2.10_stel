/* make_csd.c
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




/* This task is designed to compress a fit file*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "csddata.h"
#include "csdwrite.h"

#include "cfitread.h"
#include "fitread.h"
#include "cfitcsd.h"
#include "fitcsd.h"
#include "csdwrite.h"

#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"
#include "default.h"

/*
 $Log: make_csd.c,v $
 Revision 1.3  2005/08/10 17:17:29  barnes
 Added option to dump command line options.

 Revision 1.2  2004/10/28 13:55:57  barnes
 Added missing header.

 Revision 1.1  2004/05/10 21:40:09  barnes
 Initial revision

*/

struct RadarParm prm;
struct FitData fit;
struct CFitdata cfit;
struct CSDdata csd;

struct OptionData opt;

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
  unsigned char aflg=0;

  int c;
  FILE *fp;
  struct OldFitFp *fitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  double min_pwr=3.0;
  int sflag=0x0;
  int bmnum=-1;
  char prtxt[8]={""};
  int rnum=0;  
 
  double pmax=-1,vmax=-1,wmax=-1;
  
  unsigned char fitflg=0;
  unsigned char cfitflg=0;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'d',&min_pwr);
  OptionAdd(&opt,"b",'i',&bmnum);
  OptionAdd(&opt,"a",'x',&aflg);

  OptionAdd(&opt,"p",'d',&pmax);
  OptionAdd(&opt,"v",'d',&vmax);
  OptionAdd(&opt,"w",'d',&wmax);

  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"cfit",'x',&cfitflg);


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


  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (cfitflg==0) fitflg=1;

  if (min_pwr<0) min_pwr=0;
  if (min_pwr>30) min_pwr=30;
  if (bmnum>15) bmnum=15;

  if (aflg) sflag |=0x07;
  if (pmax !=-1) sflag=sflag | 0x01;
  if (vmax !=-1) sflag=sflag | 0x02;
  if (wmax !=-1) sflag=sflag | 0x04;

  if (pmax==-1) pmax=DEFAULT_PMAX;
  if (vmax==-1) vmax=DEFAULT_VMAX;
  if (wmax==-1) wmax=DEFAULT_WMAX;

  if (sflag & 0x01) strcpy(prtxt,"p");
  if (sflag & 0x02) strcat(prtxt,"v");
  if (sflag & 0x03) strcat(prtxt,"w");


  for (c=arg;c<argc;c++) {
    if (fitflg) {
      if (old) {
        fitfp=OldFitOpen(argv[c],NULL); 
        fprintf(stderr,"Opening file %s\n",argv[c]);
        if (fitfp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }
 
        while (OldFitRead(fitfp,&prm,&fit) !=-1) {

          if ((bmnum !=-1) && (prm.bmnum != bmnum)) continue; 
          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d recording:%s\n",
	  		   prm.time.yr,
			   prm.time.mo,prm.time.dy,prm.time.hr,
			   prm.time.mt,prm.time.sc,prm.bmnum,prtxt);
          FitToCSD(sflag,min_pwr,pmax,vmax,wmax,&prm,&fit,&csd);  
      
          if (rnum==0) CSDHeaderFwrite(stdout,&csd);
          CSDFwrite(stdout,&csd);
          rnum++;
        } 
        OldFitClose(fitfp);
      } else {
        fp=fopen(argv[c],"r");
        if (fp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }
        while (FitFread(fp,&prm,&fit) !=-1) {

          if ((bmnum !=-1) && (prm.bmnum != bmnum)) continue; 
          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d recording:%s\n",
	  		   prm.time.yr,
			   prm.time.mo,prm.time.dy,prm.time.hr,
			   prm.time.mt,prm.time.sc,prm.bmnum,prtxt);
          FitToCSD(sflag,min_pwr,pmax,vmax,wmax,&prm,&fit,&csd);  
      
          if (rnum==0) CSDHeaderFwrite(stdout,&csd);
          CSDFwrite(stdout,&csd);
          rnum++;
        } 
        fclose(fp);
      }
    } else {
      cfitfp=CFitOpen(argv[c]); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (cfitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
 
      while (CFitRead(cfitfp,&cfit) !=-1) {
        if ((bmnum !=-1) && (prm.bmnum != bmnum)) continue; 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d recording:%s\n",
	  		   prm.time.yr,
			   prm.time.mo,prm.time.dy,prm.time.hr,
			   prm.time.mt,prm.time.sc,prm.bmnum,prtxt);
        CFitToCSD(sflag,min_pwr,pmax,vmax,wmax,&cfit,&csd);  
        if (rnum==0) CSDHeaderFwrite(stdout,&csd);
        CSDFwrite(stdout,&csd);
        rnum++;
      } 
      CFitClose(cfitfp);
    }

  }

  return 0;
} 






















