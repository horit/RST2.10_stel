/* cat_fit.c
   =========
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
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: cat_fit.c,v $
 Revision 1.12  2005/08/10 18:20:18  barnes
 Added option to dump command line options.

 Revision 1.11  2004/05/10 21:32:10  barnes
 Removed redundant code.

 Revision 1.10  2003/04/07 18:43:02  barnes
 Changed fit header.

 Revision 1.9  2003/02/25 16:11:25  barnes
 Modification to use the new base libraries.

 Revision 1.8  2001/06/27 21:06:45  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.6  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.5  1999/04/10 19:12:05  barnes
 Fixed problem with help message.

 Revision 1.4  1999/04/10 19:01:16  barnes
 Added help message system.

 Revision 1.3  1999/03/16 15:12:28  barnes
 Modifications to support the new fit library.

 Revision 1.2  1998/11/09 22:54:25  barnes
 Incorporated rawdata header.

 Revision 1.1  1998/07/14 16:44:48  barnes
 Initial revision

*/


struct RadarParm pbuf[2];
struct FitData fbuf[2];

struct RadarParm *pptr;
struct FitData *fptr;
int fnum=0;

struct OptionData opt;


int main (int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char new=0;

  int i;
  
  struct OldFitFp *fp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  int irec=1;
  int drec=2;
  
  unsigned char inx=0; 
  int dnum=0;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"i",'x',&inx);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (new==1) {
    fprintf(stderr,
	    "New format files can be concatenated using the cat command\n");
    exit(-1);
  }

  if (argc-arg<2+inx) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
      	  
  fitfp=fopen(argv[argc-1-inx],"w");
  if (fitfp==NULL) {
    fprintf(stderr,"Could not create fit file.\n");
    exit(-1);
  }
  OldFitHeaderFwrite(fitfp,"cat_fit","fitacf","4.0");

  if (inx !=0) {
    inxfp=fopen(argv[argc-1],"w");
    if (inxfp==NULL) fprintf(stderr,"Could not create index file.\n");
  }
 
  for (i=arg;i<argc-1-inx;i++) {
    fp=OldFitOpen(argv[i],NULL); 
    if (fp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[i]);
      continue;
    }
    while (OldFitRead(fp,&pbuf[fnum],&fbuf[fnum]) !=-1) {
      fptr=&fbuf[fnum];
      pptr=&pbuf[fnum];
      fnum=(fnum+1) % 2;
      if ((inxfp !=NULL) && (irec==1)) OldFitInxHeaderFwrite(inxfp,pptr);
      dnum=OldFitFwrite(fitfp,pptr,fptr,NULL);
      if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,pptr);
      drec+=dnum;
      irec++;
    } 
    OldFitClose(fp);
  }
  if (inxfp !=NULL) {
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    OldFitInxFclose(inxfp,pptr,irec-1);
    fclose(inxfp);
  }
  fclose(fitfp);
  return 0;
} 






















