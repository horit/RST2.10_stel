/* extract_time.c
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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rmath.h"
#include "rfile.h"
#include "rtime.h"
#include "simage.h"

#include "errstr.h"
#include "hlpstr.h"


/*
 $Log: extract_center.c,v $
 Revision 1.4  2007/03/15 20:52:33  code
 Added help and command line option listing.

 Revision 1.3  2004/08/26 20:39:47  barnes
 Fixed bad comment

 Revision 1.2  2004/08/26 19:47:42  barnes
 Added author name.

 Revision 1.1  2004/05/10 18:13:59  barnes
 Initial revision

*/


struct OptionData opt;
int arg=0;


struct SImage simage;

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;


  int yr,mo,dy,hr,mt;
  double sc;
  int s=0;
  int fnum;
  FILE *fp;

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

  if (arg==argc) {
    OptionPrintInfo(stdout,errstr);
    exit(1);
  }

  for (fnum=arg;fnum<argc;fnum++) {
    fp=fopen(argv[fnum],"r");

    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      continue;
    }
    s=SImageFread(fp,&simage);
    fclose(fp);
    if (s==-1) continue;
    TimeEpochToYMDHMS(simage.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %g ",yr,mo,dy,hr,mt,sc);
    TimeEpochToYMDHMS(simage.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %g ",yr,mo,dy,hr,mt,sc);
    fprintf(stdout,"%d %g %g ",simage.c.flg,simage.c.glat,simage.c.glon);
    fprintf(stdout,"%s\n",argv[fnum]);
  }
  return 0;  
}







