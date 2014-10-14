/* map_ascii.c
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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "aacgm.h"

#include "errstr.h"
#include "hlpstr.h"

#include "rmath.h"
#include "griddata.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapsolve.h"
#include "make_pgrid.h"


/*
 $Log: map_ascii.c,v $
 Revision 1.4  2005/08/10 22:35:38  barnes
 Added option to dump command line options.

 Revision 1.3  2004/11/30 15:42:56  barnes
 Modification to allow reading from standard input.

 Revision 1.2  2004/10/13 15:57:30  barnes
 Moved to DataMap format.

 Revision 1.1  2004/05/10 22:05:56  barnes
 Initial revision

*/

struct OptionData opt;

struct CnvMapData map;

struct GridData grd;

struct CnvGrid grid;
struct CnvGrid vgrid;

double *zbuffer=NULL;
int zwdt,zhgt;
struct polydata *ctr;

void opterr(char *txt) {
  fprintf(stderr,"Option not recognized:%s\n",txt);
}

double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

double strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   

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
  struct RfileIndex *oinx=NULL;
  struct CnvMapIndex *inx=NULL;
 

  int yr,mo,dy,hr,mt;
  double sc;

  FILE *grdfp;
  int i;

  int status;
 
  unsigned char help=0;
  unsigned char option=0;
 
  float latmin=60.0;
  float mlt;

 
 

  char *stime_txt=NULL;
  char *sdate_txt=NULL;

  char *extime_txt=NULL;
  char *etime_txt=NULL;
  char *edate_txt=NULL;


  double stime=-1;
  double etime=-1;
  double extime=0;
  double sdate=-1;
  double edate=-1;

  double glat,glon,r;

  int step=1;

  int num;
  int lat,lon;

  int *count=NULL;
  
  unsigned char vb=0;

  int fnum=0;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);

  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"st",'t',&stime_txt);
  OptionAdd(&opt,"sd",'t',&sdate_txt);
 
  OptionAdd(&opt,"et",'t',&etime_txt);
  OptionAdd(&opt,"ed",'t',&edate_txt);

  OptionAdd(&opt,"ex",'t',&extime_txt);

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

 
  if (stime_txt !=NULL)  stime=strtime(stime_txt);
  if (etime_txt !=NULL)  etime=strtime(etime_txt);
  if (extime_txt !=NULL) extime=strtime(extime_txt);
  if (sdate_txt !=NULL)  sdate=strdate(sdate_txt);
  if (edate_txt !=NULL)  edate=strdate(edate_txt);
 
  if (old) {
    if (arg<(argc-1)) {
        FILE *fp;
       fprintf(stderr,"Loading index.\n");
       fp=fopen(argv[argc-1],"r");
       if (fp !=NULL) {
         oinx=RfileLoadIndex(fp);
         fclose(fp);
       }
    }
  } else {
    if (arg<(argc-1)) {
        FILE *fp;
       fprintf(stderr,"Loading index.\n");
       fp=fopen(argv[argc-1],"r");
       if (fp !=NULL) {
         inx=CnvMapIndexFload(fp);
         fclose(fp);
       }
    }
  }


  if (arg !=argc) grdfp=fopen(argv[arg],"r");
  else grdfp=stdin;
  if (grdfp==NULL) {
     fprintf(stderr,"File not found.\n");
     exit(-1);
  }

  if (old) {
    if (OldCnvMapFread(grdfp,&map,&grd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  } else {
    if (CnvMapFread(grdfp,&map,&grd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(-1);
    }
  }

  if ((map.hemisphere==-1) && (latmin>0)) latmin=-latmin;
  
 if (stime !=-1) { /* we must skip the start of the files */
    int yr,mo,dy,hr,mt;
    double sc;  

    if (stime==-1) stime= ( (int) grd.st_time % (24*3600));
    if (sdate==-1) stime+=grd.st_time - ( (int) grd.st_time % (24*3600));
    else stime+=sdate;

    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (old) status=OldCnvMapFseek(grdfp,yr,mo,dy,hr,mt,sc,oinx,NULL);
    else status=CnvMapFseek(grdfp,yr,mo,dy,hr,mt,sc,NULL,inx);
    if (status ==-1) {
      fprintf(stderr,"File does not contain the requested interval.\n");
      exit(-1);
    }
    if (old) OldCnvMapFread(grdfp,&map,&grd);
    else CnvMapFread(grdfp,&map,&grd);
  } else stime=grd.st_time;
 
  if (etime !=-1) {
    if (edate==-1) etime+=grd.st_time - ( (int) grd.st_time % (24*3600));
    else etime+=edate;
  } 

  if (extime !=0) etime=stime+extime;
 
  do {

    if (vb !=0) {
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",dy,mo,yr,hr,mt,(int) sc);
    }

    if (fnum==0)  {
      make_pgrid(latmin,&grid);
      count=malloc(sizeof(int)*grid.num);
    }
    memset(count,0,grid.num*sizeof(int));
    grid.type=0;
    CnvMapSolve(&map,&grid);

    num=0;
    for (i=0;i<grid.num;i++) {
      if (fabs(grid.lat[i])<=fabs(map.latmin)) continue;
      num++;
    }
   
    for (i=0;i<grd.vcnum;i++) {
      lat= (fabs(grd.data[i].mlat) - 60.0) / 1.0;
      if (lat<0) continue;
      lon= grd.data[i].mlon /2.0;
      count[lat*180+lon]++;
    }

    TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%d %d %d %d %d %d ",yr,mo,dy,hr,mt,(int) sc);
    TimeEpochToYMDHMS(grd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%d %d %d %d %d %d %g\n",yr,mo,dy,hr,mt,(int) sc,
            map.latmin);
        
    fprintf(stdout,"%d\n",num);
    
    for (i=0;i<grid.num;i++) {
      if (fabs(grid.lat[i])<=fabs(map.latmin)) continue;
      fprintf(stdout,"%#10g %10d ",grid.mag[i],count[i]);
      AACGMConvert(grid.lat[i],grid.lon[i],300.0,&glat,&glon,&r,1);
      fprintf(stdout,"%#10g %#10g ",glat,glon);
      mlt=grid.lon[i]/15.0+map.mlt.av;
      if (mlt>24.0) mlt-=24.0;  
      fprintf(stdout,"%#10g %#10g %#10g\n",grid.lat[i],grid.lon[i],mlt);
    } 
    
   fnum++;
   if (old) for (i=0;i<step;i++) status=OldCnvMapFread(grdfp,&map,&grd);
   else for (i=0;i<step;i++) status=CnvMapFread(grdfp,&map,&grd);
   if ((etime !=-1) && (grd.ed_time>=etime)) break; 
  } while (status !=-1);
  
  if (grdfp !=stdin) fclose(grdfp);
  return 0;  
}












