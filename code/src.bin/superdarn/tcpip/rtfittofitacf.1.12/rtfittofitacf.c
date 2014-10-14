/* rtfittofitacf.c
   ===============
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
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "rtypes.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "oldfitcnx.h"
#include "fitpacket.h"
#include "loginfo.h"

#include "errstr.h"
#include "hlpstr.h"
#include "fork.h"

/*
 $Log: rtfittofitacf.c,v $
 Revision 1.5  2006/09/15 19:23:28  code
 Fixed types.

 Revision 1.4  2006/05/18 18:07:16  code
 Added code to record the time of the last record.

 Revision 1.3  2005/08/10 14:33:53  barnes
 Added option to dump the command line options.

 Revision 1.2  2004/06/21 23:00:54  barnes
 Added missing header.

 Revision 1.1  2004/05/10 21:16:54  barnes
 Initial revision

*/

int outpipe=-1;
int resetflg=0;
char pname[256]={"port.id"};
char pidname[256]={"pid.id"};

extern int loaddata(char *);

char logfname[256]={"web.rt.log"};
char timefname[256]={"time.rt"};

struct OptionData opt;

void trapsignal(int signal) {
  resetflg=2;
}

void trapalarm(int signal) {
  resetflg=1;
}

void trappipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

int main(int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;

  char *logstr=NULL;
  char *timestr=NULL;
  char *pnamestr=NULL;
  char *pidstr=NULL;

  int sock;
  int port=0;
  int timeout=2*60;
  int flag,status;
  int remote_port=0;
  char logbuf[256];
  char host[256];

  int sval=255;
  unsigned char stream;

  char *port_fname=NULL;
  unsigned char port_flag=0;

  FILE *fp;

  struct RadarParm prm;
  struct FitData fit;
  unsigned char *buffer=NULL;
  int buf_len;

  sigset_t set;
  struct sigaction act;


#ifdef _POSIX 
  timer_t timer_id=-1;
  struct itimerspec timer;
  struct sigevent event;
#else
  struct itimerval timer;
#endif
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"s",'i',&sval);
  OptionAdd(&opt,"t",'i',&timeout);
  OptionAdd(&opt,"lp",'i',&port);
  OptionAdd(&opt,"rpf",'x',&port_flag);
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"T",'t',&timestr);
  OptionAdd(&opt,"pf",'t',&pnamestr);
  OptionAdd(&opt,"if",'t',&pidstr);

 
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

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (timestr !=NULL) strcpy(timefname,timestr);
  if (pnamestr !=NULL) strcpy(pname,pnamestr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);
 
  strcpy(host,argv[argc-2]);

  if (port_flag==0) remote_port=atoi(argv[argc-1]);
  else port_fname=argv[argc-1];

  stream=sval;

  
  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);
  sigaddset(&set,SIGUSR2);
  sigaddset(&set,SIGPIPE);
  sigaddset(&set,SIGALRM);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapsignal;
  sigaction(SIGUSR1,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trappipe;
  sigaction(SIGPIPE,&act,NULL);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapalarm;
  sigaction(SIGUSR2,&act,NULL);

  signal(SIGCHLD,SIG_IGN); 
 
#ifdef _POSIX
  timer.it_interval.tv_sec=0L;
  timer.it_interval.tv_nsec=0L;
  timer.it_value.tv_sec=timeout;
  timer.it_value.tv_nsec=0;
  event.sigev_signo=SIGUSR2;

  if((timer_id=timer_create(CLOCK_REALTIME,&event))==-1) {
     fprintf(stderr,"Failed to create timer\n");
     exit(-1);
  }
#else
  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapalarm;
  sigaction(SIGALRM,&act,NULL);
  
  timer.it_value.tv_sec=timeout;
  timer.it_value.tv_usec=0;
#endif

  outpipe=forkinet(port);

 

  if (port_flag==0) 
    sprintf(logbuf,"Connecting to Host %s:%d\n",host,remote_port);
  else sprintf(logbuf,"Connecting to Host %s Port File %s\n",host,port_fname);
  loginfo(logfname,logbuf);
 
  do { 
    resetflg=0;

    if (port_flag==1) {
      fp=fopen(port_fname,"r");
      if (fp !=NULL) {
        fscanf(fp,"%d",&remote_port);
        fclose(fp);
      } else remote_port=1024;
    }

    sprintf(logbuf,"Connecting to host:%s %d",host,remote_port);

    loginfo(logfname,logbuf);

    sock=ConnexOpen(host,remote_port); 
    if (sock==-1) {
      loginfo(logfname,"Could not connect to host.");
      sleep(10); 
      continue;
    }

    status=(write(sock,&stream,sizeof(char)) !=1);
    if (status !=0) {
      fprintf(stderr,"Could not set stream to host.\n");
      close(sock);
      sock=-1;
      continue;
    }

    do {
#ifdef _POSIX
    timer_settime(timer_id,0,&timer,NULL);
#else
    setitimer(ITIMER_REAL,&timer,NULL);
#endif
      status=OldFitCnxRead(1,&sock,&prm,&fit,&flag,NULL);
      if ((flag==-1) || (status==-1) || (resetflg !=0)) break;
      if (flag==0) continue;
      buffer=fitpacket(&prm,&fit,&buf_len);
      if (outpipe==-1) {
         loginfo(logfname,"Child process died - Restarting.");
         outpipe=forkinet(port);
      }
      if (outpipe !=-1) status=ConnexWriteIP(outpipe,(void *) buffer,buf_len);
      free(buffer);
    } while(1);
    loginfo(logfname,"Connection to host failed.");
    ConnexClose(sock);
    if (resetflg !=0) {
      if (resetflg==1) loginfo(logfname,"Alarm Triggered.");
      else loginfo(logfname,"User signal received.");
    }
  } while (1);
  sleep(10);

  return 0;
}
   

 





















