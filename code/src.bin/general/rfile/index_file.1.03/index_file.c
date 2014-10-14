/* index_file.c
   ============
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
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "option.h"
#include "hlpstr.h"

/* 
 $Log: index_file.c,v $
 Revision 1.5  2005/08/08 15:08:59  barnes
 Added option to dump the command line options

 Revision 1.4  2004/05/05 16:07:57  barnes
 Added missing header.

 Revision 1.3  2004/05/04 19:12:30  barnes
  Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 17:46:32  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/04/25 14:55:43  barnes
 Initial revision

 */


struct OptionData opt;

int main(int argc,char *argv[]) {
  int ptr;
  int arg;
  FILE *fp;
  int yr,mo,dy,hr,mt; 
  double sc;
  double tme[2];
  void *tptr[2];
  unsigned char help=0;
  unsigned char option=0;

 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

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
  
  tptr[0]=tme;

  ptr=ftell(fp);

  while (RfileRead(fp,0,NULL,tptr) !=-1) {
    TimeEpochToYMDHMS(tme[0],&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %g ",yr,mo,dy,hr,mt,sc);
    TimeEpochToYMDHMS(tme[1],&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %g ",yr,mo,dy,hr,mt,sc);
    fprintf(stdout,"%d\n",ptr);
    ptr=ftell(fp);
  }
  if (arg<argc) fclose(fp);
  return 0;

}
  





