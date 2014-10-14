/* etime.c
   =======
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
#include "rtime.h"
#include "option.h"
#include "hlpstr.h"

/*
 $Log: etime.c,v $
 Revision 1.4  2006/08/31 13:51:59  code
 Added the ability to select seconds of year.

 Revision 1.3  2005/08/08 15:15:35  barnes
 Added option to dump the command line options

 Revision 1.2  2004/04/27 17:48:21  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/02/25 00:26:55  barnes
 Initial revision

 */



char buffer[256];

struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  FILE *fp=NULL;
  int status=0;
  int yr,mo,dy,hr,mt,sc;
  double etime;

  unsigned char day=0;
  unsigned char year=0;
  unsigned char help=0;
  unsigned char option=0;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"day",'x',&day);
  OptionAdd(&opt,"year",'x',&year);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  
  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;
  
  while ((fscanf(fp,"%d %d %d %d %d %d",&yr,&mo,&dy,&hr,&mt,&sc)==6) && 
	 (status==0)) {
    if (yr<1900) yr+=1900;
    if (year==1) {
      etime=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
    } else etime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    if (day==0) fprintf(stdout,"%d",(int) etime);
    else fprintf(stdout,"%d",(int) etime % (24*3600));
    if (fgets(buffer,256,fp)==NULL) {
      status=-1;
      break;
    }
    fprintf(stdout,buffer);
  }
  if (fp !=stdin) fclose(fp);
  return 0;

}
  
 







