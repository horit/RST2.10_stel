/* make_fit.c
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
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#include "rtypes.h"
#include "option.h"

#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "oldfitacf.h"
#include "fitacf.h"
#include "rawread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: make_fit.c,v $
 Revision 1.19  2006/07/13 18:58:41  code
 Added origin flag.

 Revision 1.18  2005/08/10 18:37:41  barnes
 Added option to dump out command line options.

 Revision 1.17  2005/06/30 16:53:28  barnes
 Changed logging so that debugging FitACF is easier.

 Revision 1.16  2004/07/24 00:23:42  barnes
 Added missing header.

 Revision 1.15  2004/07/24 00:21:42  barnes
 Added the ability to run using the original version of the FitACF algorithm.

 Revision 1.14  2004/05/10 21:40:40  barnes
 Removed redundant code.

*/

struct RadarParm prm;
struct RawData raw;
struct FitData fit;
struct FitBlock fblk;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct OptionData opt;

int main(int argc,char *argv[]) {

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  unsigned char old=0;
  unsigned char new=0;

  char *envstr;
  int status;
  int arg;
  unsigned char original=0;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  FILE *fp=NULL;
  struct OldRawFp *rawfp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;  
  int irec=1;
  int drec=2;
  int dnum=0;

  time_t ctime;
  int c,n;
  char command[128];
 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"original",'x',&original);
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




  if ((old) && (argc-arg<2)) {
    OptionPrintInfo(stdout,hlpstr);
    exit(-1);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);
  

  if (old) {
     rawfp=OldRawOpen(argv[arg],NULL);
     if (rawfp==NULL) {
       fprintf(stderr,"File not found.\n");
       exit(-1);
     }
     status=OldRawRead(rawfp,&prm,&raw);  
  } else { 
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");

    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    status=RawFread(fp,&prm,&raw);
  }

  radar=RadarGetRadar(network,prm.stid);
  if (radar==NULL) {
    fprintf(stderr,"Failed to get radar information.\n");
    exit(-1);
  }

  site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);

  if (site==NULL) {
    fprintf(stderr,"Failed to get site information.\n");
    exit(-1);
  }


  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }



  if (vb) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm.time.yr,prm.time.mo,
	     prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum);


  if (original) {
    OldFitACFStart(site,prm.time.yr,&fblk);
    OldFitACF(&prm,&raw,&fblk,&fit);  
  } else {
    FitACFStart(site,prm.time.yr,&fblk); 
    FitACF(&prm,&raw,&fblk,&fit);
  }
  
  if (old) {
    char vstr[256];
    fitfp=fopen(argv[arg+1],"w");
    if (fitfp==NULL) {
      fprintf(stderr,"Could not create fit file.\n");
      exit(-1);
    } 
    if (argc-arg>2) {
      inxfp=fopen(argv[arg+2],"w");
      if (inxfp==NULL) {
        fprintf(stderr,"Could not create inx file.\n");
        exit(-1);
      }
    }
    sprintf(vstr,"%d.%d",fit.revision.major,fit.revision.minor);
    OldFitHeaderFwrite(fitfp,"make_fit","fitacf",vstr);
    if (inxfp !=NULL) OldFitInxHeaderFwrite(inxfp,&prm);
  }


  
  do {


    ctime = time((time_t) 0);
    strcpy(prm.origin.command,command);
    strcpy(prm.origin.time,asctime(gmtime(&ctime)));
    prm.origin.time[24]=0;
  
    if (old) {
       dnum=OldFitFwrite(fitfp,&prm,&fit,NULL);
       if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,&prm);
       drec+=dnum;
       irec++;
    } else status=FitFwrite(stdout,&prm,&fit);
    
    if (old) status=OldRawRead(rawfp,&prm,&raw);
    else status=RawFread(fp,&prm,&raw);

     if (vb) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm.time.yr,prm.time.mo,
	     prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum);


    if (status==0) {
      if (original) OldFitACF(&prm,&raw,&fblk,&fit);
      else FitACF(&prm,&raw,&fblk,&fit);
    }
  
  } while (status==0);
  
  if (original) OldFitACFEnd(&fblk);
  else FitACFEnd(&fblk);
  if (old) OldRawClose(rawfp);
  return 0;
}













