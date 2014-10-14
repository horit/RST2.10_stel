/* map_fit.c 
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
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "fitmap.h"

#include "hlpstr.h"
#include "version.h"

/*
 $Log: map_fit.c,v $
 Revision 1.12  2005/08/10 22:42:36  barnes
 Added option to dump command line options.

 Revision 1.11  2004/10/12 22:51:37  barnes
 Moved to DataMap Format.

 Revision 1.10  2004/06/21 22:33:03  barnes
 Added missing header.

 Revision 1.9  2004/05/10 22:07:24  barnes
 Removed redundant code.

 Revision 1.8  2003/04/04 01:08:47  barnes
 Fixed bug in setting version number.

 Revision 1.7  2003/02/26 16:30:29  barnes
 Modification to deal with base libraries.

 Revision 1.6  2002/06/12 20:08:40  barnes
 Added code to set the source field in the output and
 the version numbers.

 Revision 1.5  2001/06/27 21:10:00  barnes
 Added license tag

 Revision 1.4  2000/08/17 15:00:24  barnes
 Changed return type of main().

 Revision 1.3  2000/07/31 20:13:06  barnes
 Added model and error weighting flags.

 Revision 1.2  1999/10/07 14:40:26  barnes
 Modifications to use the generic file loader.

 Revision 1.1  1999/09/27 22:14:16  barnes
 Initial revision

 */


struct CnvMapData map;
struct GridData grd;

struct OptionData opt;

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

  unsigned char vb=0;

  FILE *fp;   
  char *fname=NULL;

  int yr,mo,dy,hr,mt;
  double sc;
  char *source=NULL;

  char *ewstr=NULL;
  char *mwstr=NULL;

  int error_wt=-1;
  int model_wt=-1;
  int major=-1;
  int minor=-1;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"ew",'t',&ewstr);
  OptionAdd(&opt,"mw",'t',&mwstr);
 
  OptionAdd(&opt,"s",'t',&source);
  OptionAdd(&opt,"major",'i',&major);
  OptionAdd(&opt,"minor",'i',&minor);

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


  if (arg !=argc) fname=argv[arg];

  if (ewstr !=NULL) {
     if (tolower(ewstr[0])=='y') error_wt=1;
     else error_wt=0;
  }
   
  if (mwstr !=NULL) {
     if (tolower(mwstr[0])=='n') model_wt=1;
     else model_wt=0;
  }
   
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (old) {
    while (OldCnvMapFread(fp,&map,&grd) !=-1) {
          
      TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (error_wt !=-1) map.error_wt=error_wt;
      if (model_wt !=-1) map.model_wt=model_wt;

      if (source !=NULL) strcpy(map.source,source);
      else strcpy(map.source,"map_fit");
      if (major !=-1) map.major_rev=major;
      else map.major_rev=MAJOR_VERSION;
      if (minor !=-1) map.minor_rev=minor;
      else map.minor_rev=MINOR_VERSION;

      CnvMapFitMap(&map,&grd);
      OldCnvMapFwrite(stdout,&map,&grd);
      if (vb==1) 
        fprintf(stderr,
              "%d-%d-%d %d:%d:%d dp=%g error=%g chi_sqr=%g rms_err=%g\n",
	      yr,mo,dy,hr,mt,(int) sc,
	      map.pot_drop/1000,
    	      map.pot_drop_err/1000,map.chi_sqr,map.rms_err);
    }
  } else {
     while (CnvMapFread(fp,&map,&grd) !=-1) {
          
      TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (error_wt !=-1) map.error_wt=error_wt;
      if (model_wt !=-1) map.model_wt=model_wt;

      if (source !=NULL) strcpy(map.source,source);
      else strcpy(map.source,"map_fit");
      if (major !=-1) map.major_rev=major;
      else map.major_rev=MAJOR_VERSION;
      if (minor !=-1) map.minor_rev=minor;
      else map.minor_rev=MINOR_VERSION;

      CnvMapFitMap(&map,&grd);
      CnvMapFwrite(stdout,&map,&grd);
      if (vb==1) 
        fprintf(stderr,
              "%d-%d-%d %d:%d:%d dp=%g error=%g chi_sqr=%g rms_err=%g\n",
	      yr,mo,dy,hr,mt,(int) sc,
	      map.pot_drop/1000,
    	      map.pot_drop_err/1000,map.chi_sqr,map.rms_err);
    }
  }
   
  return 0;
}






















