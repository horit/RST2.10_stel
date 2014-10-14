/* trim_cfit.c
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "cfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: trim_cfit.c,v $
 Revision 1.2  2007/03/15 20:58:57  code
 Added the ability to list the command line options.

 Revision 1.1  2004/06/21 18:19:38  barnes
 Initial revision

*/


struct CFitdata cfit;

int fnum=0;

int cpid=-1;
int channel=-1;

struct OptionData opt;

double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

double strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return (double) hr*3600L+mn*60L;
}   

int main (int argc,char *argv[]) {

  int arg;

  int status=0;

  struct CFitfp *cfitfp;
  
  int yr,mo,dy,hr,mt;
  double sc;
 
  double atime;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double stime=-1;
  double etime=-1;
  double extime=0;
 
  double sdate=-1;
  double edate=-1;

  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char set_channel=0;
  
  char *chnstr=NULL;
  char *cpstr=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  OptionAdd(&opt,"cp",'t',&cpstr);
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"sc",'x',&set_channel);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }


  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if  (argc-arg<1) {
    OptionPrintInfo(stderr,errstr);
    exit(1);
  }

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);

  if (cpstr !=NULL) cpid=atoi(cpstr);
   
  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }
    


  cfitfp=CFitOpen(argv[arg]);
  if (cfitfp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (CFitRead(cfitfp,&cfit)==-1) {
    fprintf(stderr,"Error reading file\n");
    exit(-1);
  }
  atime=cfit.time;

  

  /* skip here */

  if ((stime !=-1) || (sdate !=-1)) { 
    int yr,mo,dy,hr,mt;
    double sc;  
      
    if (stime==-1) stime= ( (int) atime % (24*3600));
    if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
    else stime+=sdate;


    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    status=CFitSeek(cfitfp,yr,mo,dy,hr,mt,sc,NULL);
    if (status ==-1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (CFitRead(cfitfp,&cfit)==-1) {
      fprintf(stderr,"Error reading file\n");
      exit(-1);
    }
  } else stime=atime;
   
  if (etime !=-1) {
    if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
    else etime+=edate;
  }
  if (extime !=0) etime=stime+extime;

  
  do {
    if ((cpid !=-1) && (cfit.cp !=cpid)) continue;
    if ((set_channel==0) &&
        (channel !=-1) && (cfit.channel !=channel)) continue;
 
    if (set_channel==1) cfit.channel=channel;
    
    status=CFitFwrite(stdout,&cfit);
    
    atime=cfit.time;
    TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d %.2d %.2d %.4d\n",
                            yr,mo,dy,hr,mt,
                            (int) sc,cfit.channel,
                            cfit.bmnum,cfit.cp);
 
    if ((etime !=-1) && (atime>=etime)) break;
  } while (CFitRead(cfitfp,&cfit) !=-1);
  CFitClose(cfitfp);
  return 0; 
} 






















