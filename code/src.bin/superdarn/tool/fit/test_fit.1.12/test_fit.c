/* test_fit.c
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

/*Demonstration program that opens and reads fit files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "version.h"

#include "fitread.h"
#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"



/* 
 $Log: test_fit.c,v $
 Revision 1.12  2006/07/11 15:58:04  code
 Modification to support the beam azimuth and orgin code.

 Revision 1.11  2005/08/10 19:01:35  barnes
 Added option to dump command line options.

 Revision 1.10  2004/10/28 14:00:41  barnes
 Added missing header.

 Revision 1.9  2004/07/19 18:44:52  barnes
 Fixed bug in reading fit files.

 Revision 1.8  2004/05/10 21:50:11  barnes
 Removed redundant code.

 Revision 1.7  2003/04/07 18:48:04  barnes
 Changed fit header.

 Revision 1.6  2003/02/25 14:52:57  barnes
 Modification to use the new base libraries.

 Revision 1.5  2002/07/18 15:02:56  barnes
 Added code to print the channel and CP number.

 Revision 1.4  2001/06/27 21:07:40  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:12:02  barnes
 Added Author Name

 Revision 1.2  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.1  2000/03/21 00:56:11  barnes
 Initial revision

 */


struct RadarParm prm;
struct FitData fit;
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

  int i;
  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  int c;
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"new",'x',&new); 

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

 
  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
      while (OldFitRead(fitfp,&prm,&fit) !=-1) {
        fprintf(stdout,
	  "%d-%d-%d %d:%d:%d beam=%d bmazm=%f channel=%d cpid=%d scan=%d\n",
               prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
               prm.time.sc,prm.bmnum,prm.bmazm,prm.channel,prm.cp,prm.scan);

        fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm.origin.code,prm.origin.time,prm.origin.command);
        fprintf(stdout,"v:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].v);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }

        fprintf(stdout,"v_e:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].v_err);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"p_l:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].p_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"w_l:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].w_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
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
    while (FitFread(fp,&prm,&fit) !=-1) {
        fprintf(stdout,"%d %d\n",fit.revision.major,fit.revision.minor);
        fprintf(stdout,
	   "%d-%d-%d %d:%d:%d beam=%d  bmazm=%f channel=%d cpid=%d scan=%d\n",
               prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
               prm.time.sc,prm.bmnum,prm.bmazm,prm.channel,prm.cp,prm.scan);
        fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm.origin.code,prm.origin.time,prm.origin.command);
        fprintf(stdout,"combf=%s\n",prm.combf);
        fprintf(stdout,"v:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].v);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"v_e:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].v_err);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"p_l:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].p_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"w_l:\n");
        for (i=0;i<prm.nrang;i++) {
          fprintf(stdout,"%.4g",fit.rng[i].w_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");

    }
    if (fp !=stdin) fclose(fp);
  }


  return 0;
} 






















