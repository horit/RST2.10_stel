/* combine_grid.c
   ==============
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"

#include "errstr.h"
#include "hlpstr.h"
#include "make_grid.h"

/*
 $Log: combine_grid.c,v $
 Revision 1.14  2005/08/10 19:19:05  barnes
 Added option to dump command line options.

 Revision 1.13  2004/10/08 16:39:32  barnes
 Added DataMap format.

 Revision 1.12  2004/05/10 21:32:56  barnes
 Removed redundant code.

 Revision 1.11  2003/02/25 22:40:21  barnes
 Modification to use the base libraries.

 Revision 1.10  2002/07/18 17:40:53  barnes
 Fixed bug in dealing with stereo files.

 Revision 1.9  2001/06/27 21:06:53  barnes
 Added license tag

 Revision 1.8  2001/01/29 18:11:57  barnes
 Added Author Name

 Revision 1.7  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.6  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.5  1999/10/06 19:20:10  barnes
 Added missing headers.

 Revision 1.4  1999/04/10 17:11:08  barnes
 Added the help message system.

 Revision 1.3  1998/07/30 14:45:17  barnes
 Changed code to use the structure griddata.

 Revision 1.2  1998/06/24 17:50:47  barnes
 Fixed bug caused by change of return from read_master.

 Revision 1.1  1998/06/18 17:47:31  barnes
 Initial revision

 */

char *fname[32];
int fnum=4;

FILE *in_fp[20];

struct GridData in_rcd[20];
int read_flg[20];
int dflg[20];

struct GridData rcd;

struct OptionData opt;


int read_set(int flg) {
  int c=0;
  int i;
  int status=0;
  
  for (i=0;i<fnum;i++) {
    if ((in_fp[i] !=NULL) && (read_flg[i]==1)) {
      if (flg)  status=OldGridFread(in_fp[i],&in_rcd[i]);
      else status=GridFread(in_fp[i],&in_rcd[i]);
      if (status !=-1) {
        read_flg[i]=0;
        dflg[i]=1;
      } else {
        dflg[i]=0;
        fclose(in_fp[i]);
        in_fp[i]=NULL;
      }
    }
    if (in_fp[i] !=NULL) c++;
  }
  return c;
}

int get_length() { 
  int smin=3600*24;
  int scan;
  int i;
  for (i=0;i<fnum;i++) {
    if (dflg[i] !=0) {
       scan=in_rcd[i].ed_time-in_rcd[i].st_time;
       if (scan<smin) smin=scan;
    }
  }
  return smin;
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


  int arg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;
  unsigned char replace=0;
 
  int record=0;
  int period=0;
  int opfp=0;

  int pmin=365*24*3600;
  int fmax=0;

  int i,c=0;
 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"r",'x',&replace);

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
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
  fnum=argc-arg;
 
  for (i=0;i<fnum;i++) {
    dflg[i]=0;
    in_fp[i]=fopen(argv[arg+i],"r");
    if (in_fp[i]==NULL) fprintf(stderr,"File %s Not Found.\n",fname[i]);
    else {
      read_flg[i]=1;
      c++;
    }
  }
  if (c==0) {
    fprintf(stderr,"No files to be processed.\n");
    exit(-1);
  }
  fmax=c;

  while ((opfp=read_set(old)) !=0)  {
    if (record==0) {
      rcd.st_time=1e31;
      c=0;
      for (i=0;i<fnum;i++) {
        if (dflg[i] !=0) {
          if (in_rcd[i].st_time<rcd.st_time) rcd.st_time=in_rcd[i].st_time;
          c++;
        }
      }
    } 
    period=get_length(); 
    if (period<pmin) pmin=period;
    rcd.ed_time=rcd.st_time+period;

    /* load up the results here */

    make_grid(&rcd,in_rcd,fnum,replace,dflg);
    if (vb) {
      int syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc;
      double sec;
      TimeEpochToYMDHMS(rcd.st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
      ssc=sec;
      TimeEpochToYMDHMS(rcd.ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
      esc=sec;

      fprintf(stderr,"%d-%d-%d %d:%d:%d %d:%d:%d pnts=%d files=%d\n",
              syr,smo,sdy,shr,smt,ssc,ehr,emt,esc,rcd.vcnum,rcd.stnum);
    }
    if (old) OldGridFwrite(stdout,&rcd);
    else GridFwrite(stdout,&rcd);
   
    /* get next record */
    rcd.st_time=rcd.ed_time;
    
    /* discard old records */
    for (i=0;i<fnum;i++) if (in_fp[i] !=NULL) 
      if (in_rcd[i].ed_time <= rcd.st_time) read_flg[i]=1;
    record++;
  }

  return 0;
}







