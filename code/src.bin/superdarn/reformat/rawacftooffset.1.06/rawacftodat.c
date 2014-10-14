/* rawacftodat.c
   ============= 
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "rawread.h"
#include "oldrawwrite.h"
#include "hlpstr.h"

/*
 $Log: rawacftodat.c,v $
 Revision 1.6  2007/01/31 18:35:59  code
 Fixed bad return.

 Revision 1.5  2007/01/31 18:33:14  code
 Changed exit status to POSIX compliant.

 Revision 1.4  2007/01/31 18:00:05  code
 Added record counter and error on write failure.

 Revision 1.3  2007/01/30 17:24:33  code
 Modification to override the threshold.

 Revision 1.2  2005/08/09 19:20:08  barnes
 Fixed command line options.

 Revision 1.1  2004/05/10 21:31:11  barnes
 Initial revision

*/

#define ORIG_MAX_RANGE 75

struct OptionData opt;

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
  int arg=0;

  int s;
  int thr=-1;

  struct RadarParm rprm;
  struct RawData rawacf;
  FILE *fp;

  int rtab[ORIG_MAX_RANGE];
  float snr[MAX_RANGE];
  int inx,l,step;
  float maxval;
  int cnt=0;

  int recnum=0;

  char vstring[256];
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'i',&thr);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL); 
 
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

 
  if (arg !=argc) { 
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    } 
  } else fp=stdin;

  
  s=RawFread(fp,&rprm,&rawacf);
  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
  if (thr !=-1) rawacf.thr=thr;

  sprintf(vstring,"%d.%.3d",rawacf.revision.major,rawacf.revision.minor);
  if (OldRawHeaderFwrite(stdout,"rawwrite",vstring,rawacf.thr,            
			"example file") !=0) {
    fprintf(stderr,"Error writing header.\n");
    exit(-1);
  }

  do {
    recnum++;

    if (thr !=-1) rawacf.thr=thr;

    for (l=0;l<rprm.nrang;l++) {
      if (rprm.noise.search>0)
        snr[l]=rawacf.pwr0[l]/rprm.noise.search;
      else snr[l]=0;
    }

    if (rprm.nrang>ORIG_MAX_RANGE) {
      step=rprm.nrang/ORIG_MAX_RANGE;
      for (l=0;l<ORIG_MAX_RANGE;l++) {
        maxval=0.0;
        inx=l*step;
        for (s=0;s<step;s++) {
          if (snr[l*step+s]>maxval) {
            maxval=snr[l*step+s];
            inx=l*step+s;
          }
        }
        rtab[l]=inx;
      }
      rprm.rsep=rprm.rsep*step;
      s=OldRawFwrite(stdout,"rawwrite",&rprm,&rawacf,recnum,rtab);  
    } else  s=OldRawFwrite(stdout,"rawwrite",&rprm,&rawacf,recnum,NULL);
    if (s !=0) {
      cnt=-1;
      break;
    }

     if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm.time.yr,
                    rprm.time.mo,rprm.time.dy,rprm.time.hr,rprm.time.mt,
                    rprm.time.sc);

     cnt++;
  } while ((s=RawFread(fp,&rprm,&rawacf)) !=-1);

  if (cnt==-1) exit(EXIT_FAILURE);
  exit(EXIT_SUCCESS);
  return 0;
}



