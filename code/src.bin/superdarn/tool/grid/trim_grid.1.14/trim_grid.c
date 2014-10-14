/* trim_grid.c
   ===========
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
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "gridindex.h"
#include "gridseek.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "radar.h"
#include "errstr.h"
#include "hlpstr.h"


/*
 $Log: trim_grid.c,v $
 Revision 1.23  2005/08/10 20:21:17  barnes
 Added option to dump command line options.

 Revision 1.22  2004/10/29 15:30:37  barnes
 Fixed verbose mode.

 Revision 1.21  2004/06/04 01:43:52  barnes
 Adopted text strings as radar identifiers.

 Revision 1.20  2004/05/10 21:51:32  barnes
 Removed redundant code.

 Revision 1.19  2003/03/13 14:23:44  barnes
 Removed debugging code.

 Revision 1.18  2003/02/25 23:17:20  barnes
 Modification to use the base libraries.

 Revision 1.17  2002/02/07 16:30:01  barnes
 Modified code to deal with filtering out a specific channel.

 Revision 1.16  2001/06/27 21:07:53  barnes
 Added license tag

 Revision 1.15  2001/01/29 18:12:02  barnes
 Added Author Name

 Revision 1.14  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.13  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.12  2000/08/02 14:31:14  barnes
 Added fast searching and indexing.

 Revision 1.11  2000/06/15 18:19:46  barnes
 Moved the integration processing to the main grid library.

 Revision 1.10  2000/06/09 15:09:45  barnes
 Fixed bug that meant the last record was not written when integrating.

 Revision 1.9  2000/06/02 23:24:24  barnes
 Modification to add the -i option that allows multiple records to be merged
 together.

 Revision 1.8  1999/11/02 15:11:04  barnes
 Fixed bug in command line options.

 Revision 1.7  1999/11/02 13:23:13  barnes
 Added the ability to set the start and end date.

 Revision 1.6  1999/10/06 19:19:39  barnes
 Added missing headers.

 Revision 1.5  1999/10/04 18:54:45  barnes
 Modifications to use the new grid file format.

 Revision 1.4  1999/04/10 17:18:45  barnes
 Added help message system.

 Revision 1.3  1998/11/11 00:51:41  barnes
 Fixed bug that left the st_cnt variable undefined.

 Revision 1.2  1998/07/30 14:48:06  barnes
 Changed code to use the structure griddata.

 Revision 1.1  1998/06/18 13:38:45  barnes
 Initial revision

 */


char *fname=NULL;
char *iname=NULL;
FILE *fp;

struct GridData ircd;
struct GridData orcd;
struct GridData arcd;

struct OptionData opt;

