/* fitacftofit.c
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
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "oldfitwrite.h"

#include "hlpstr.h"

/*
 $Log: fitacftofit.c,v $
 Revision 1.5  2007/01/31 18:37:03  code
 Changed exit status to POSIX compliant.

 Revision 1.4  2007/01/31 17:55:07  code
 Added record count and error on write failure.

 Revision 1.3  2005/08/09 18:32:42  barnes
 Corrected command line options.

 Revision 1.2  2004/10/28 13:53:25  barnes
 Added missing header.

 Revision 1.1  2004/05/10 21:29:48  barnes
 Initial revision

*/

struct OptionData opt;

#define ORIG_MAX_RANGE 75

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
  int arg=0;
  int s;
  struct RadarParm rprm;
  struct FitData fitacf;
  int dnum;
  FILE *fp; 
  char vstr[256];

  int rtab[ORIG_MAX_RANGE];
  float snr[MAX_RANGE];
  int inx,l,step;
  float maxval;

  int cnt=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vb);
 
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
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }  
  } else fp=stdin; 

  s=FitFread(fp,&rprm,&fitacf);
  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }

  sprintf(vstr,"%d.%.3d",fitacf.revision.major,fitacf.revision.minor);

  if (OldFitHeaderWrite(fileno(stdout),"fitacftofit","fitwrite",vstr) ==-1) {
    fprintf(stderr,"Could not write header.\n");
    exit(-1);
  }

  do {


    for (l=0;l<rprm.nrang;l++) {
      if (rprm.noise.search>0)
        snr[l]=fitacf.rng[l].p_0/rprm.noise.search;
      else snr[l]=0;
    }
  

    if (rprm.nrang>ORIG_MAX_RANGE) {
      step=rprm.nrang/ORIG_MAX_RANGE;
      for (l=0;l<ORIG_MAX_RANGE;l++) {
        maxval=0.0;
        inx=l*step;
        for (s=0;s<step;s++) {
	  /*
                    if (fitacf.rng[l*step+s].qflg==0) continue;
                    if (fitacf.rng[l*step+s].gsct !=0) continue;
	  */
	  if (snr[l*step+s]>maxval) {
	    maxval=snr[l*step+s];
	    inx=l*step+s;
	  }
        }
        rtab[l]=inx;
      }
      rprm.rsep=rprm.rsep*step;
      dnum=OldFitFwrite(stdout,&rprm,
			&fitacf,rtab);
    } else dnum=OldFitFwrite(stdout,&rprm,
			     &fitacf,NULL);

    if (dnum==-1) {
      cnt=-1;
      break;
    } 

    if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm.time.yr,
                    rprm.time.mo,rprm.time.dy,rprm.time.hr,rprm.time.mt,
                    rprm.time.sc);

    cnt++;
  } while ((s=FitFread(fp,&rprm,&fitacf)) !=-1);

  if (cnt==-1) exit(EXIT_FAILURE);
  else exit(EXIT_SUCCESS);

  return 0;
}



