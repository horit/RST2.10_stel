/* rtpoll.c
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
 $Log: rtpoll.c,v $
 Revision 1.12  2007/03/13 19:00:33  code
 Added the ability to record the process id in a file.

 Revision 1.11  2005/08/08 15:14:39  barnes
 Added option to dump the command line options

 Revision 1.10  2004/04/27 17:53:02  barnes
 Audit - Enforced warning set to "all".

 Revision 1.9  2003/04/08 18:09:52  barnes
 Fixed bug in setting the filename.

 Revision 1.8  2003/04/08 18:06:33  barnes
 Changed the log format.

 Revision 1.7  2003/03/13 00:10:45  barnes
 Modification to use the base libraries.

 Revision 1.6  2001/06/27 21:04:11  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:12:10  barnes
 Added Author Name

 Revision 1.4  2000/08/22 14:40:13  barnes
 Added return to main.

 Revision 1.3  2000/08/16 22:19:33  barnes
 Changed return type of main().

 Revision 1.2  2000/07/24 15:06:44  barnes
 Fixed name conflict.

 Revision 1.1  1999/10/14 19:38:45  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "option.h"
#include "errstr.h"
#include "hlpstr.h"
#include "log_info.h"

#define FNAME "rt.dat"

time_t tval=-1;
struct flock rflock;
char logfname[256];
char log_buf[256];

char pidname[256]={"pid.id"};


struct OptionData opt;

int main(int argc,char *argv[]) {
  FILE *fp;
  unsigned char help=0;  
  unsigned char option=0;
  
  pid_t pid;

  int arg,j;
  int dtime=5;
  int fildes,rc;
  int status;
  struct stat buf;
  struct flock rflock;
  
  char *logstr=NULL;
  char *fnamestr=NULL;
  char *pidstr=NULL;


  char command[256];  
  char fname[64]=FNAME;

  rflock.l_type=F_RDLCK;
  rflock.l_whence=SEEK_SET;
  rflock.l_start=0;
  rflock.l_len=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"d",'i',&dtime);
  OptionAdd(&opt,"if",'t',&pidstr);
  OptionAdd(&opt,"f",'t',&fnamestr);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (argc==arg) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }

  if (logstr !=NULL) strcpy(logfname,logstr);
  if (fnamestr !=NULL) strcpy(fname,fnamestr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);


  command[0]=0;
  for (j=arg;j<argc;j++) {
      strcat(command,argv[j]);
      strcat(command," ");
  }
  
  if (command[0] !=0) {
    sprintf(log_buf,"Command\t:%s",command);
    log_info(logfname,log_buf);
  }
  sprintf(log_buf,"Data file:%s",fname);
  log_info(logfname,log_buf);
  sprintf(log_buf,"Poll Interval:%ds",dtime);
  log_info(logfname,log_buf);

  fp=fopen(pidname,"w");
  pid=getpid();
  sprintf(log_buf,"Process ID recorded in file %s.",pidname);
  log_info(logfname,log_buf);
  sprintf(log_buf,"PID %d.",(int) pid);
  log_info(logfname,log_buf);
  fprintf(fp,"%d\n",(int) pid);
  fclose(fp);

  while (1) {

     sleep(dtime);
     fildes=open(fname,O_RDONLY); 
     if (fildes==-1) continue;
     rflock.l_type=F_RDLCK;
     status=fcntl(fildes,F_SETLKW,&rflock);
     rc=fstat(fildes,&buf);
     rflock.l_type=F_UNLCK;  
     status=fcntl(fildes,F_SETLKW,&rflock);
     close(fildes);
     if (buf.st_mtime !=tval) {

       tval=buf.st_mtime; 
       log_info(logfname,"File changed.");
       if (command[0] !=0) {
         sprintf(log_buf,"Executing command:%s",command);
         log_info(logfname,log_buf);
         system(command);
       }
     }
  }
  return 0;
}

  




















