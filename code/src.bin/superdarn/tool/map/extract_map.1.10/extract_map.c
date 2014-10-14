/* extract_map.c
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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "hlpstr.h"

/* 
 $Log: extract_map.c,v $
 Revision 1.15  2005/08/10 21:48:11  barnes
 Added option to dump the command line options.

 Revision 1.14  2004/10/12 23:06:58  barnes
 Moved to DataMap format.

 Revision 1.13  2004/06/21 22:32:02  barnes
 Added missing header.

 Revision 1.12  2004/05/10 22:03:11  barnes
 Removed redundant code.

 Revision 1.11  2004/04/27 20:41:58  barnes
 Audit - Enforced warning set to "all".

 Revision 1.10  2003/02/26 03:22:14  barnes
 Modification to deal with base libraries.

 Revision 1.9  2002/03/05 14:44:00  barnes
 Added extra chi-squared parameter.

 Revision 1.8  2001/06/27 21:09:06  barnes
 Added license tag

 Revision 1.7  2001/06/11 14:14:51  barnes
 Changed command line option to "-mid".

 Revision 1.6  2001/06/11 14:08:39  barnes
 Fixed bugs in the command line arguments.

 Revision 1.5  2000/08/17 15:00:24  barnes
 Changed return type of main().

 Revision 1.4  2000/06/01 17:16:30  barnes
 Bug fix found by simon - the model direction was not being correctly
 recorded in the file.

 Revision 1.3  1999/10/07 22:15:53  barnes
 Removed debugging code.

 Revision 1.2  1999/10/07 14:42:40  barnes
 Modifications to use the new generic file format.

 Revision 1.1  1999/09/27 22:14:52  barnes
 Initial revision

*/

struct CnvMapData map;
struct GridData grd;

#define GRID 0x01
#define SCATTER 0x02
#define POTENTIAL 0x04
#define LATMIN 0x08

char *model_dir[]={
    "Bz+","Bz+/By+","By+","Bz-/By+","Bz-",
    "Bz-/By-","By-","Bz+/By-",0};

char *model_mag[]={"0<BT<4","4<BT<6","6<BT<12",0};

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
  int tme;
  int yr,mo,dy,eyr,emo,edy,hr,mt;
  double sc;
  int i,j;

  
  int flg=GRID;
  unsigned char rflg=0;
  unsigned char sflg=0;
  unsigned char pflg=0;
  unsigned char lflg=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"mid",'x',&rflg);
  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"p",'x',&pflg);
  OptionAdd(&opt,"l",'x',&lflg);

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
  if (arg !=argc) fname=argv[arg];
  if (sflg) flg=SCATTER;
  if (pflg) flg=POTENTIAL;
  if (lflg) flg=LATMIN;

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldCnvMapFread(fp,&map,&grd)!=-1) {
    
      if (flg==GRID) {

        if (vb==1) {
         TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d-%d-%d %d:%d:%d-",
                 yr,mo,dy,hr,mt,(int) sc); 
         TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d:%d:%d pnts=%d (%d)\n",hr,mt,(int) sc,
		 grd.vcnum,grd.stnum); 
	}
        OldGridFwrite(stdout,&grd);
      } else {
        if (rflg==0) {
         tme=(map.st_time+map.ed_time)/2.0;
         TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
        } else {
          TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
          TimeEpochToYMDHMS(map.ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
          if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
          fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
        }
        if (flg==SCATTER) {
          fprintf(stdout,"%d %d ",grd.vcnum,grd.stnum);
          for (i=0;i<grd.stnum;i++) fprintf(stdout,"%d ",
					      grd.sdata[i].npnt);
          for (i=0;i<grd.stnum;i++) fprintf(stdout,"%d ",
					      grd.sdata[i].st_id);
          fprintf(stdout,"\n");
        } else if (flg==POTENTIAL) {
          for (i=0;i<8;i++) if (strcmp(map.imf_model[1],model_dir[i])==0) 
                                break;
          for (j=0;j<3;j++) if (strcmp(map.imf_model[0],model_mag[j])==0)
                                 break;
          fprintf(stdout,"%g %g %g %g %d %d %d %d %g %g %g\n",
	    map.pot_drop/1000,map.Bx,map.By,map.Bz,
	    i,j,grd.vcnum,grd.stnum,map.chi_sqr,
            map.chi_sqr_dat,map.rms_err);
        } else if (flg==LATMIN) {
          fprintf(stdout,"%g\n",map.latmin);
        }
      }
    }


  } else {


     while (CnvMapFread(fp,&map,&grd)!=-1) {
    
      if (flg==GRID) {

        if (vb==1) {
         TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d-%d-%d %d:%d:%d-",
                 yr,mo,dy,hr,mt,(int) sc); 
         TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stderr,"%d:%d:%d pnts=%d (%d)\n",hr,mt,(int) sc,
		 grd.vcnum,grd.stnum); 
	}
        GridFwrite(stdout,&grd);
      } else {
        if (rflg==0) {
         tme=(map.st_time+map.ed_time)/2.0;
         TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
        } else {
          TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",
	      yr,mo,dy,hr,mt,(int) sc);
          TimeEpochToYMDHMS(map.ed_time,&eyr,&emo,&edy,&hr,&mt,&sc);
          if ((eyr !=yr) || (emo !=mo) || (edy !=dy)) hr+=24;
          fprintf(stdout,"%.2d %.2d %.2d ",hr,mt,(int) sc);
        }
        if (flg==SCATTER) {
          fprintf(stdout,"%d %d ",grd.vcnum,grd.stnum);
          for (i=0;i<grd.stnum;i++) fprintf(stdout,"%d ",
					      grd.sdata[i].npnt);
          for (i=0;i<grd.stnum;i++) fprintf(stdout,"%d ",
					      grd.sdata[i].st_id);
          fprintf(stdout,"\n");
        } else if (flg==POTENTIAL) {
          for (i=0;i<8;i++) if (strcmp(map.imf_model[1],model_dir[i])==0) 
                                break;
          for (j=0;j<3;j++) if (strcmp(map.imf_model[0],model_mag[j])==0)
                                 break;
          fprintf(stdout,"%g %g %g %g %d %d %d %d %g %g %g\n",
	    map.pot_drop/1000,map.Bx,map.By,map.Bz,
	    i,j,grd.vcnum,grd.stnum,map.chi_sqr,
            map.chi_sqr_dat,map.rms_err);
        } else if (flg==LATMIN) {
          fprintf(stdout,"%g\n",map.latmin);
        }
      }
    }
  }

  fclose(fp);
  return 0;
}






















