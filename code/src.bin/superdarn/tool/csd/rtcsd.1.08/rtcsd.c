/* rtcsd.c
   =======
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
#include "csddata.h"

#include "fitcsd.h"
#include "csdwrite.h"

#include "compress.h"

#include "connex.h"
#include "fitcnx.h"

#include "buffer.h"
#include "default.h"
#include "loginfo.h"

#include "errstr.h"
#include "hlpstr.h"



/*
 $Log: rtcsd.c,v $
 Revision 1.4  2005/08/10 17:24:09  barnes
 Added option to dump the command line options.

 Revision 1.3  2004/06/08 15:06:05  barnes
 Changed the filename to yyyymmdd.sss.csd

 Revision 1.2  2004/06/03 20:03:30  barnes
 Adopted the use of strings as radar identifiers.

 Revision 1.1  2004/05/10 21:12:12  barnes
 Initial revision

*/

int resetflg;

#define FNAME "rt.csd"
#define PATH "."
#define PIDFILE "csd.rt.pid"
#define LOGNAME "csd.rt.log"
#define BUFSIZE 4*1024*1024

struct CSDdata csd;

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
 
int testfile(char *fname) {
 int fildes;
  int rc;
  struct stat buf;
  fildes=open(fname,O_RDONLY);
  if (fildes==-1) return 0;
  rc=fstat(fildes,&buf);
  close(fildes);
  if (rc ==-1) return 0;
  if (buf.st_size != 0) {
    return 1;
  }
  return 0;
}

