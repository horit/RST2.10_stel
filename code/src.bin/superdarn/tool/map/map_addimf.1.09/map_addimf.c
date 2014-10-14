/* map_addimf.c
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
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"

#include "hlpstr.h"

#include "cdf.h"
#include "rcdf.h"
#include "locate.h"

#include "istp.h"
#include "wind.h"
#include "ace.h"

/*
 $Log: map_addimf.c,v $
 Revision 1.15  2005/08/10 22:23:02  barnes
 Added option to dump command line options.

 Revision 1.14  2004/10/12 22:05:47  barnes
 Moved to DataMap format.

 Revision 1.13  2004/08/29 21:37:38  barnes
 Fixes for Mac OS X.

 Revision 1.12  2004/05/10 22:04:41  barnes
 Removed redundant code.

 Revision 1.11  2003/03/09 17:34:24  barnes
 Changed CDF header name.

 Revision 1.10  2003/02/26 16:09:24  barnes
 Modification to deal with base libraries.

 Revision 1.9  2002/11/25 21:04:42  barnes
 Added support for the h0 data files.

 Revision 1.8  2001/06/27 21:09:30  barnes
 Added license tag

 Revision 1.7  2001/03/02 15:10:29  barnes
 Added filter to block bad IMF data values.

 Revision 1.6  2000/08/17 15:00:24  barnes
 Changed return type of main().

 Revision 1.5  2000/08/14 14:31:48  barnes
 Added the ability to set the data path name.

 Revision 1.4  2000/08/02 20:46:44  barnes
 Removed debugging code.

 Revision 1.3  2000/08/02 20:46:01  barnes
 Added the ability to set the expected file extent on the command line.

 Revision 1.2  2000/08/01 14:03:46  barnes
 Added the ability to read the delay time from a file and to use non-standard
 IMF data from a plain text file.

 Revision 1.1  2000/07/31 17:36:29  barnes
 Initial revision

 */

char *fname=NULL;
FILE *fp;

struct GridData grd;
struct CnvMapData  map;
 
char dpath[256]={"/data"};

double st_time;
double ed_time;

struct file_list *fptr;
struct imfdata imf;

struct RCDFData data[10];

struct OptionData opt;

#define DELAYSTEP 50
#define IMFSTEP 50

struct delaytab {
  int num;
  double *time;
  float *delay;
};


int load_text(FILE *fp,struct imfdata *ptr) {

  int yr,mo,dy,hr,mt;
  float sc;
  float bx,by,bz;
  char line[256];
  int i;
  int cnt=0;

  ptr->time=malloc(sizeof(double)*IMFSTEP);
  ptr->BGSMc=malloc(sizeof(float)*IMFSTEP*3);
  ptr->BGSEc=malloc(sizeof(float)*IMFSTEP*3);
   while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]==' ') || (line[i]=='\t') ||
             (line[i] =='\n'));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;
  
    if (sscanf(line,"%d %d %d %d %d %g %g %g %g",&yr,&mo,&dy,&hr,&mt,&sc,
              &bx,&by,&bz) != 9) continue;
    ptr->time[cnt]=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->BGSMc[cnt*3]=bx;
    ptr->BGSMc[cnt*3+1]=by;
    ptr->BGSMc[cnt*3+2]=bz;

    ptr->BGSEc[cnt*3]=bx;   /* fudge as we assume the file contains */
    ptr->BGSEc[cnt*3+1]=by; /* only one set of values. */
    ptr->BGSEc[cnt*3+2]=bz;

    cnt++;
    if ((cnt % IMFSTEP)==0) {
        int blk;
        blk=1+cnt/IMFSTEP;
        ptr->time=realloc(ptr->time,sizeof(double)*IMFSTEP*blk);
        ptr->BGSMc=realloc(ptr->BGSMc,sizeof(float)*IMFSTEP*blk*3);
        ptr->BGSEc=realloc(ptr->BGSEc,sizeof(float)*IMFSTEP*blk*3);

    }

  }
  ptr->cnt=cnt;
  ptr->time=realloc(ptr->time,sizeof(double)*cnt);
  ptr->BGSMc=realloc(ptr->BGSMc,sizeof(float)*cnt*3);
  ptr->BGSMc=realloc(ptr->BGSMc,sizeof(float)*cnt*3);
  return 0;
}

