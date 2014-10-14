/* oldwritegrid.c
   ===========
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
#include "griddata.h"

/*
 $Log: oldwritegrid.c,v $
 Revision 1.2  2004/10/08 17:02:40  barnes
 Modified griddata structure.

 Revision 1.1  2004/10/07 20:03:22  barnes
 Initial revision

*/

#define NBLOCKS 2

int OldGridEncodeOne(FILE *fp,struct GridData *ptr) {
  int i;
  char *name[]={"st_id","chn","nvec","freq0","major_rev","minor_rev",
                "prog_id","noise_mean","noise_sd","gsct","v_min","v_max",
                "p_min","p_max","w_min","w_max","ve_min","ve_max",0};
  char *unit[]={"count","count","count","KHz","count","count",
                "count","count","count","count","m/sec","m/sec",
                "dB","dB","m/sec","m/sec","m/sec","m/sec",0};
  char *type[]={"int","int","int","float","int","int","int",
                "float","float","int","float","float","float","float",
                "float","float","float","float",0};

  int npnt=0;
  for (i=0;i<ptr->stnum;i++) if (ptr->sdata[i].st_id !=-1) npnt++;
  
  fprintf(fp,"%d %d\n",npnt,18);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<ptr->stnum;i++) if (ptr->sdata[i].st_id !=-1) {  
      fprintf(fp,"%10d %10d %10d %#10g %10d %10d ",
      ptr->sdata[i].st_id,ptr->sdata[i].chn,
      ptr->sdata[i].npnt,ptr->sdata[i].freq0,
      (int) ptr->sdata[i].major_revision, (int) ptr->sdata[i].minor_revision);
      fprintf(fp,"%10d %#10g %#10g %10d ",
      ptr->sdata[i].prog_id,ptr->sdata[i].noise.mean,ptr->sdata[i].noise.sd,
      (int) ptr->sdata[i].gsct);
      fprintf(fp,"%#10g %#10g %#10g %#10g %#10g %#10g %#10g %#10g\n",
              ptr->sdata[i].vel.min,ptr->sdata[i].vel.max,
              ptr->sdata[i].pwr.min,ptr->sdata[i].pwr.max,
              ptr->sdata[i].wdt.min,ptr->sdata[i].wdt.max,
              ptr->sdata[i].verr.min,ptr->sdata[i].verr.max);




   }
  return npnt;
}



int OldGridEncodeTwo(FILE *fp,struct GridData *ptr) {
  int i;
  char *name[]={"gmlong","gmlat","kvect","st_id","chn","grid_index",
                   "vlos","vlos_sd",0};
  char *unit[]={"degrees","degrees","degrees","count","count",
                   "count","m/sec","m/sec",0};
  char *type[]={"float","float","float","int","int","int","float","float",0};

  int npnt=0;

  
  for (i=0;i<ptr->vcnum;i++) if (ptr->data[i].st_id !=-1) npnt++;
  
  fprintf(fp,"%d %d\n",npnt,8);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<ptr->vcnum;i++) {
    if (ptr->data[i].st_id==-1) continue;
    fprintf(fp,"%#10g %#10g %#10g %10d %10d %10d %#10g %#10g\n",
              ptr->data[i].mlon,
              ptr->data[i].mlat,
              ptr->data[i].azm,
              ptr->data[i].st_id,
 	      ptr->data[i].chn,
              ptr->data[i].index,
              ptr->data[i].vel.median,
              ptr->data[i].vel.sd);
 
  
  }
  return npnt;
}



int OldGridEncodeThree(FILE *fp,struct GridData *ptr) {
  int i;
  char *name[]={"gmlong","gmlat","kvect","st_id","chn","grid_index",
                   "vlos","vlos_sd",
                   "pwr","pwr_sd","wdt","wdt_sd",0};
  char *unit[]={"degrees","degrees","degrees","count","count",
                   "count","m/sec","m/sec","dB","dB","m/sec","m/sec",0};
  char *type[]={"float","float","float","int","int","int","float","float",
                "float","float","float","float",0};

  int npnt=0;
 
  for (i=0;i<ptr->vcnum;i++) if (ptr->data[i].st_id !=-1) npnt++;
  
  fprintf(fp,"%d %d\n",npnt,12);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<ptr->vcnum;i++) {
    if (ptr->data[i].st_id==-1) continue;
    fprintf(fp,
      "%#10g %#10g %#10g %10d %10d %10d %#10g %#10g %#10g %#10g %#10g %#10g\n",
              ptr->data[i].mlon,
              ptr->data[i].mlat,
              ptr->data[i].azm,
              ptr->data[i].st_id,
	      ptr->data[i].chn,
              ptr->data[i].index,
              ptr->data[i].vel.median,
              ptr->data[i].vel.sd,
              ptr->data[i].pwr.median,
              ptr->data[i].pwr.sd,
              ptr->data[i].wdt.median,
              ptr->data[i].wdt.sd);
 
  
  }
  return npnt;
}


int OldGridFwrite(FILE *fp,struct GridData *ptr) {
  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  double sec;
  int nfile,npnt;
 
  TimeEpochToYMDHMS(ptr->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
  ssc=(int) sec;
  TimeEpochToYMDHMS(ptr->ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
  esc=(int) sec;
 
  fprintf(fp,"%d %d %d %d %d %d ",syr,smo,sdy,shr,smt,ssc);
  fprintf(fp,"%d %d %d %d %d %d\n",eyr,emo,edy,ehr,emt,esc);
  fprintf(fp,"%d\n",NBLOCKS);

  nfile=OldGridEncodeOne(fp,ptr);
  if (ptr->xtd) npnt=OldGridEncodeThree(fp,ptr);
  else npnt=OldGridEncodeTwo(fp,ptr);
 
  return 0;
}





