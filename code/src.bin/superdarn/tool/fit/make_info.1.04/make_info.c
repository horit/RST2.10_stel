/* make_info.c
   ===========*/



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
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"

#include "cfitread.h"
#include "fitread.h"
#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"


/*
 $Log: make_info.c,v $
 Revision 1.6  2005/08/10 18:57:21  barnes
 Added option to dump command line options.

 Revision 1.5  2004/10/28 14:00:00  barnes
 Added missing header.

 Revision 1.4  2004/08/30 15:33:59  barnes
 Fixed bug in opening old data files

 Revision 1.3  2004/05/10 21:44:24  barnes
 Removed redundant code.

 Revision 1.2  2003/04/07 19:00:20  barnes
 Changed fit header.

 Revision 1.1  2003/02/25 19:01:07  barnes
 Initial revision

*/

struct RadarParm prm;
struct FitData fit;
struct CFitdata cfit;
struct OptionData opt;


int main(int argc,char *argv[]) {

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

  int gsct,isct,i,c;
  struct OldFitFp *fitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  FILE *fp=NULL;

  int yr,mo,dy,hr,mt,isc;
  double dsc;
  int us;

  unsigned char fitflg=0;
  unsigned char cfitflg=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

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



  if (cfitflg==0) fitflg=1;


  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  if (fitflg) {
    if (old) {
      for (c=arg;c<argc;c++) {
        fitfp=OldFitOpen(argv[c],NULL); 
        fprintf(stderr,"Opening file %s\n",argv[c]);
        if (fitfp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[c]);
          continue;
        }

        while (OldFitRead(fitfp,&prm,&fit) !=-1) {
   
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
                 prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
	        prm.time.sc,prm.time.us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",prm.stid,
                prm.bmnum,prm.channel,
                 prm.scan); 
        fprintf(stdout,"%8d %3d %3d",prm.cp,prm.intt.sc,prm.intt.us);
        fprintf(stdout,"%5d %5d ",prm.frang,prm.rsep);
        fprintf(stdout,"%6d %6g ",prm.tfreq,prm.noise.search);
        fprintf(stdout,"%2d %4d ",prm.atten,prm.nave);
        fprintf(stdout,"%3d",prm.nrang);

        gsct=0;
        isct=0;
        for (i=0;i<prm.nrang;i++) {
            if (fit.rng[i].qflg==1) {
              if (fit.rng[i].gsct==1) gsct++;
              else isct++;
            }
          }
          fprintf(stdout,"%3d %3d ",gsct,isct);
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
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
                 prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
	        prm.time.sc,prm.time.us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",prm.stid,
                prm.bmnum,prm.channel,
                prm.scan); 
        fprintf(stdout,"%8d %3d %3d",prm.cp,prm.intt.sc,prm.intt.us);
        fprintf(stdout,"%5d %5d ",prm.frang,prm.rsep);
        fprintf(stdout,"%6d %6g ",prm.tfreq,prm.noise.search);
        fprintf(stdout,"%2d %4d ",prm.atten,prm.nave);
        fprintf(stdout,"%3d",prm.nrang);

        gsct=0;
        isct=0;
        for (i=0;i<prm.nrang;i++) {
            if (fit.rng[i].qflg==1) {
              if (fit.rng[i].gsct==1) gsct++;
              else isct++;
            }
          }
          fprintf(stdout,"%3d %3d ",gsct,isct);
          fprintf(stdout,"\n");
      }
      if (fp !=stdin) fclose(fp);
    }   
  } else {
    for (c=arg;c<argc;c++) {
      cfitfp=CFitOpen(argv[c]);
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fp==NULL) {
	fprintf(stderr,"file %s not found\n",argv[c]);
	continue;
      }

      while (CFitRead(cfitfp,&cfit) !=-1) {
        TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&dsc);
        isc=(int) dsc;
        us=1.0e6*(dsc-isc);
	fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2d ",
		yr,mo,dy,hr,mt,isc,us);
        fprintf(stdout,"%.2d %.2d %.1d %4d ",cfit.stid,
                cfit.bmnum,cfit.channel,
                cfit.scan);
        fprintf(stdout,"%8d %3d %3d",cfit.cp,cfit.intt.sc,cfit.intt.us);
        fprintf(stdout,"%5d %5d ",cfit.frang,cfit.rsep);
        fprintf(stdout,"%6d %6d ",cfit.tfreq,cfit.noise);
        fprintf(stdout,"%2d %4d ",cfit.atten,cfit.nave);
        fprintf(stdout,"%3d",cfit.nrang);

        gsct=0;
        isct=0;
        for (i=0;i<cfit.num;i++) {
	  if (cfit.data[i].gsct==1) gsct++;
	  else isct++;
	  
	}
	fprintf(stdout,"%3d %3d ",gsct,isct);
	fprintf(stdout,"\n");
      }
      CFitClose(cfitfp);
    }
  }


  return 0;
}
   









