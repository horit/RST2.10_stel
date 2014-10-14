/* rtcfit.c
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "radar.h" 

#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"

#include "fitcfit.h"
#include "cfitwrite.h"

#include "connex.h"
#include "fitcnx.h"

#include "loginfo.h"

#include "errstr.h"
#include "hlpstr.h"



/*
 $Log: rtcfit.c,v $
 Revision 1.2  2005/08/10 16:58:49  barnes
 Added option to dump command line options.

 Revision 1.1  2005/03/03 17:56:31  barnes
 Initial revision

*/

int resetflg;


#define PATH "."
#define PIDFILE "cfit.rt.pid"
#define LOGNAME "cfit.rt.log"

struct CFitdata cfit;



struct OptionData opt;

extern int dotflag;
char logfname[256]={LOGNAME};



void trapsignal(int signal) {
  resetflg=2; 
}

double strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   
 
int main(int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
 
  char *logstr=NULL;
  char *pathstr=NULL;
  char *pidstr=NULL;

  char *envstr;

  int sock;

  int remote_port=0;
  char dname[256];
  char logbuf[256];

  pid_t pid;
  FILE *fp;
  char host[256];
  int flag,status;
  struct flock flock;
  
  struct RadarParm prm;
  struct FitData fit;

  sigset_t set;
  struct sigaction act;

  struct timeval tv; 
  int reset=60;
  int cnt=0; 

  int sval=255;
  unsigned char stream;

  struct RadarNetwork *network=NULL;


  char path[256]={PATH};
  char pidfile[256]={PIDFILE};
  char *port_fname=NULL;
  unsigned char port_flag=0;
  double now=-1;
  double dstart=0;

  int yr,mo,dy,hr,mt;
  double sc;
 
  double minpwr=3.0;

  pid=getpid();

  flock.l_type=F_WRLCK;
  flock.l_whence=SEEK_SET;
  flock.l_start=0;
  flock.l_len=0;
 
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

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"s",'i',&sval);

  OptionAdd(&opt,"mp",'d',&minpwr);

  OptionAdd(&opt,"rpf",'x',&port_flag);

  
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"p",'t',&pathstr);
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
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (pathstr !=NULL) strcpy(path,pathstr);
  if (pidstr !=NULL) strcpy(pidfile,pidstr);
 
  strcpy(host,argv[argc-2]);
  if (port_flag==0) remote_port=atoi(argv[argc-1]);
  else port_fname=argv[argc-1];

  stream=sval;
 

  if (port_flag==0) sprintf(logbuf,"Host:%s %d",host,remote_port);
  else sprintf(logbuf,"Host:%s Port File:%s",host,port_fname);
  loginfo(logfname,logbuf);
 

  sprintf(logbuf,"File path:%s",path);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"pid file:%s",pidfile);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"pid:%d",(int) pid);
  loginfo(logfname,logbuf);
 
  fp=fopen(pidfile,"w");
  fprintf(fp,"%d\n",pid);
  fclose(fp);
  sigemptyset(&set);
  sigaddset(&set,SIGUSR1);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trapsignal;
  sigaction(SIGUSR1,&act,NULL);

  now=0;

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
    if (sock<0) {
      loginfo(logfname,"Could not connect to host - retrying.");
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

    resetflg=0;
    do {
     
       tv.tv_sec=reset; 
       status=FitCnxRead(1,&sock,&prm,&fit,&flag,&tv);
     
       if ((status==-1) || (flag==-1) || (resetflg !=0))  break;
       if (status==0) resetflg=1;
          
       if ((status==1) && (flag==1)) {
         fprintf(stderr,".");
         fflush(stderr);
         dotflag=1;
          
      
         FitToCFit(minpwr,&cfit,&prm,&fit);
         now=cfit.time;
         if (dstart==0) {
           dstart=now-(int) now % (24*3600); /* start of day */
           TimeEpochToYMDHMS(dstart,&yr,&mo,&dy,&hr,&mt,&sc);
           sprintf(dname,"%s/%.4d%.2d%.2d.%s.cfit",
		       path,yr,mo,dy,RadarGetCode(network,cfit.stid,0));
	 }

     
         fp=fopen(dname,"a");
         CFitFwrite(fp,&cfit);
         fclose(fp);

         if ((now-dstart) >= 24*3600) { /* advance to the next day */
           dstart=now-(int) now % (24*3600); /* start of day */
           TimeEpochToYMDHMS(dstart,&yr,&mo,&dy,&hr,&mt,&sc);
           sprintf(dname,"%s/%.4d%.2d%.2d.%s.cfit",
		       path,yr,mo,dy,
                       RadarGetCode(network,cfit.stid,0));
	 }
	 
         cnt++;
	    
       } 
  

     
    } while (1);
 
    if (resetflg==0) loginfo(logfname,"Connection failed.");
    if (resetflg==1) loginfo(logfname,"Connection timed out.");
    if (resetflg==2) loginfo(logfname,"Connection reset by signal.");
    ConnexClose(sock);
  } while(1);

  return 0;
}
   

 










