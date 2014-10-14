/* make_fitinx.c
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "dmap.h"
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitwrite.h"
#include "fitindex.h"

#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: make_fitinx.c,v $
 Revision 1.4  2005/08/10 18:49:18  barnes
 Added option to dump the command line options.

 Revision 1.3  2004/10/28 13:59:17  barnes
 Added missing header.

 Revision 1.2  2004/06/22 17:52:19  barnes
 Changed references of Long to Int.

 Revision 1.1  2004/05/10 21:42:54  barnes
 Initial revision

*/


struct RadarParm pbuf[2];
struct FitData fbuf[2];

struct FitData *fptr;
struct RadarParm *pptr;

int fnum=0;

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

  int i=0;

 
  FILE *fp;
  struct RadarParm prm;
  struct FitData fit;
  int sze=0;
  double tval;

  struct OldFitFp *fitfp=NULL;
  FILE *inxfp=NULL;
  
  int irec=1;
  int drec=2;
  int dnum=0;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

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


  if (old) {
    if (argc-arg<2) {
      OptionPrintInfo(stdout,errstr);
      exit(-1);
    }
     	  
    inxfp=fopen(argv[argc-1],"w");
    if (inxfp==NULL) {
      fprintf(stderr,"could not create index file.\n");
      exit(-1);  
    }

  
    fitfp=OldFitOpen(argv[argc-2],NULL); 
    if (fitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[i]);
      exit(-1);
    }

 
    while (OldFitRead(fitfp,&pbuf[fnum],&fbuf[fnum]) !=-1) {
      fptr=&fbuf[fnum];
      pptr=&pbuf[fnum];
      fnum=(fnum+1) % 2;
      if (irec==1) OldFitInxHeaderFwrite(inxfp,pptr);
      dnum=OldFitFwrite(NULL,pptr,fptr,NULL);
      OldFitInxFwrite(inxfp,drec,dnum,pptr);
      drec+=dnum;
      irec++;
      if (vb) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",pptr->time.yr,
             pptr->time.mo,pptr->time.dy,pptr->time.hr,pptr->time.mt,
             pptr->time.sc,pptr->bmnum);


    } 
    OldFitClose(fitfp);
  
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    OldFitInxFclose(inxfp,pptr,irec-1);
    fclose(inxfp);
  } else {
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    
    while (FitFread(fp,&prm,&fit) !=-1) {
      if (vb) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm.time.yr,prm.time.mo,
	     prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum);

      tval=TimeYMDHMSToEpoch(prm.time.yr,
		             prm.time.mo,
                             prm.time.dy,
                             prm.time.hr,
		             prm.time.mt,
                             prm.time.sc+prm.time.us/1.0e6);
      ConvertFwriteDouble(stdout,tval);
      ConvertFwriteInt(stdout,sze);
      sze+=FitWrite(-1,&prm,&fit);
    }
    if (fp !=stdin) fclose(fp);
  }   




  return 0;
} 






















