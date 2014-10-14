/* socket.c
   ========
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

/* 
 $Log: socket.c,v $
 Revision 1.7  2007/04/05 16:04:55  code
 Fixed QNX4 types.

 Revision 1.6  2007/03/13 14:48:46  code
 Only log time when a valid packet arrives.

 Revision 1.5  2006/09/15 19:21:28  code
 Fixed types.

 Revision 1.4  2006/07/12 14:45:02  code
 Fixed missing header.

 Revision 1.3  2006/05/18 18:11:42  code
 Added code to record the time of the last record.

 Revision 1.2  2004/08/25 21:38:33  barnes
  Fixes for MacOS 10 (Darwin)

 Revision 1.1  2004/05/10 21:13:27  barnes
 Initial revision

 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>

#ifdef _QNX4
#include <sys/select.h>
#endif


#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtime.h"
#include "socket.h"
#include "connect.h"
#include "msgmem.h"

extern char timefname[256];

struct client client[CLIENT_MAX];
int msgmax=0;
int runloop=1;

char tmpbuf[BUF_SIZE];
char mbuf[BUF_SIZE];


void logtime(char *fname,int nbytes) {
  char txt[256];
  int mask=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  struct flock flock;
  int fid;
  int yr,mo,dy,hr,mt,sc,us,s;
  

  flock.l_type=F_WRLCK;
  flock.l_whence=SEEK_SET;
  flock.l_start=0;
  flock.l_len=0;

  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);

  fid=open(fname,O_WRONLY | O_TRUNC | O_CREAT,mask);

  if (fid !=0) {
    sprintf(txt,"%d %d %d %d %d %d %d %d\n",yr,mo,dy,hr,mt,sc,us,nbytes);
    s=write(fid,txt,strlen(txt)+1);
    close(fid);
  }
}

void trapreset(int signal) {
   runloop=0;
}

void reset() {
  int i;
  for (i=0;i<CLIENT_MAX;i++) {
     client[i].sock=-1;
     client[i].in_sze=0;
     client[i].out_sze=0;
  }
}

int pollsock(int sock,struct timeval *tv,fd_set *fdset) {
  int i;
  FD_SET(sock,fdset);
  for (i=0;i<msgmax;i++) 
    if (client[i].sock !=-1) FD_SET(client[i].sock,fdset);

  if (select(FD_SETSIZE,fdset,0,0,tv) < 0) return -1;
  return 0;
}

int createsocket(int *port) {

#ifdef _QNX4
  int length;
#else
  socklen_t length;
#endif

  int sock;
  struct sockaddr_in server;

  struct hostent *gethostbyname();
 
  /* reset the client array */
  reset();
  
  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) return -1;
  
  /* name and bind socket to an address and port number */

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  if (*port !=0) server.sin_port=htons(*port); 
  else server.sin_port=0;
  
  if (bind(sock,(struct sockaddr *) &server,sizeof(server))) return -1;
   
  /* Find out assigned port number and print it out */

  length=sizeof(server);
  if (getsockname(sock,(struct sockaddr *) &server,&length)) return -1;
    
  *port=ntohs(server.sin_port);
  return sock;
}

int processsocket(int sock,int inpipe) {

  fd_set fdset; /* selected file descriptors */  
  int poll,i;

  struct timeval tv;

   sigset_t set;
  struct sigaction act;

  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapreset;
  sigaction(SIGUSR1,&act,NULL);

  signal(SIGPIPE,SIG_IGN);

  listen(sock,5);
 
  tv.tv_sec=0;
  tv.tv_usec=0;

  poll=0;  
  runloop=1; 
  do {

    FD_ZERO(&fdset);
    FD_SET(inpipe,&fdset);  
    if (poll==0) {
      if (pollsock(sock,NULL,&fdset) !=0) continue;
    } else pollsock(sock,&tv,&fdset);

    /* open any new connections if possible */

    opensock(sock,&fdset);

    poll=0;

    /* check to see if the root server has sent any data */

    if (FD_ISSET(inpipe,&fdset)) {
      int size;
      size=read(inpipe,mbuf,BUF_SIZE);
      if (size==0) break;
 
      /* log the time */

      logtime(timefname,size);

      writeraw(mbuf,size);
    }
    
    /* send the data to the clients */

    if (writesock() !=0) poll=1;
   
    /* read back any data from the clients */

    readsock(&fdset,tmpbuf,BUF_SIZE); 
    
    /* decode the buffers here */

  } while(runloop);
  
  /* close all the clients down */
 
  for (i=0;i<msgmax;i++) {
    if (client[i].sock !=0) close(client[i].sock);
  }
  close(sock);
  return -1;
}



