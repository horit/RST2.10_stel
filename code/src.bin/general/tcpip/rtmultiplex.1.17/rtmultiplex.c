/* rtmultiplex.c
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
#include "connex.h"

#include "loginfo.h"
#include "errstr.h"
#include "hlpstr.h"
#include "pipe.h"
#include "fork.h"


/*
 $Log: rtmultiplex.c,v $
 Revision 1.14  2006/05/18 17:56:09  code
 Fixed bug in setting the name of the time file.

 Revision 1.13  2006/05/18 17:34:46  code
 Added code to record the time of the last record.

 Revision 1.12  2005/08/08 15:13:41  barnes
 Added option to dump the command line options

 Revision 1.11  2005/01/13 17:03:55  barnes
 Changed variable name.

 Revision 1.10  2004/08/25 21:17:49  barnes
 Removed redundant declaration.

 Revision 1.9  2004/07/08 18:23:39  barnes
 Fixed bug in setting the time out period.

 Revision 1.8  2004/07/08 18:01:51  barnes
 Added code to reset a connection when data is not received for a period of time.

 Revision 1.7  2004/07/07 21:18:19  barnes
 Removed debugging code.

 Revision 1.6  2004/07/07 21:15:40  barnes
 Tightened up delay times.

 Revision 1.5  2004/06/21 21:59:11  barnes
 Added an improved timeout system.

 Revision 1.4  2004/06/16 21:15:36  barnes
 Added a delay to resetting the chile process to let the connection reset.

 Revision 1.3  2004/06/16 20:59:15  barnes
 Modified handling of broken connections so that there is a delay before
 trying to re-establish the link - This should prevent overloading.

 Revision 1.2  2004/05/04 19:14:06  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/04/27 18:02:29  barnes
 Initial revision

*/


#define MAXSERVER 32

int num=0;
char host[MAXSERVER][256];
int remoteport[MAXSERVER];
int sock[MAXSERVER];
time_t tick[MAXSERVER];

time_t pipewait=0;

unsigned  char *buffer[MAXSERVER];
int size[MAXSERVER];
int flag[MAXSERVER];



int outpipe=-1;
int resetflg=0;

char pname[256]={"port.id"};
char pidname[256]={"pid.id"};
char logfname[256]={"web.rt.log"};
char timefname[256]={"time.rt"};
unsigned char vb=0;


struct OptionData opt;

void trappipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

int main(int argc,char *argv[]) {
  int arg;
  int c;
  unsigned char help=0;
  unsigned char option=0;

  char *logstr=NULL;
  char *timestr=NULL;
  char *pnamestr=NULL;
  char *pidstr=NULL;

  int port=0;
  int timeout=20;
  int status;
  char logbuf[256];

  int num=0;
  char host[MAXSERVER][256];
  int remoteport[MAXSERVER];
  int sock[MAXSERVER];

  unsigned char sfilter;
  unsigned char st;


  sigset_t set;
  struct sigaction act;

  struct timeval tmout;
  int cnt=0;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'i',&timeout);
  OptionAdd(&opt,"lp",'i',&port);
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"T",'t',&timestr);

  OptionAdd(&opt,"pf",'t',&pnamestr);
  OptionAdd(&opt,"if",'t',&pidstr);
  OptionAdd(&opt,"f",'x',&sfilter); /* allow stream filtering */
 
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
 
  for (c=arg;c<argc;c+=2) {
    strcpy(host[num],argv[c]);
    remoteport[num]=atoi(argv[c+1]);
    sock[num]=-1;
    num++;
  }

  sigemptyset(&set);
  sigaddset(&set,SIGPIPE);
 
  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trappipe;
  sigaction(SIGPIPE,&act,NULL);

  signal(SIGCHLD,SIG_IGN); 
 
  outpipe=forkinet(port);

  do {
    for (c=0;c<num;c++) {
      if (time(NULL) > tick[c]) {
	 if (sock[c] !=-1) {
	   sprintf(logbuf,
                   "Connection to Host %s:%d timed out - connection reset",
                   host[c],remoteport[c]);
           loginfo(logfname,logbuf);
           close(sock[c]);
           sock[c]=-1;
	 }
         sprintf(logbuf,"Connecting to Host %s:%d.",host[c],remoteport[c]);
         loginfo(logfname,logbuf);
         sock[c]=ConnexOpen(host[c],remoteport[c]);
         if (sock[c]==-1) {
           sprintf(logbuf,"Could not connect to Host %s:%d.",
		 host[c],remoteport[c]);
           loginfo(logfname,logbuf);
	 }
         tick[c]=time(NULL)+timeout;
      }
    }

  
    cnt=0;
    for (c=0;c<num;c++) if (sock[c] !=-1) cnt++;
    if (cnt==0) {
      sleep(1);
      continue;
    }

    tmout.tv_sec=timeout;
    tmout.tv_usec=0;

    if (vb) {

      sprintf(logbuf,"Polling for incoming data. (%d open connections)",
	      cnt);
      loginfo(logfname,logbuf);
    }
    status=ConnexRead(num,sock,buffer,size,flag,&tmout);
    if (status==0) {
      loginfo(logfname,"select() timed out - restarting connections.");
      for (c=0;c<num;c++) {
        close(sock[c]);
	sock[c]=-1;
        tick[c]=time(NULL)+timeout;
      }
      continue;
    }


    if (status==-1) {
      loginfo(logfname,"select() failed - restarting connections.");
      for (c=0;c<num;c++) {
        close(sock[c]);
	sock[c]=-1;
        tick[c]=time(NULL)+timeout;
      }
      continue;
    }

    for (c=0;c<num;c++) {
      if (flag[c]==0) continue;
      if (flag[c]==-1) {
        if (vb) {
          sprintf(logbuf,"Connection to Host %s:%d failed.",
	  host[c],remoteport[c]);
          loginfo(logfname,logbuf);
	}
        close(sock[c]);
	sock[c]=-1;
        tick[c]=time(NULL)+timeout;
        continue;
      }  
      if ((outpipe==-1) && (time(NULL) > pipewait)) {
        loginfo(logfname,"Child process died - Restarting.");
        outpipe=forkinet(port);
        if (outpipe==-1) {
          pipewait=time(NULL)+timeout;
          continue;
	}
      }
      tick[c]=time(NULL)+timeout;
      if (sfilter) st=c+1;
      else st=255;
      if (vb) {
         sprintf(logbuf,"Received %d bytes from Host %s:%d.",
	         size[c],host[c],remoteport[c]);
         loginfo(logfname,logbuf);
      }
      if (outpipe==-1) continue;
      
      status=pipewrite(outpipe,st,buffer[c],size[c]);
      if (status==-1) {
            loginfo(logfname,"Pipe broken");
	    close(outpipe);
            pipewait=time(NULL)+timeout;
            outpipe=-1;
      }
    }
  } while (1);
  return 0;
}


   

 






















