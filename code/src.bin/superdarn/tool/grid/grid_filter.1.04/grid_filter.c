/* grid_filter.c
   ============= 
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
#include "option.h"
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "hlpstr.h"


/*
 $Log: grid_filter.c,v $
 Revision 1.5  2005/08/10 20:02:06  barnes
 Added option to dump the command line options.

 Revision 1.4  2004/10/08 16:46:13  barnes
 Added DataMap format.

 Revision 1.3  2004/05/10 21:00:52  barnes
 Removed redundant code.

 Revision 1.2  2003/03/13 00:19:08  barnes
 Modification for the base libraries.

 Revision 1.1  2001/11/27 15:19:23  barnes
 Initial revision

*/


struct OptionData opt;
struct GridData grd;
  
int skip=10*60; /* skip time */

int main(int argc,char *argv[]) {

 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  double tval=0,dval=0;
  int c=0;
 
  FILE *fp;
   
  int old=0;
  int new=0;


  int arg=0;
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


  if (arg !=argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(1);
    }
  } else fp=stdin;
  
  if (old) {
    while (OldGridFread(fp,&grd) !=-1)  {
    
      if (c==0) dval=grd.st_time-((int) grd.st_time % (24*3600));
      if ((grd.st_time-dval)>=tval) {
        OldGridFwrite(stdout,&grd);
        tval+=skip;
      }
    }
    if (fp !=stdin) fclose(fp); 
  } else {
    while (GridFread(fp,&grd) !=-1)  {
    
      if (c==0) dval=grd.st_time-((int) grd.st_time % (24*3600));
      if ((grd.st_time-dval)>=tval) {
        GridFwrite(stdout,&grd);
        tval+=skip;
      }
    }
    if (fp !=stdin) fclose(fp); 
  }
  return 0;
}