int main(int argc,char *argv[]) {

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;
  unsigned char aflg=0;

  char *logstr=NULL;
  char *pathstr=NULL;
  char *fnamestr=NULL;
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
  int bsze=BUFSIZE;
  int fid=0;
  struct RadarParm prm;
  struct FitData fit;

  sigset_t set;
  struct sigaction act;

 


  struct timeval tv;

 
  int reset=60;
  int cnt=0; 

  int repeat=0;
  
  int sflag=0x08;
  int bmnum=-1;

  int sval=255;
  unsigned char stream;

  struct RadarNetwork *network=NULL;

  double pmax=-1,vmax=-1,wmax=-1;
  double min_pwr=3.0;
  int n;
  int mask=S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  char fname[256]={FNAME};
  char path[256]={PATH};
  char pidfile[256]={PIDFILE};
  char *port_fname=NULL;
  unsigned char port_flag=0;
  double s;
  double now=-1;
  double tlen=3600;
  double dstart=0;

  int yr,mo,dy,hr,mt;
  double sc;
  char prmtxt[8]={""};

  unsigned char daily_flag=0;

  char *tstr=NULL;

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

  OptionAdd(&opt,"t",'d',&min_pwr);
  OptionAdd(&opt,"b",'i',&bmnum);
  OptionAdd(&opt,"a",'x',&aflg);
 
  OptionAdd(&opt,"pwr",'d',&pmax);
  OptionAdd(&opt,"vel",'d',&vmax);
  OptionAdd(&opt,"wdt",'d',&wmax);

  OptionAdd(&opt,"m",'i',&bsze);


  OptionAdd(&opt,"r",'i',&repeat);
  OptionAdd(&opt,"tl",'t',&tstr);

  OptionAdd(&opt,"rpf",'x',&port_flag);
  OptionAdd(&opt,"nd",'x',&daily_flag);
  
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"p",'t',&pathstr);
  OptionAdd(&opt,"f",'t',&fnamestr);
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

  daily_flag=!daily_flag;

  if (min_pwr<0) min_pwr=0;
  if (min_pwr>30) min_pwr=30;
  if (bmnum>15) bmnum=15;

  if (aflg) sflag |=0x07;
  if (pmax !=-1) sflag=sflag | 0x01;
  if (vmax !=-1) sflag=sflag | 0x02;
  if (wmax !=-1) sflag=sflag | 0x04;

  if (pmax==-1) pmax=DEFAULT_PMAX;
  if (vmax==-1) vmax=DEFAULT_VMAX;
  if (wmax==-1) wmax=DEFAULT_WMAX;

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (fnamestr !=NULL) strcpy(fname,fnamestr);
  if (pathstr !=NULL) strcpy(path,pathstr);
  if (pidstr !=NULL) strcpy(pidfile,pidstr);

  if (tstr !=NULL) tlen=strtime(tstr);
 
  strcpy(host,argv[argc-2]);
  if (port_flag==0) remote_port=atoi(argv[argc-1]);
  else port_fname=argv[argc-1];

  stream=sval;
 
  if (makebuffer(bsze)==-1) {
     loginfo(logfname,"Could not reserve memory terminating");
     exit(-1);
  }


  if (port_flag==0) sprintf(logbuf,"Host:%s %d",host,remote_port);
  else sprintf(logbuf,"Host:%s Port File:%s",host,port_fname);
  loginfo(logfname,logbuf);
 
  sprintf(logbuf,"Buffer size:%d kb",bsze/1024);
  loginfo(logfname,logbuf);

  sprintf(logbuf,"Output file name:%s",fname);
  loginfo(logfname,logbuf);
  if (daily_flag) {
    sprintf(logbuf,"Daily file path:%s",path);
    loginfo(logfname,logbuf);
  } else {
    sprintf(logbuf,"Not recording daily data");
    loginfo(logfname,logbuf);
  }

  sprintf(logbuf,"pid file:%s",pidfile);
  loginfo(logfname,logbuf);
  sprintf(logbuf,"pid:%d",(int) pid);
  loginfo(logfname,logbuf);

  if (repeat !=0) sprintf(logbuf,"Writing record every %d beams",repeat);
  else sprintf(logbuf,"Writing record at end of scan");
  loginfo(logfname,logbuf);

  sprintf(logbuf,"File length (hr:mt) %.2d:%.2d",
         (int) tlen/3600,(int) (tlen/60) % 60);
  loginfo(logfname,logbuf);

  if (sflag & 0x01) strcpy(prmtxt,"p");
  if (sflag & 0x02) strcat(prmtxt,"v");
  if (sflag & 0x04) strcat(prmtxt,"w");
  sprintf(logbuf,"Parameters recorded:%s",prmtxt);
  loginfo(logfname,logbuf);

  if (bmnum !=-1) sprintf(logbuf,"Beam recorded:%d",bmnum);
  else sprintf(logbuf,"Recording all beams.");
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

  now=tlen;
  loaddelta(fname);

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
          
         /* process beam here */
       
           
         for (n=0;n<prm.nrang;n++) fit.rng[n].p_0=fit.rng[n].p_l;
      
  
         FitToCSD(sflag,min_pwr,pmax,vmax,wmax,&prm,&fit,&csd);  
         
    	 
         if ((bmnum !=-1) && (csd.bmnum==bmnum) && (csd.scan==0)) csd.scan=1;
         if ((bmnum !=-1) && (csd.bmnum!=bmnum)) continue;
	 
         s=compressdata(&csd);
         if (s !=-1) now=csd.time;

          

         
         /* write data here */
       
         if (((repeat>0) && ((cnt % repeat)==0)) || 
            ((repeat==0) && (prm.scan==1))) {

           loginfo(logfname,"Writing Record");
           fid=open(fname,O_WRONLY | O_TRUNC | O_CREAT,mask);
           if (fid==0) continue;
           fcntl(fid,F_SETLKW,&flock);
           CSDHeaderWrite(fid,&csd);
           writedelta(fid,now-tlen);
           close(fid);           
           if (daily_flag) {

             if (dstart==0) {
               dstart=now-(int) now % (24*3600); /* start of day */
               TimeEpochToYMDHMS(dstart,&yr,&mo,&dy,&hr,&mt,&sc);
               sprintf(dname,"%s/%.4d%.2d%.2d.%s.csd",
		       path,yr,mo,dy,
                       RadarGetCode(network,csd.stid,0));
	     }
                   
             if (testfile(dname)==0) { /* file does not exist so create it */
	       fp=fopen(dname,"w");
               CSDHeaderFwrite(fp,&csd);
               fclose(fp);
             }

             fp=fopen(dname,"a");

             /* write anything in the current day file */

             writetmpdelta(fileno(fp),dstart+24*3600);  
            
             fclose(fp);

             if ((now-dstart) >= 24*3600) { /* advance to the next day */
               dstart=now-(int) now % (24*3600); /* start of day */
               TimeEpochToYMDHMS(dstart,&yr,&mo,&dy,&hr,&mt,&sc);
               sprintf(dname,"%s/%.4d%.2d%.2d.%s.csd",
		       path,yr,mo,dy,
                       RadarGetCode(network,csd.stid,0));
	    }
	  }

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
   

 










