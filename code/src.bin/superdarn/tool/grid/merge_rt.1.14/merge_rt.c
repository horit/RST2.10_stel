/* merge_rt.c
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include "rtypes.h"
#include "option.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "errstr.h"
#include "hlpstr.h"
#include "log_info.h"

/*
 $Log: merge_rt.c,v $
 Revision 1.24  2007/03/13 19:13:05  code
 Added the ability to log the process ID.

 Revision 1.23  2005/08/10 20:52:40  barnes
 Added option to dump command line options.

 Revision 1.22  2005/03/23 22:14:52  barnes
 Fixed bug in setting channel correctly.

 Revision 1.21  2005/03/23 22:10:43  barnes
 Fixed unitialized variable

 Revision 1.20  2005/03/23 22:09:52  barnes
 Fixed problem with setting the channel value.

 Revision 1.19  2004/10/08 19:29:04  barnes
 Added DataMap format.

 Revision 1.18  2004/06/21 22:57:18  barnes
 Added missing header.

 Revision 1.17  2004/05/10 21:11:11  barnes
 Removed redundant code.

 Revision 1.16  2003/04/08 17:59:25  barnes
 Changed the log format.

 Revision 1.15  2003/02/26 00:12:07  barnes
 Modification to use the base libraries.

 Revision 1.14  2001/06/27 21:06:33  barnes
 Added license tag

 Revision 1.13  2001/01/29 18:12:03  barnes
 Added Author Name

 Revision 1.12  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.11  2000/08/16 22:33:11  barnes
 Changed return type of main().

 Revision 1.10  1999/10/14 19:05:34  barnes
 Fixed bug in merging extended files.

 Revision 1.9  1999/10/13 22:13:00  barnes
 Modifications for the extended file format.

 Revision 1.8  1999/10/07 20:44:43  barnes
 Moved to new file format.

 Revision 1.7  1999/04/11 19:35:17  barnes
 Removed some debug code.

 Revision 1.6  1999/04/10 19:50:47  barnes
 Added help message system.

 Revision 1.5  1999/03/25 15:20:28  barnes
 Fixed bug in determining the correct start time for a record.

 Revision 1.4  1999/03/25 14:57:27  barnes
 Modification so that if there is no clear determination of what start time
 to use then use the most recent one.

 Revision 1.3  1999/03/18 23:32:54  barnes
 Locking of stable version
 ..

 Revision 1.2  1998/12/21 17:08:53  barnes
 Fixed log message.

 Revision 1.1  1998/12/21 15:26:07  barnes
 Initial revision

 */


int fnum=0;
char *inname[20];
char *outname;

char pidname[256]={"pid.id"};

int dflg[20];
struct GridData in_rcd[20]; 
struct GridData rcd;

int tin=0;
double st_tme[20];
double ed_tme[20];
int tcnt[20];
char logname[256];
char logbuf[256];

struct OptionData opt;

int make_grid(struct GridData *out,
	      struct GridData *in,int fnum,int *dflg) {

  int i,j,k=0,l=0;
  int tpnt=0;
  struct GridGVec *data[20];

  out->stnum=0;
  out->vcnum=0;
  out->xtd=0;
 
  l=0;
  for (i=0;i<fnum;i++) 
    if ( (dflg[i] !=0) && 
         (in[i].st_time<out->ed_time) && 
	 (in[i].ed_time>out->st_time)) { 
   
     
     if (in[i].xtd) out->xtd=1;
     for (j=0;j<in[i].stnum;j++) {  
  
       if (out->sdata !=NULL) out->sdata=realloc(out->sdata,
                                      sizeof(struct GridSVec)*(l+1));
       else out->sdata=malloc(sizeof(struct GridSVec));
     
       out->sdata[l].st_id=in[i].sdata[j].st_id;
       out->sdata[l].chn=in[i].sdata[j].chn;
       out->sdata[l].npnt=in[i].sdata[j].npnt;
       out->sdata[l].freq0=in[i].sdata[j].freq0;
       out->sdata[l].major_revision=in[i].sdata[j].major_revision;
       out->sdata[l].minor_revision=in[i].sdata[j].minor_revision;
       out->sdata[l].prog_id=in[i].sdata[j].prog_id;
       out->sdata[l].noise.mean=in[i].sdata[j].noise.mean;
       out->sdata[l].noise.sd=in[i].sdata[j].noise.sd;
       out->sdata[l].gsct=in[i].sdata[j].gsct;
       out->sdata[l].vel.min=in[i].sdata[j].vel.min;
       out->sdata[l].vel.max=in[i].sdata[j].vel.max;
       out->sdata[l].pwr.min=in[i].sdata[j].pwr.min;
       out->sdata[l].pwr.max=in[i].sdata[j].pwr.max;
       out->sdata[l].wdt.min=in[i].sdata[j].wdt.min;
       out->sdata[l].wdt.max=in[i].sdata[j].wdt.max;
       out->sdata[l].verr.min=in[i].sdata[j].verr.min;
       out->sdata[l].verr.max=in[i].sdata[j].verr.max;

       data[l]=in[i].data;     
       l++;
     }
    }
    tpnt=0;
    for (i=0;i<l;i++) tpnt+=out->sdata[i].npnt;
    out->stnum=l;
    out->vcnum=tpnt;
    if (out->data !=NULL) 
      out->data=realloc(out->data,sizeof(struct GridGVec)*tpnt);    
    else out->data=malloc(sizeof(struct GridGVec)*tpnt);

    k=0;
    for (i=0;i<l;i++) {
      memcpy(&out->data[k],data[i],sizeof(struct GridGVec)*out->sdata[i].npnt);
      k+=out->sdata[i].npnt;
    } 

  return l;
}
    
    




    

