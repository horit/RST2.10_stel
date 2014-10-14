/* raw_cp.c
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
#include "rawdata.h"

#include "rawread.h"
#include "oldrawread.h"

#include "hlpstr.h"
#include "errstr.h"

/*
 $Log: raw_cp.c,v $
 Revision 1.1  2007/02/28 15:31:09  code
 Initial revision

*/

int cpid[256];
int cpcnt=0;

struct RadarParm prm;
struct RawData raw;
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

  struct OldRawFp *rawfp=NULL;
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
      rawfp=OldRawOpen(argv[c],NULL); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (rawfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
      while (OldRawRead(rawfp,&prm,&raw) !=-1) {
        for (i=0;i<cpcnt;i++) if (cpid[i]==prm.cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm.cp;
          cpcnt++;
        } 
      } 
      OldRawClose(rawfp);
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
    while (RawFread(fp,&prm,&raw) !=-1) {
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


















