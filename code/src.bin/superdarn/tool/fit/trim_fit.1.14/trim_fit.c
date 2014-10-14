/* trim_fit.c
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
#include "dmap.h"
#include "rtime.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitwrite.h"
#include "fitindex.h"
#include "fitseek.h"


#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: trim_fit.c,v $
 Revision 1.17  2005/08/10 19:12:06  barnes
 Added option to dump the command line options.

 Revision 1.16  2005/06/30 16:57:36  barnes
 Allowed seconds to be specified in the time.

 Revision 1.15  2004/10/28 14:03:21  barnes
 Added missing header.

 Revision 1.14  2004/05/10 21:51:02  barnes
 Removed redundant code.

 Revision 1.13  2003/04/07 18:48:48  barnes
 Changed fit header.

 Revision 1.12  2003/02/25 16:28:01  barnes
 Modification to use the base libraries.

 Revision 1.11  2002/07/18 16:40:08  barnes
 Added undocumented feature to fix stereo fit files.

 Revision 1.10  2002/02/26 15:37:36  barnes
 Added code to allow filtering based on channel or program ID.

 Revision 1.9  2001/06/27 21:07:49  barnes
 Added license tag

 Revision 1.8  2001/01/29 18:11:57  barnes
 Added Author Name

 Revision 1.7  2000/09/18 18:26:01  barnes
 Changed file header to trim_fit.

 Revision 1.6  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.5  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.4  1999/11/02 13:57:51  barnes
 Added the ability to set the start and end dates.

 Revision 1.3  1999/08/12 13:42:57  barnes
 Added verbose option and fixed end time.

 Revision 1.2  1999/05/18 18:05:41  barnes
 Fixed problem with setting the start time of the file is not specified
 on the command line.

 Revision 1.1  1999/05/18 17:29:11  barnes
 Initial revision

*/


struct RadarParm pbuf[2];
struct FitData fbuf[2];

struct RadarParm *pptr;
struct FitData *fptr;

int fnum=0;

struct RadarParm prm;
struct FitData fit;
struct FitIndex *inx=NULL;

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
  double sc;
  int i,j;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  for (j=i+1;(text[j] !=':') && (text[j] !=0);j++);
  if (text[j]==0) { 
    mn=atoi(text+i+1);
    return (double) hr*3600L+mn*60L;
  }
  text[j]=0;
  mn=atoi(text+i+1);
  sc=atof(text+j+1);
  return (double) hr*3600L+mn*60L+sc;
}   

