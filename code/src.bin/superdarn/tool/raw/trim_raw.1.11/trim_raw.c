/* trim_raw.c
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
#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "version.h"

#include "rawread.h"
#include "rawwrite.h"
#include "rawindex.h"
#include "rawseek.h"

#include "oldrawread.h"
#include "oldrawwrite.h"

#include "errstr.h"
#include "hlpstr.h"


/*
 $Log: trim_raw.c,v $
 Revision 1.13  2007/02/09 14:17:49  code
 Removed redundant fclose.

 Revision 1.12  2007/01/30 18:07:50  code
 Added the ability to override the threshold.

 Revision 1.11  2005/08/10 16:42:58  barnes
 Added option to dump the command line options.

 Revision 1.10  2005/06/30 16:56:40  barnes
 Allowed seconds to be specified in the time.

 Revision 1.9  2004/05/10 21:51:57  barnes
 Removed redundant code.

 Revision 1.8  2003/04/07 19:01:39  barnes
 Changed header version.

 Revision 1.7  2003/02/25 02:07:23  barnes
 Modifications to use the new base libraries.

 Revision 1.6  2001/06/27 21:07:57  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:57  barnes
 Added Author Name

 Revision 1.4  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.3  1999/11/02 13:51:27  barnes
 Added the ability to set the start and end dates.

 Revision 1.2  1999/08/12 13:42:37  barnes
 Added verbose option and fixed end time.

 Revision 1.1  1999/05/18 18:04:09  barnes
 Initial revision

*/

struct RadarParm prm;
struct RawData raw;
struct RawIndex *inx=NULL;

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

  int arg=0;

  int recnum=0;
  float thr=-1;
  int status=0;
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

  int yr,mo,dy,hr,mt;
  double sc;
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;

  struct OldRawFp *rawfp=NULL;
  FILE *fp=NULL;

  char vstring[256];

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'f',&thr);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

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


  if ((old) && (argc-arg<2)) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);


   if (old) {
     rawfp=OldRawOpen(argv[arg],NULL);
     if (rawfp==NULL) {
       fprintf(stderr,"File not found.\n");
       exit(-1);
     }
  } else { 
    if ((argc-arg)>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp==NULL) {
	fprintf(stderr,"Index not found.\n");
        exit(-1);
      }
      inx=RawIndexFload(fp);

      
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
  }

   if (old) {
     if (OldRawRead(rawfp,&prm,&raw)==-1)  {
       fprintf(stderr,"Error reading file\n");
       exit(-1);
     }
   } else {
     if (RawFread(fp,&prm,&raw)==-1)  {
       fprintf(stderr,"Error reading file\n");
       exit(-1);
     }
   }

   atime=TimeYMDHMSToEpoch(prm.time.yr,
	   	           prm.time.mo,
                           prm.time.dy,
                           prm.time.hr,
		           prm.time.mt,
                           prm.time.sc+prm.time.us/1.0e6);

   /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      /* we must skip the start of the files */
      int yr,mo,dy,hr,mt;
      double sc; 
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;

      TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      if (old) status=OldRawSeek(rawfp,yr,mo,dy,hr,mt,sc,NULL);
      else status=RawFseek(fp,yr,mo,dy,hr,mt,sc,NULL,inx); 
    
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }
 
      if (old) {
         if (OldRawRead(rawfp,&prm,&raw)==-1)  {
           fprintf(stderr,"Error reading file\n");
           exit(-1);
         }
      } else {
        if (RawFread(fp,&prm,&raw)==-1)  {
           fprintf(stderr,"Error reading file\n");
           exit(-1);
        }
      }

    } else stime=atime;
   
    if (etime !=-1) {
       if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
       else etime+=edate;
    }

   if (extime !=0) etime=stime+extime;

   if (thr !=-1) raw.thr=thr;

   if (old) {
     fp=fopen(argv[argc-1],"w");
     if (fp==NULL) {
       fprintf(stderr,"Could not open output file.\n");
       exit(-1);
     }
     sprintf(vstring,"%d.%.3d",raw.revision.major,raw.revision.minor);
     if (OldRawHeaderFwrite(fp,"rawwrite",vstring,raw.thr,
			    "trimmed with trim_raw") !=0) {
       fprintf(stderr,"Could not write header.\n");
       exit(-1);
     }
   }


 
   do {
     if (thr !=-1) raw.thr=thr;
     if (old) {
      recnum++;
      OldRawFwrite(fp,"rawwrite",&prm,&raw,recnum,NULL);
     } else RawFwrite(stdout,&prm,&raw);

     atime=TimeYMDHMSToEpoch(prm.time.yr,
		    prm.time.mo,
                    prm.time.dy,
                    prm.time.hr,
		    prm.time.mt,
                    prm.time.sc+prm.time.us/1.0e6);
   TimeEpochToYMDHMS(atime,&yr,&mo,&dy,&hr,&mt,&sc);
   if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

    if ((etime !=-1) && (atime>=etime)) break;
    if (old) status=OldRawRead(rawfp,&prm,&raw);
    else status=RawFread(fp,&prm,&raw);
 
  } while (status !=-1);  
  if (old) OldRawClose(rawfp);
  if (fp !=stdin) fclose(fp);
  return 0;


}






















