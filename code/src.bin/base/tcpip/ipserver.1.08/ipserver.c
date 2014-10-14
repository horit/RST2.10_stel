/* ipserver.c
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

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "option.h"
#include "connex.h"
#include "log_info.h"

#include "errstr.h"
#include "hlpstr.h"
#include "fork.h"

/*
 $Log: ipserver.c,v $
 Revision 1.4  2005/08/05 20:33:42  barnes
 Added option to dump command line options.

 Revision 1.3  2004/06/21 21:41:01  barnes
 Added missing header.

 Revision 1.2  2004/04/27 14:49:56  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/06 20:23:22  barnes
 Initial revision

*/


int outpipe=-1;
int resetflg=1;

char *dfpname={"port.id"};
char *dfpidname={"pid.id"};
char *dflogfname={"log.rt"};


char *pname=NULL;
char *pidname=NULL;
extern char *logfname;

struct OptionData opt;

void trap_pipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

void trap_signal(int signal) {
  resetflg=1;
}


int main(int argc,char *argv[]) {
  
  int port=0,arg=0;
  int status;
  

  char *date;
  

  unsigned char help=0; 
  unsigned char option=0; 
 

  sigset_t set;
  struct sigaction act;
  
  time_t ltime;
  struct tm *time_of_day;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"L",'t',&logfname);
  OptionAdd(&opt,"pf",'t',&pname);
  OptionAdd(&opt,"if",'t',&pidname);
  OptionAdd(&opt,"lp",'i',&port);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (logfname==NULL) logfname=dflogfname;
  if (pname==NULL) pname=dfpname;
  if (pidname==NULL) pidname=dfpidname;

  signal(SIGCHLD,SIG_IGN); 

  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);
  sigaddset(&set,SIGPIPE);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_signal;
  sigaction(SIGUSR1,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_pipe;
  sigaction(SIGPIPE,&act,NULL);

  outpipe=fork_inet(port);
  
  status=0;
 
  do {
 
    /* get date */
    fprintf(stderr,".");
    fflush(stderr);

    time(&ltime);  
    time_of_day=localtime(&ltime);

    date=asctime(time_of_day);  

    if (outpipe==-1) {
      log_info("Child process died - Restarting.");
      outpipe=fork_inet(port);
    }
    if (outpipe !=-1) status=ConnexWriteIP(outpipe,(void *) date,
                                          strlen(date)+1);
    
    sleep(5);
    status=0;

  } while(status==0);
  return 0;

}
   

 






