struct delaytab *load_delay(FILE *fp) {
  struct delaytab *ptr;
  int yr,mo,dy,hr,mt;
  float sc;
  int dhr,dmt;
  char line[256];
  int i;
  int cnt=0;
  ptr=malloc(sizeof(struct delaytab));
  ptr->time=malloc(sizeof(double)*DELAYSTEP);
  ptr->delay=malloc(sizeof(float)*DELAYSTEP);
 
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]==' ') || (line[i]=='\t') ||
             (line[i] =='\n'));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;
  
    if (sscanf(line,"%d %d %d %d %d %g %d %d",&yr,&mo,&dy,&hr,&mt,&sc,
              &dhr,&dmt) != 8) continue;

    ptr->time[cnt]=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->delay[cnt]=dhr*3600+dmt*60;
    cnt++;
    if ((cnt % DELAYSTEP)==0) {
        int blk;
        blk=1+cnt/DELAYSTEP;
        ptr->time=realloc(ptr->time,sizeof(double)*DELAYSTEP*blk);
        ptr->delay=realloc(ptr->delay,sizeof(float)*DELAYSTEP*blk);
    }

  }

  ptr->num=cnt;
  ptr->time=realloc(ptr->time,sizeof(double)*cnt);
  ptr->delay=realloc(ptr->delay,sizeof(float)*cnt);
 
  return ptr;
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



int load_wind() {

  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 
  sprintf(path,"%s/%s",dpath,"wind");

  fprintf(stderr,"%s\n",path);

  fptr=locate_files(path,"mfi",st_time,ed_time);

  for (i=0;i<fptr->cnt;i++) {
    fprintf(stderr,"%s\n",fptr->fname[i]);

     status=CDFopen(fptr->fname[i],&id);
    if (status !=CDF_OK) {
      fprintf(stderr,"Could not open cdf file.\n");
      continue;
    }
  
    status=windmfi_imf(id,&imf,st_time,ed_time);
    
    CDFclose(id);
  }
  free_locate(fptr);
  return 0;
}