struct RadarNetwork *network;  

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

  unsigned char vb=0;
  struct RfileIndex *oinx=NULL;
  struct GridIndex *inx=NULL;


  char *envstr;
  int st_id[32];
  int s;
  double err[3]={100,100,100};
  int st_cnt=0;
  int intg=0;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double sdte=-1;
  double edte=-1;

  double ssec=-1; /* specified start time */
  double esec=-1; /* end time */
  double exsec=0;

  int yr,mo,dy,hr,mt;
  double sc;

  char *ststr=NULL;
  char *chnstr=NULL;
  int channel=-1;

  int rnum=0;
  int i,j;

  int rcnt=0;

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
  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  OptionAdd(&opt,"cn",'t',&chnstr);
  OptionAdd(&opt,"exc",'t',&ststr);


  OptionAdd(&opt,"i",'i',&intg);
 
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



  if (exstr !=NULL) exsec=strtime(exstr);
  if (stmestr !=NULL) ssec=strtime(stmestr);
  if (etmestr !=NULL) esec=strtime(etmestr);
  if (sdtestr !=NULL) sdte=strdate(sdtestr);
  if (edtestr !=NULL) edte=strdate(edtestr);

  if (chnstr !=NULL) {
    if (tolower(chnstr[0])=='a') channel=1;
    if (tolower(chnstr[0])=='b') channel=2;
  }
    
  if (ststr !=NULL) {
    char *tmp;
    tmp=strtok(ststr,",");
    do {
       st_id[st_cnt]=RadarGetID(network,tmp);
       if (st_id[st_cnt] !=-1) st_cnt++;
       else {
	 st_id[st_cnt]=atoi(tmp);
         st_cnt++;
       }
     } while ((tmp=strtok(NULL,",")) !=NULL);
  }



  if (argc-arg>1) {
    fname=argv[arg];
    iname=argv[arg+1];
  } else if (arg !=argc) fname=argv[arg];

  if (old) {
   if (iname !=NULL) {
       fp=fopen(iname,"r");
       if (fp !=NULL) { 
         oinx=RfileLoadIndex(fp);
         fclose(fp);
       }
    }
  } else {
    if (argc-arg>1) {
      fp=fopen(argv[arg+1],"r");
      if (fp !=NULL) { 
         inx=GridIndexFload(fp);
         fclose(fp);
      }
    } 
  }

  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (old) s=OldGridFread(fp,&ircd);
  else s=GridFread(fp,&ircd);
 
  if (s !=-1) {
      if (ssec==-1) ssec=((int) ircd.st_time % (24*3600));
      if (sdte==-1) ssec+=ircd.st_time - ((int) ircd.st_time % (24*3600));
      else ssec+=sdte;
      if (esec !=-1) {
         if (edte==-1) esec+=ircd.st_time - ((int) ircd.st_time % (24*3600));
         else esec+=edte;
      }
      if (exsec !=0) esec=ssec+exsec;
  }

  if (fp !=stdin) { /* do a search as we are reading from a file */
   
     TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
     if (old) {
       if (ircd.st_time<ssec) 
         OldGridFseek(fp,yr,mo,dy,hr,mt,(int) sc,oinx,NULL);
     } else {
       if (ircd.st_time<ssec) GridFseek(fp,yr,mo,dy,hr,mt,(int) sc,NULL,inx);
     }
  }

  while (s !=-1)  {
    rnum++;
    if (ircd.st_time<ssec) {
      if (old) s=OldGridFread(fp,&ircd);
      else s=GridFread(fp,&ircd);
      continue;
    }
    if ((esec !=-1) && (ircd.ed_time>esec)) break;   
         
    for (i=0;i<ircd.stnum;i++) {
      for (j=0;(j<st_cnt) &&  (ircd.sdata[i].st_id !=st_id[j]);j++);
      if (j<st_cnt) {
        if (channel==-1) ircd.sdata[i].st_id=-1;
        else if (ircd.sdata[i].chn==channel) ircd.sdata[i].st_id=-1;
      }
    }  

    for (i=0;i<ircd.vcnum;i++) {
      for (j=0;(j<st_cnt) &&  (ircd.data[i].st_id !=st_id[j]);j++);
      if (j<st_cnt) {
        if (channel==-1) ircd.data[i].st_id=-1;
        else if (ircd.data[i].chn==channel) ircd.data[i].st_id=-1;
      }
    }
    
    if (intg !=0) { /* must do integral calculation */
   
      if ((rcnt>0) && (ircd.st_time>=orcd.ed_time)) {
        /* write the record out */ 
        GridAdd(&orcd,NULL,rcnt);
        GridIntegrate(&arcd,&orcd,err);
        if (vb) {
           TimeEpochToYMDHMS(arcd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
		   "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",yr,mo,dy,hr,mt,(int) sc);
           TimeEpochToYMDHMS(arcd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
             "%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d (%d stations)\n",yr,mo,dy,
		   hr,mt,(int) sc,arcd.vcnum,arcd.stnum);

	}
        if (old) OldGridFwrite(stdout,&arcd);
        else GridFwrite(stdout,&arcd);
        rcnt=0;
      }

      if (rcnt==0) {
        GridCopy(&orcd,&ircd);
        orcd.ed_time=orcd.st_time+intg;
        rcnt++;
      } else {
        GridAdd(&orcd,&ircd,rcnt);
        rcnt++;
        
      }
    } else {
       if (vb) {
         TimeEpochToYMDHMS(ircd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,
	         "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",yr,mo,dy,hr,mt,(int) sc);
         TimeEpochToYMDHMS(ircd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,
            "%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d (%d stations)\n",yr,mo,dy,
	     hr,mt,(int) sc,ircd.vcnum,ircd.stnum);

         
       }

       if (old) OldGridFwrite(stdout,&ircd);
       else GridFwrite(stdout,&ircd);
    }
    if (old) s=OldGridFread(fp,&ircd);
    else s=GridFread(fp,&ircd);
  }
  if (intg !=0) {
    if (rcnt>0) {
    
      /* write the record out */
   
      GridAdd(&orcd,NULL,rcnt);
      
      GridIntegrate(&arcd,&orcd,err);
    
      if (vb) {
           TimeEpochToYMDHMS(arcd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
		   "%.4d-%.2d-%.2d %.2d:%.2d:%.2d ",yr,mo,dy,hr,mt,(int) sc);
           TimeEpochToYMDHMS(arcd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stderr,
             "%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d (%d stations)\n",yr,mo,dy,
		   hr,mt,(int) sc,arcd.vcnum,arcd.stnum);
      }


      if (old) OldGridFwrite(stdout,&arcd);
      else GridFwrite(stdout,&arcd);

      rcnt=0;
    }
  }
 
  if (fp !=stdin) fclose(fp); 
  return 0; 
}







