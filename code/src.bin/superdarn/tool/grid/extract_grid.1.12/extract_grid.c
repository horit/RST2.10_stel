/* extract_grid.c
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

#include "hlpstr.h"

struct GridData rcd;
struct OptionData opt;

/* 
 $Log: extract_grid.c,v $
 Revision 1.18  2005/08/10 19:49:13  barnes
 Added option to dump command line options.

 Revision 1.17  2004/10/07 22:51:27  barnes
 Moved to DataMap file format.

 Revision 1.16  2004/05/10 21:33:32  barnes
 Removed redundant code.

 Revision 1.15  2003/04/14 18:43:11  barnes
 Changed the "-r" option to be "-mid" to be consistent.

 Revision 1.14  2003/02/25 22:53:15  barnes
 Fixed bug in setting up command line options.

 Revision 1.13  2003/02/25 22:52:39  barnes
 Modification to use the base libraries.

 Revision 1.12  2001/06/27 21:06:57  barnes
 Added license tag

 Revision 1.11  2001/06/11 14:13:46  barnes
 Changed command line option to "-mid".

 Revision 1.10  2001/01/29 18:11:57  barnes
 Added Author Name

 Revision 1.9  2000/10/31 18:03:53  barnes
 Changed command line option to "-full".

 Revision 1.8  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.7  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.6  1999/10/06 19:19:16  barnes
 Added missing header.

 Revision 1.5  1999/10/04 17:48:37  barnes
 Modification to use the new extensible grid_data format.

 Revision 1.4  1999/04/11 18:56:37  barnes
 Modified the command line options.

 Revision 1.3  1999/04/10 17:12:44  barnes
 Added the help message system.

 Revision 1.2  1998/07/30 14:46:52  barnes
 Changed code to use the structure griddata.

 Revision 1.1  1998/06/19 15:23:27  barnes
 Initial revision

*/


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

  unsigned char rflg=0;


  FILE *fp;   
  char *fname=NULL;
  double tme;
  int yr,mo,dy,hr,mt,eyr,emo,edy;
  double sc;
  int i;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 
  OptionAdd(&opt,"mid",'x',&rflg);

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



  rflg=!rflg;

  if (arg<argc) fname=argv[arg];
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldGridFread(fp,&rcd) !=-1) {
      if (rflg==0) {
        tme=(rcd.st_time+rcd.ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
		       yr,mo,dy,hr,mt,(int) sc);
      } else {
        TimeEpochToYMDHMS(rcd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	        yr,mo,dy,hr,mt,(int) sc);
        TimeEpochToYMDHMS(rcd.ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
        if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
        fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
      }
    
      fprintf(stdout,"%d %d ",rcd.vcnum,rcd.stnum);
    
      for (i=0;i<rcd.stnum;i++) fprintf(stdout,"%d ",rcd.sdata[i].npnt);
      for (i=0;i<rcd.stnum;i++) fprintf(stdout,"%d ",rcd.sdata[i].st_id);
      fprintf(stdout,"\n");
    

    }
    if (fp !=stdout) fclose(fp);
  } else {
    while (GridFread(fp,&rcd) !=-1) {
      if (rflg==0) {
        tme=(rcd.st_time+rcd.ed_time)/2.0;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
		       yr,mo,dy,hr,mt,(int) sc);
      } else {
        TimeEpochToYMDHMS(rcd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	        yr,mo,dy,hr,mt,(int) sc);
        TimeEpochToYMDHMS(rcd.ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
        if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
        fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
      }
    
      fprintf(stdout,"%d %d ",rcd.vcnum,rcd.stnum);
    
      for (i=0;i<rcd.stnum;i++) fprintf(stdout,"%d ",rcd.sdata[i].npnt);
      for (i=0;i<rcd.stnum;i++) fprintf(stdout,"%d ",rcd.sdata[i].st_id);
      fprintf(stdout,"\n");
    

    }
    if (fp !=stdout) fclose(fp);
  }

  return 0;
}






