int main(int argc,char *argv[]) {

/* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */



  int old=0;
  int new=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  pid_t pid; 

  char *logstr=NULL;
  char *pidstr=NULL;

  int i,t;
  time_t stme;
  struct tm *gtme;
  time_t utc;
  int tmax,tval;
  double st_max;
  int offset=5;
  int repeat=30;


  FILE *fp;
  int fd; 
  int status=0;
  int mxflg=0;
  
  struct flock rflock;
  struct flock wflock;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"i",'i',&repeat);
  OptionAdd(&opt,"o",'i',&offset);
  OptionAdd(&opt,"r",'x',&mxflg);
  OptionAdd(&opt,"L",'t',&logstr);
  OptionAdd(&opt,"if",'t',&pidstr);
 

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  old=!new;


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

  mxflg=!mxflg;
  if (logstr !=NULL) strcpy(logname,logstr);
  if (pidstr !=NULL) strcpy(pidname,pidstr);


  for (i=arg;i<argc-1;i++) {
    inname[fnum]=argv[i];
    fnum++;
  }
  outname=argv[argc-1];


  rflock.l_type=F_RDLCK;
  rflock.l_whence=SEEK_SET;
  rflock.l_start=0;
  rflock.l_len=0;

  wflock.l_type=F_WRLCK;
  wflock.l_whence=SEEK_SET;
  wflock.l_start=0;
  wflock.l_len=0;

  if (fnum==0) {
    fprintf(stderr,"No files specified.\n");
    exit(-1);
  }

  sprintf(logbuf,"Merging files:");
  log_info(logname,logbuf);
  for (i=0;i<fnum;i++) {
    sprintf(logbuf,"%s",inname[i]);
    log_info(logname,logbuf);
  }
  sprintf(logbuf,"Output file:%s",outname);
  log_info(logname,logbuf);
  sprintf(logbuf,"Interval length:%d",repeat);
  log_info(logname,logbuf);
  sprintf(logbuf,"Interval offset:%d",offset); 
  log_info(logname,logbuf);

  fp=fopen(pidname,"w");
  pid=getpid();
  sprintf(logbuf,"Process ID recorded in file %s.",pidname);
  log_info(logname,logbuf);
  sprintf(logbuf,"PID %d.",(int) pid);
  log_info(logname,logbuf);
  fprintf(fp,"%d\n",(int) pid);
  fclose(fp);

  /* normalize on a minute boundary */

  if (repeat<1) repeat=1;
  if (offset>repeat) offset=0;
  if (offset<0) offset=0;

  stme=time(NULL);
  stme=stme-(stme % repeat)+offset;
 
  while(1) {
    do {
      utc=time(NULL);
      gtme=gmtime(&utc);
      utc=utc-stme;
      sleep(1);
    } while (utc<repeat);    
    stme=time(NULL);
    tin=0;
    for (i=0;i<fnum;i++) {
      fp=fopen(inname[i],"r");
      if (fp==NULL) continue;
      fd=fileno(fp);
      status=fcntl(fd,F_SETLKW,&rflock);
      if (old) {
        if (status==0) status=OldGridFread(fp,&in_rcd[i]);
      } else {
        if (status==0) status=GridFread(fp,&in_rcd[i]);
      }
      if (status !=-1) {
        dflg[i]=1;
        for (t=0;(t<tin) && (st_tme[t] !=in_rcd[i].st_time);t++);
        tcnt[t]++;
        if (t==tin) { 
          st_tme[t]=in_rcd[i].st_time;
          ed_tme[t]=in_rcd[i].ed_time;
          tin++;
        }
      } else dflg[i]=0;
      fclose(fp);
    }
    tmax=0;
    tval=0;

   
    if (mxflg==1) {
      for (t=0;t<tin;t++) if (tcnt[t]>tmax) {
        tmax=tcnt[t];
        tval=t;
      }
    }
   
    if ((mxflg==0) || (tmax==1)) {
      /* use the most recent record */
      st_max=-1;
      for (t=0;t<tin;t++) if (st_tme[t]>st_max) {
        st_max=st_tme[t];
        tval=t;
      }
    }
      
   
      
    for (t=0;t<tin;t++) tcnt[t]=0;

    rcd.st_time=st_tme[tval];
    rcd.ed_time=ed_tme[tval];
    
    make_grid(&rcd,in_rcd,fnum,dflg);
    sprintf(logbuf,"Number of files %d, Number of points %d.",
	    rcd.stnum,rcd.vcnum);
    log_info(logname,logbuf);
    fp=fopen(outname,"w");
    if (fp !=NULL) {
      fd=fileno(fp);
      status=fcntl(fd,F_SETLKW,&wflock);
      if (old) {
        if (status==0) OldGridFwrite(fp,&rcd);
      } else {
        if (status==0) GridFwrite(fp,&rcd);
      }
      fclose(fp);
    }
  }
  return 0;
}