int load_ace() {

  int i;
  char path[256];

  CDFid id;
  CDFstatus status;
 
  sprintf(path,"%s/%s",dpath,"ace");
  fprintf(stderr,"%s\n",path);

  /* first check to see if we have the h0 files */

  fptr=locate_files(path,"h0_mfi",st_time,ed_time);
  fprintf(stderr,"HeyHey\n");

  if (fptr->cnt !=0) {
    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);
      status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
      status=acemfi_imf(id,&imf,st_time,ed_time);
    
      CDFclose(id);
    }
    free_locate(fptr);
  } else {
    free_locate(fptr);    
    fptr=locate_files(path,"k1_mfi",st_time,ed_time);

    for (i=0;i<fptr->cnt;i++) {
      fprintf(stderr,"%s\n",fptr->fname[i]);

       status=CDFopen(fptr->fname[i],&id);
      if (status !=CDF_OK) {
        fprintf(stderr,"Could not open cdf file.\n");
        continue;
      }
    
      status=acemfi_imf(id,&imf,st_time,ed_time);
    
      CDFclose(id);
    }
    free_locate(fptr);
  }
  return 0;
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
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  char *envstr;
 
  char *dname=NULL;
  struct delaytab *dtable=NULL;

  char *iname=NULL;
 

  unsigned char aflg=0,wflg=0;

  int yr,mo,dy,hr,mt;
  double sc;
  double tme;
  int b;
  int s;
  float extent=24*3600;
  float delay=1800;
  float dBx=0;
  float dBy=0;
  float dBz=0;

  char *pstr=NULL;
  char *dstr=NULL;
  char *estr=NULL;

  int j,k;
 
 
  envstr=getenv("ISTP_PATH");
  if (envstr !=NULL) strcpy(dpath,envstr);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"ace",'x',&aflg);
  OptionAdd(&opt,"wind",'x',&wflg);
  OptionAdd(&opt,"if",'t',&iname);
  OptionAdd(&opt,"df",'t',&dname);

  OptionAdd(&opt,"p",'t',&pstr);
  OptionAdd(&opt,"d",'t',&dstr);

  OptionAdd(&opt,"bx",'f',&dBx);
  OptionAdd(&opt,"by",'f',&dBy);
  OptionAdd(&opt,"bz",'f',&dBz);

  OptionAdd(&opt,"ex",'t',&estr);

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


  if (pstr !=NULL) strcpy(dpath,pstr);
  if (dstr !=NULL) delay=strtime(dstr);  
  if (estr !=NULL) extent=strtime(estr);
  
  if (arg !=argc) fname=argv[arg];


  if (dname !=NULL) {
    fp=fopen(dname,"r");
    if (fp !=NULL) {
     dtable=load_delay(fp);
     fclose(fp);
    }
  }  

  if (iname !=NULL) {
    fp=fopen(iname,"r");
    if (fp !=NULL) {
     load_text(fp,&imf);
     fclose(fp);
    }
  }


  if (fname !=NULL) {
    fp=fopen(fname,"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;

  if (dtable !=NULL) delay=dtable->delay[0];

  if (old) s=OldCnvMapFread(fp,&map,&grd);
  else s=CnvMapFread(fp,&map,&grd);

  st_time=map.st_time-delay;
  ed_time=map.st_time-delay+extent; 

  if (wflg==1) load_wind();
  else if (aflg==1) load_ace();
    

 

  j=0;
  k=0;

  do {  

    if (dtable !=NULL) {
      while ((k<dtable->num) && (dtable->time[k]<=map.st_time)) k++;
      if (k==0) delay=dtable->delay[0];
      else delay=dtable->delay[k-1];
    }  
    
 
    tme=map.st_time-delay;
    map.Bx=dBx;
    map.By=dBy;
    map.Bz=dBz;
    map.imf_flag=9;


    

    if (imf.cnt>0) {
      while ((imf.time[j]<tme) && (j<imf.cnt)) j++;
      if (j>0) j--; /* correct so that we get the right start point */
     
      map.imf_flag=9;

      if (j<imf.cnt) {
        map.Bx=0;
        map.By=0;
        map.Bz=0;
        tme=map.ed_time-delay;
        b=0;
        do {
          if ((fabs(imf.BGSMc[3*j])<500) &&
              (fabs(imf.BGSMc[3*j+1])<500) &&
	      (fabs(imf.BGSMc[3*j+1])<500)) {
            map.Bx+=imf.BGSMc[3*j];
            map.By+=imf.BGSMc[3*j+1];
            map.Bz+=imf.BGSMc[3*j+2];
            b++;
	  }
          j++;
        } while ((imf.time[j]<tme) && (j<imf.cnt));
        if (j>0) j--;


        if (b>0) {
          map.Bx=map.Bx/b;
          map.By=map.By/b;
          map.Bz=map.Bz/b;
          map.imf_flag=1;

          dBx=map.Bx; /* this now becomes the default */
          dBy=map.By;
          dBz=map.Bz;

        } else {
          map.Bx=dBx;
          map.By=dBy;
          map.Bz=dBz;
        }
      }
    }
    map.imf_delay=delay/60;
    if (old) OldCnvMapFwrite(stdout,&map,&grd);
    else CnvMapFwrite(stdout,&map,&grd);

    if (vb==1) {
       TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
       fprintf(stderr,
               "%d-%d-%d %d:%d:%d delay=%d:%d Bx=%g By=%g Bz=%g\n",
               yr,mo,dy,hr,mt,(int) sc,(int) (delay/3600),
               ( (int) delay % 3600)/60,
               map.Bx,map.By,map.Bz);
    }  

    if (old) s=OldCnvMapFread(fp,&map,&grd);
    else s=CnvMapFread(fp,&map,&grd);

  } while (s!=-1);


  fclose(fp); 
  return 0; 
}







