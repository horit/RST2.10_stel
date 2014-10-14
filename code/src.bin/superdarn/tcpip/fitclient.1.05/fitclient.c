/* fitclient.c
   ===========
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "rtypes.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "oldfitcnx.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: fitclient.c,v $
 Revision 1.5  2007/03/16 17:32:52  code
 Fixed option dump.

 Revision 1.4  2005/08/10 14:14:49  barnes
 Added option to dump the command line options.

 Revision 1.3  2004/06/21 22:56:11  barnes
 Added missing header.

 Revision 1.2  2004/05/10 21:10:42  barnes
 Removed redundant code.

 */

struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;

  int sock;
  int remote_port=0;
  char host[256];
  int flag,status;
  struct RadarParm prm;
  struct FitData fit;

  int sval=0;
  unsigned char stream;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"s",'i',&sval);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (argc-arg<2) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }
  
  strcpy(host,argv[argc-2]);
  remote_port=atoi(argv[argc-1]);
  stream=sval;

 
  sock=ConnexOpen(host,remote_port); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  
  status=(write(sock,&stream,sizeof(char)) !=1);
  if (status !=0) {
    fprintf(stderr,"Could not set stream to host.\n");
    exit(-1);
  }
  


  do {
   status=OldFitCnxRead(1,&sock,&prm,&fit,&flag,NULL);
   if (status==-1) break;
   if (flag !=-1) {
      int i;
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n", 
              prm.time.yr,prm.time.mo,prm.time.dy,
	      prm.time.hr,prm.time.mt,prm.time.sc);
      fprintf(stderr,"bmnum = %d\tchannel = %d\tintt = %d\n",prm.bmnum,
                                prm.channel,prm.intt.sc);
      fprintf(stderr,"frang = %d\ttfreq = %d\n",prm.frang,
                                               prm.tfreq);
      fprintf(stderr,"rsep = %d\tnoise.search = %g\n",prm.rsep,
                                              prm.noise.search);
      fprintf(stderr,"noise.mean = %g\tscan = %d\n",
                      prm.noise.mean,prm.scan);
      fprintf(stderr,"CPID = %d\n",
                      prm.cp);

      
      for (i=0;i<75; i++) {
        if (fit.rng[i].qflg !=0) fprintf(stderr,"d");
        else fprintf(stderr,"-");
      }
      fprintf(stderr,"\n");

      for (i=0;i<75; i++) {
        if (fit.rng[i].gsct !=0) fprintf(stderr,"g");
        else fprintf(stderr,"-");
      }
      fprintf(stderr,"\n");

      for (i=0;i<75; i++) {
        if (fit.rng[i].qflg !=0) fprintf(stderr,"%d:%g\n",i,fit.rng[i].v);
      }
      
   }           
  } while(1);
  fprintf(stderr,"Connection failed.\n");

  return 0;
}
   

 










