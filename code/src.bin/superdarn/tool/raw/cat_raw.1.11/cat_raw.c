/* cat_raw.c
   =========
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

#include "oldrawread.h"
#include "oldrawwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: cat_raw.c,v $
 Revision 1.15  2007/01/30 18:11:09  code
 Fixed typo.

 Revision 1.14  2007/01/30 18:10:11  code
 Added the ability to override the threshold.

 Revision 1.13  2005/08/10 16:27:11  barnes
 Added option to dump the command line options.

 Revision 1.12  2004/05/10 21:32:34  barnes
 Removed redundant code.

 Revision 1.11  2003/04/07 18:58:43  barnes
 Changed header version.

 Revision 1.10  2003/02/25 02:09:44  barnes
 Changed the threshold command line option to "-t".

 Revision 1.9  2003/02/25 01:58:27  barnes
 Modification to use the new base libraries.

 Revision 1.8  2001/06/27 21:06:49  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.6  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.5  1999/04/10 19:11:03  barnes
 Fixed problem with printing the help message.

 Revision 1.4  1999/04/10 19:02:32  barnes
 Added help message system.

 Revision 1.3  1999/03/16 17:10:50  barnes
 Modifications to use the new raw library.

 Revision 1.2  1998/11/09 22:54:57  barnes
 Incorporated rawdata header.

 Revision 1.1  1998/07/15 18:37:03  barnes
 Initial revision

*/


struct RadarParm prm;
struct RawData raw;

struct OptionData opt;

int main (int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char new=0;
  float thr=-1;
  int hflg=1;
  int i;
  int recnum=0;
  struct OldRawFp *infp=NULL;
  char vstring[256];
  FILE *outfp=NULL;

  sprintf(vstring,"%d.%.3d",MAJOR_VERSION,MINOR_VERSION);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"t",'f',&thr);

  OptionAdd(&opt,"new",'x',&new);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (new==1) {
    fprintf(stderr,
    "New format files can be concatenated with the cat command.\n");
    exit(-1);
  }

  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  outfp=fopen(argv[argc-1],"w");
  if (outfp==NULL) { 
    fprintf(stderr,"Could not open output file.\n");
    exit(-1);
  }
    
  for (i=arg;i<argc-1;i++) {
    infp=OldRawOpen(argv[i],NULL);
    if (infp==NULL) {
      fprintf(stderr,"Could not open file %s.\n",argv[i]);
      continue;
    }
    while (OldRawRead(infp,&prm,&raw) !=-1) {
      if (thr !=-1) raw.thr=thr;
      if (hflg==1) {
	if (OldRawHeaderFwrite(outfp,"rawwrite",vstring,raw.thr,
			       "concatenated with cat_raw") !=0) {
	  fprintf(stderr,"Could not write header.\n");
	  exit(-1);
	}
        hflg=0;
      }


      recnum++;
      OldRawFwrite(outfp,"rawwrite",&prm,&raw,recnum,NULL);
    }    
    OldRawClose(infp);
  }
  fclose(outfp);

  return 0;
}






















