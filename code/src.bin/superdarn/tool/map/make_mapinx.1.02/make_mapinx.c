/* make_mapinx.c
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "griddata.h"
#include "cnvmap.h"

#include "cnvmapread.h"
#include "cnvmapwrite.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: make_mapinx.c,v $
 Revision 1.2  2005/08/10 22:08:48  barnes
 Added option to dump command line options.

 Revision 1.1  2004/10/19 21:19:31  barnes
 Initial revision

 Revision 1.2  2004/10/08 19:57:36  barnes
 Added verbose mode.

 Revision 1.1  2004/10/08 17:49:18  barnes
 Initial revision

*/


struct OptionData opt;
struct GridData grd;
struct CnvMapData map;


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

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
 
  FILE *fp;
  
  int sze=0;
  int yr,mo,dy,hr,mt;
  double sc; 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

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



  if (old) {
    fprintf(stderr,
    "Index files for old format map files cannot be created.\n");
    exit(-1);
  }

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
    
  while (CnvMapFread(fp,&map,&grd) !=-1) {
    if (vb) {
      TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
              yr,mo,dy,hr,mt,(int) sc,grd.vcnum);
    }
    ConvertFwriteDouble(stdout,map.st_time);
    ConvertFwriteInt(stdout,sze);
    sze+=CnvMapWrite(-1,&map,&grd);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















