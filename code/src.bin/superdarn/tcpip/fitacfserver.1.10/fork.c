/* fork.c
   ======
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
 $Log: fork.c,v $
 Revision 1.1  2004/05/10 21:09:20  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef _QNX4
#include <sys/select.h>
#endif

#include "loginfo.h"
#include "socket.h"
#include "connect.h"

extern char pname[256];
extern char pidname[256];
extern char logfname[256];
  
int fork_inet(int port) {
  FILE *fp;
  char logbuf[256];  
  int status;
  int sock;
  int pipeid[2];
  pid_t cpid; /* process id of the child */

  /* create the pipe */

  if (pipe(pipeid) <0) return -1;

  /* make the pipe unblocking 
   *
   * as we are dealing with a pipe a message is
   * either sent or discarded. So there is no possibility
   * that the task will hang - however if the inet processing
   * fork does not empty the pipe quickly enough, it is possible
   * for a record to be lost.
   */

  if ((status=fcntl(pipeid[1],F_GETFL))==-1) return -1;
  status|=O_NONBLOCK;
  if ((status=fcntl(pipeid[1],F_SETFL,status))==-1) return -1; 
 
  if ((cpid=fork()) !=0) {
    close(pipeid[0]);
    return pipeid[1];
  }

  close(pipeid[1]);

  loginfo(logfname,"Child Server Process Starting");

  sock=create_socket(&port);
  if (sock !=-1) {
    pid_t pid;
    fp=fopen(pname,"w");
    fprintf(fp,"%d\n",port);
    fclose(fp);
    sprintf(logbuf,"Listening on port %d.",port);
    loginfo(logfname,logbuf);
    sprintf(logbuf,"Port number recorded in file %s.",pname);
    loginfo(logfname,logbuf);
    fp=fopen(pidname,"w");
    pid=getppid();
    sprintf(logbuf,"Process ID recorded in file %s.",pidname);
    loginfo(logfname,logbuf);
    sprintf(logbuf,"Parent PID %d.",(int) pid);
    loginfo(logfname,logbuf);
    fprintf(fp,"%d\n",(int) pid);
    pid=getpid();
    sprintf(logbuf,"Child PID %d.",(int) pid);
    loginfo(logfname,logbuf);
    fprintf(fp,"%d\n",(int) pid);
    fclose(fp);

  } else loginfo(logfname,"Failed to create socket");


  if (sock !=-1) process_socket(sock,pipeid[0]);
 
  close(sock);
  close(pipeid[0]);
  loginfo(logfname,"Child Server Process Terminating");
  exit(0);
  
  return -1;
  
}    

