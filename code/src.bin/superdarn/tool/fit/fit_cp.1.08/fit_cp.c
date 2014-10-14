/* fit_cp.c
   ========
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
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "oldfitread.h"

#include "hlpstr.h"
#include "errstr.h"

/* 
 $Log: fit_cp.c,v $
 Revision 1.9  2007/02/22 16:48:39  code
 Fixed bug in reading regular fit files.

 Revision 1.8  2007/02/09 18:14:12  code
 Fixed missing switch for old and new file formats.

 Revision 1.7  2006/09/15 19:27:25  code
 Fixed unitialized variable.

 Revision 1.6  2005/08/10 18:31:27  barnes
 Added option to dump out command line options.

 Revision 1.5  2004/10/28 13:58:38  barnes
 Added missing header.

 Revision 1.4  2004/05/10 21:35:35  barnes
 Removed redundant code.

 Revision 1.3  2003/04/07 18:51:07  barnes
 Changed fit header.

 Revision 1.2  2003/02/25 21:27:30  barnes
 Modification to use the base libraries.

 Revision 1.1  2002/07/18 15:16:26  barnes
 Initial revision

 */



int cpid[256];
int cpcnt=0;

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

  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  int c,i;
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  old=!new;


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
        for (i=0;i<cpcnt;i++) if (cpid[i]==prm.cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm.cp;
          cpcnt++;
        } 
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
      for (i=0;i<cpcnt;i++) if (cpid[i]==prm.cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm.cp;
          cpcnt++;
        } 
      }
    if (fp !=stdin) fclose(fp);
  } 
  fprintf(stdout,"%d\n",cpcnt);
  for (i=0;i<cpcnt;i++) fprintf(stdout,"%.2d:%d\n",i,cpid[i]);
  exit(cpcnt);

  return cpcnt;
} 






