int main (int argc,char *argv[]) {

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

  int status=0;
  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  FILE *inxfp=NULL;
  
  int irec=1;
  int drec=2;
 
  int dnum=0;
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

  unsigned char inxflg=0;
  unsigned char set_channel=0;
  
  char *chnstr=NULL;
  char *cpstr=NULL;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"i",'x',&inxflg);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  OptionAdd(&opt,"cp",'t',&cpstr);
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"sc",'x',&set_channel);


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


  if ((old) && (argc-arg<2+inxflg)) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
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
    
  if (old) {
    char vstr[256];
    if ((argc-1-inxflg-arg) >1)  fitfp=OldFitOpen(argv[arg],argv[arg+1]);
    else fitfp=OldFitOpen(argv[arg],NULL);

    if (fitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    }
    fp=fopen(argv[argc-1-inxflg],"w");
    if (fp==NULL) {
      fprintf(stderr,"could not create fit file.\n");
      exit(-1);
    }
 
    if (inxflg !=0) {
      inxfp=fopen(argv[argc-1],"w");
      if (inxfp==NULL) fprintf(stderr,"could not create index file.\n");
    }

    if (OldFitRead(fitfp,&pbuf[fnum],&fbuf[fnum])==-1) {
      fprintf(stderr,"Error reading file\n");
      exit(-1);
    }
    sprintf(vstr,"%d.%d",fbuf[fnum].revision.major,fbuf[fnum].revision.minor); 
    OldFitHeaderFwrite(fp,"trim_fit","fitacf",vstr);

    atime=TimeYMDHMSToEpoch(pbuf[fnum].time.yr,
		    pbuf[fnum].time.mo,
                    pbuf[fnum].time.dy,
                    pbuf[fnum].time.hr,
		    pbuf[fnum].time.mt,
                    pbuf[fnum].time.sc+pbuf[fnum].time.us/1.0e6);


    /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      int yr,mo,dy,hr,mt;
      double sc;  
      
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;


      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=OldFitSeek(fitfp,yr,mo,dy,hr,mt,sc,NULL);
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (OldFitRead(fitfp,&pbuf[fnum],&fbuf[fnum])==-1) {
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
      pptr=&pbuf[fnum];
      fptr=&fbuf[fnum];
      fnum=(fnum+1) % 2;

      if ((cpid !=-1) && (pptr->cp !=cpid)) continue;
      if ((set_channel==0) &&
          (channel !=-1) && (pptr->channel !=channel)) continue;
 
      if (set_channel==1) pptr->channel=channel;

      if ((inxfp !=NULL) && (irec==1)) OldFitInxHeaderFwrite(inxfp,pptr);
      dnum=OldFitFwrite(fp,pptr,fptr,NULL);
      if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,pptr);
      atime=TimeYMDHMSToEpoch(pptr->time.yr,
	  	      pptr->time.mo,
                      pptr->time.dy,
                      pptr->time.hr,
		      pptr->time.mt,
                      pptr->time.sc+pptr->time.us/1.0e6);
      TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d %.2d %.2d %.4d\n",
                            yr,mo,dy,hr,mt,
                            (int) sc,pptr->channel,
                            pptr->bmnum,pptr->cp);

      drec+=dnum;
      irec++;
      atime=TimeYMDHMSToEpoch(pbuf[fnum].time.yr,
		    pbuf[fnum].time.mo,
                    pbuf[fnum].time.dy,
                    pbuf[fnum].time.hr,
		    pbuf[fnum].time.mt,
                    pbuf[fnum].time.sc+pbuf[fnum].time.us/1.0e6);
     
      if ((etime !=-1) && (atime>=etime)) break;
 
    } while (OldFitRead(fitfp,&pbuf[fnum],&fbuf[fnum]) !=-1);
    OldFitClose(fitfp);
    if (inxfp !=NULL) {
      fclose(inxfp);
      inxfp=fopen(argv[argc-1],"r+");
      OldFitInxFclose(inxfp,pptr,irec-1);
      fclose(inxfp);
    }
    fclose(fp);
  } else {
    if ((argc-arg)>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp==NULL) {
        fprintf(stderr,"Index not found.\n");
        exit(-1);
      }
      inx=FitIndexFload(fp);
      fclose(fp);
      if (inx==NULL) {
        fprintf(stderr,"Error loading index.\n");
        exit(-1);
      }
    }
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    if (FitFread(fp,&prm,&fit)==-1) {
      fprintf(stderr,"Error reading file\n");
      exit(-1);
    }

    atime=TimeYMDHMSToEpoch(prm.time.yr,
		            prm.time.mo,
                            prm.time.dy,
                            prm.time.hr,
		            prm.time.mt,
                            prm.time.sc+prm.time.us/1.0e6);


    /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      int yr,mo,dy,hr,mt;
      double sc;  
      
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;


      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=FitFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx);
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
      if (FitFread(fp,&prm,&fit)==-1) {
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
      if ((cpid !=-1) && (prm.cp !=cpid)) continue;
      if ((set_channel==0) &&
          (channel !=-1) && (prm.channel !=channel)) continue;
 
      if (set_channel==1) prm.channel=channel;

      status=FitFwrite(stdout,&prm,&fit);
      atime=TimeYMDHMSToEpoch(prm.time.yr,
	  	      prm.time.mo,
                      prm.time.dy,
                      prm.time.hr,
		      prm.time.mt,
                      prm.time.sc+prm.time.us/1.0e6);
      TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d %.2d %.2d %.4d\n",
                            yr,mo,dy,hr,mt,
                            (int) sc,prm.channel,
                            prm.bmnum,prm.cp);
 
      if ((etime !=-1) && (atime>=etime)) break;
 
    } while (FitFread(fp,&prm,&fit) !=-1);
    if (fp !=stdin) fclose(fp);
  }

  return 0; 
} 






















