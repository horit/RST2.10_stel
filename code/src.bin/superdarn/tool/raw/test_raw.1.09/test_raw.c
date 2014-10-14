/* test_raw.c
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
#include "option.h"

#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "version.h"

#include "rawread.h"
#include "oldrawread.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: test_raw.c,v $
 Revision 1.9  2006/07/11 15:58:25  code
 Modification to support the beam azimuth and orgin code.

 Revision 1.8  2005/08/10 16:35:22  barnes
 Added option to dump the command line options.

 Revision 1.7  2004/05/10 21:50:37  barnes
 Removed redundant code.

 Revision 1.6  2003/02/25 01:53:31  barnes
 Modification to use the new base libraries.

 Revision 1.5  2002/02/21 20:16:34  barnes
 Fixed bug in reporting lag-zero powers.

 Revision 1.4  2001/06/27 21:07:44  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:12:02  barnes
 Added Author Name

 Revision 1.2  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.1  2000/03/21 00:57:41  barnes
 Initial revision

*/

struct RadarParm prm;
struct RawData raw;
struct OptionData opt;

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

  int c=1,i;
  int arg=0;
  struct OldRawFp *rawfp=NULL;
  FILE *fp;
  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

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



  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

       
  if (old) {
    for (c=arg;c<argc;c++) {
      rawfp=OldRawOpen(argv[c],NULL);
      if (rawfp==NULL) {
        fprintf(stderr,"Could not open file %s.\n",argv[c]);
        continue;
      }
      while (OldRawRead(rawfp,&prm,&raw) !=-1) {
       fprintf(stdout,
          "%d-%d-%d %d:%d:%d beam=%d  bmazm=%f channel=%d cpid=%d scan=%d\n",
           prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
           prm.time.sc,prm.bmnum,prm.bmazm,prm.channel,prm.cp,prm.scan);
       fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm.origin.code,prm.origin.time,prm.origin.command);

       fprintf(stdout,"pwr 0:\n");
       for (i=0;i<prm.nrang;i++) {
         fprintf(stdout,"%g",raw.pwr0[i]);
         if ((i % 8)==0) fprintf(stdout,"\n");
         else fprintf(stdout,"\t");
        
       }
       fprintf(stdout,"\n");
       
    
      }    
      OldRawClose(rawfp);
    }
  } else {
    if (arg==argc) fp=stdin;
    else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"Could not open file.\n");
        exit(-1);
      }
    }
    while (RawFread(fp,&prm,&raw) !=-1) {
       fprintf(stdout,
          "%d-%d-%d %d:%d:%d beam=%d  bmazm=%f channel=%d cpid=%d scan=%d\n",
           prm.time.yr,prm.time.mo,prm.time.dy,prm.time.hr,prm.time.mt,
           prm.time.sc,prm.bmnum,prm.bmazm,prm.channel,prm.cp,prm.scan);
       fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm.origin.code,prm.origin.time,prm.origin.command);

      fprintf(stdout,"pwr 0:\n");
      for (i=0;i<prm.nrang;i++) {
        fprintf(stdout,"%g",raw.pwr0[i]);
        if ((i % 8)==0) fprintf(stdout,"\n");
        else fprintf(stdout,"\t");
      }
      fprintf(stdout,"\n");   
    }
    if (fp !=stdin) fclose(fp);
  }    
  
  return 0;

}






















