/* trim_map.c
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
#include <math.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"



#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: trim_map.c,v $
 Revision 1.11  2005/08/11 14:00:23  barnes
 Added option dump the command line options.

 Revision 1.10  2004/10/13 00:42:18  barnes
 Moved to DataMap format.

 Revision 1.9  2004/06/21 22:39:25  barnes
 Added missing header.

 Revision 1.8  2004/05/10 22:08:19  barnes
 Removed redundant code.

 Revision 1.7  2003/02/26 03:01:48  barnes
 Modification to deal with base libraries.

 Revision 1.6  2001/06/27 21:10:17  barnes
 Added license tag

 Revision 1.5  2000/08/17 15:00:24  barnes
 Changed return type of main().

 Revision 1.4  2000/08/02 16:21:28  barnes
 Added indexing and fast seek.

 Revision 1.3  1999/11/02 15:13:01  barnes
 Added the ability to take the file from stdin.

 Revision 1.2  1999/11/02 14:20:41  barnes
 Added the ability to set the start and end dates.

 Revision 1.1  1999/10/07 14:42:16  barnes
 Initial revision

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

struct GridData grd;
struct CnvMapData  map;

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
  struct CnvMapIndex *inx=NULL;


  int status;

  int yr,mo,dy,hr,mt;
  double sc;

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

  int major=-1;
  int minor=-1;
  char *source=NULL;

  int rnum=0;
 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);
  
  OptionAdd(&opt,"s",'t',&source);
  OptionAdd(&opt,"major",'i',&major);
  OptionAdd(&opt,"minor",'i',&minor);

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
         inx=CnvMapIndexFload(fp);
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

  if (old) status=OldCnvMapFread(fp,&map,&grd);
  else status=CnvMapFread(fp,&map,&grd);

  if (status !=-1) {
      if (ssec==-1) ssec=((int) map.st_time % (24*3600));
      if (sdte==-1) ssec+=map.st_time - ((int) map.st_time % (24*3600));
      else ssec+=sdte;
      if (esec !=-1) {
         if (edte==-1) esec+=map.st_time - ((int) map.st_time % (24*3600));
         else esec+=edte;
      }
      if (exsec !=0) esec=ssec+exsec;
  }


  if (fp !=stdin) { /* do a search as we are reading from a file */
     int yr,mo,dy,hr,mt;
     double sc;
     TimeEpochToYMDHMS(ssec,&yr,&mo,&dy,&hr,&mt,&sc);
     if (old) {
       if (map.st_time<ssec) OldCnvMapFseek(fp,yr,mo,dy,hr,mt,(int) sc,
					    oinx,NULL);
     } else {
       if (map.st_time<ssec) CnvMapFseek(fp,yr,mo,dy,hr,mt,(int) sc,NULL,inx);
     }
  }


  while (status !=-1) {  

   
    rnum++;
    if (map.st_time<ssec) {
      if (old) status=OldCnvMapFread(fp,&map,&grd);
      else status=CnvMapFread(fp,&map,&grd);
      continue;
    }
    if ((esec !=-1) && (map.ed_time>esec)) break;   

    if (vb==1) {
       TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
       fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);
    }  
            
    if (source !=NULL) strcpy(map.source,source);
    if (major !=-1) map.major_rev=major;
    if (minor !=-1) map.minor_rev=minor;
   
    if (old) OldCnvMapFwrite(stdout,&map,&grd);
    else CnvMapFwrite(stdout,&map,&grd);
    if (old) status=OldCnvMapFread(fp,&map,&grd);
    else status=CnvMapFread(fp,&map,&grd);
    
  }

  if (fp !=stdin) fclose(fp); 
  return 0; 
}







