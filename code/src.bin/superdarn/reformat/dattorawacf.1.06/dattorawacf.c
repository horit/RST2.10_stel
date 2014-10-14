/* dattorawacf.c
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
#include <string.h>
#include <time.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "rawwrite.h"
#include "radar.h" 



#include "oldrawread.h"

#include "errstr.h"
#include "hlpstr.h"




/*
 $Log: dattorawacf.c,v $
 Revision 1.6  2007/01/31 18:37:52  code
 Changed exit status to POSIX compliant.

 Revision 1.5  2007/01/31 17:51:36  code
 Added record count and error on write failure.

 Revision 1.4  2007/01/30 17:17:40  code
 Added the ability to override the thresholding.

 Revision 1.3  2006/07/11 15:56:46  code
 Modification to support the beam azimuth and orgin code.

 Revision 1.2  2005/08/09 18:19:56  barnes
 Corrected handling of the command line options.

 Revision 1.1  2004/05/10 21:28:24  barnes
 Initial revision

*/  

struct OptionData opt;
struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;



int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;
  float thr=-1;
  int arg=0;
  int s;

  struct OldRawFp *rawfp=NULL;
  struct RadarParm rprm;
  struct RawData rawacf;


  time_t ctime;
  int c,n;
  char command[128];
 
  char *envstr=NULL;
  FILE *fp;
 
  float offset;
  int cnt=0;

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

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"t",'f',&thr);
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


  if (arg==argc) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  rawfp=OldRawOpen(argv[arg],NULL);

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }


  while ((s=OldRawRead(rawfp,&rprm,&rawacf)) !=-1) {

    rprm.origin.code=1;
    ctime = time((time_t) 0);
    strcpy(rprm.origin.command,command);
    strcpy(rprm.origin.time,asctime(gmtime(&ctime)));
    rprm.origin.time[24]=0;

    radar=RadarGetRadar(network,rprm.stid);
    site=RadarYMDHMSGetSite(radar,rprm.time.yr,
                    rprm.time.mo,rprm.time.dy,rprm.time.hr,rprm.time.mt,
			   rprm.time.sc);

    /* calculate beam azimuth */

    offset=site->maxbeam/2.0-0.5;
    rprm.bmazm=site->boresite+site->bmsep*(rprm.bmnum-offset);
    if (thr !=-1) rawacf.thr=thr;

    s=RawFwrite(stdout,&rprm,&rawacf);
    if (s==-1) {
      cnt=-1;
      break;
    } 
    if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm.time.yr,
                    rprm.time.mo,rprm.time.dy,rprm.time.hr,rprm.time.mt,
                    rprm.time.sc);

    cnt++;  
  }
  if (cnt==-1) exit(EXIT_FAILURE);
  exit(EXIT_SUCCESS); 
  return 0;
}



