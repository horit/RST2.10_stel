/* makesimageinx.c
   =============== */

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
#include "rmath.h"
#include "rconvert.h"
#include "rfile.h"
#include "rtime.h"
#include "option.h"
#include "simage.h"
#include "hlpstr.h"

/*
 $Log: makesimageinx.c,v $
 Revision 1.4  2005/08/09 15:17:46  barnes
 Added the ability to dump the command line options.

 Revision 1.3  2004/08/26 19:59:45  barnes
 Added help message.

 Revision 1.2  2004/06/22 18:21:54  barnes
 Changed references of integers to "int".

 Revision 1.1  2004/05/10 18:15:59  barnes
 Initial revision

*/


struct SImage simage;
struct OptionData opt;


int main(int argc,char *argv[]) { 
  int sze=0;
  int s=0;
  FILE *fp=NULL;
  int arg;
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


  while ((s=SImageFread(fp,&simage)) !=-1) {
    fprintf(stderr,"%d\n",sze);
      ConvertFwriteDouble(stdout,simage.st_time);
      ConvertFwriteDouble(stdout,simage.ed_time);
      ConvertFwriteInt(stdout,sze);
      sze+=SImageWrite(-1,&simage);


  }
  if (fp !=stdin) fclose(fp);
  return 0;
}







