/* locate.c
   ======== 
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

/* This is a simple test program to demonstrate how to read a CDF file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>

#include "rtypes.h"
#include "rtime.h"
#include "cdf.h"
#include "rcdf.h"
#include "locate.h"

/* 
 $Log: locate.c,v $
 Revision 1.8  2004/08/29 21:37:38  barnes
 Fixes for Mac OS X.

 Revision 1.7  2004/05/10 22:04:41  barnes
 Removed redundant code.

 Revision 1.6  2003/03/09 17:34:24  barnes
 Changed CDF header name.

 Revision 1.5  2003/02/26 16:09:24  barnes
 Modification to deal with base libraries.

 Revision 1.4  2002/11/25 21:04:42  barnes
 Added support for the h0 data files.

 Revision 1.3  2001/06/27 21:09:30  barnes
 Added license tag

 Revision 1.2  2000/12/06 15:34:46  barnes
 Fixed incompatible pointer types.

 Revision 1.1  2000/07/31 17:36:29  barnes
 Initial revision

 */

#define ONE_MONTH (28*24*3600)

extern struct RCDFData data[10];

void free_locate(struct file_list *ptr) {
  int i;
  for (i=0;i<ptr->cnt;i++) if (ptr->fname[i] !=NULL) free(ptr->fname[i]);
  free(ptr);

}

int infname(char *body,char *str) { 
  
  int i=0; 
  int j=0;
  while (body[i] !=0) {
    if (body[i]==str[j]) j++;
    else j=0;
    if (str[j]==0) break;
    i++;
  }
  if (body[i]==0) return 0;
  return 1;
}


#ifdef _DARWIN 
int dsel(struct dirent *dp) {
#else
int dsel(const struct dirent *dp) {
#endif
  if (dp->d_name[0]=='.') return 0;
  return 1;
}

int test_file_epoch(char *fname,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double fstime=0,fetime=0;
  int max=0;  
  CDFid id;
  CDFstatus status;
  double *dptr;
  char *varlist[]={"Epoch",NULL};
 
fprintf(stderr,"inside of test file epoch\n");
  status=CDFopen(fname,&id);
fprintf(stderr,"after cdfopen, status=%s\n",status);
  if (status !=CDF_OK) return -1;

  
fprintf(stderr,"maybe we can reach here\n");
  max=RCDFMaxRecR(id,varlist[0]);
fprintf(stderr,"maybe we can not reach here\n");
  if (max>0) {
     status=RCDFReadR(id,0,varlist,data);
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fstime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
     status=RCDFReadR(id,max-1,varlist,data);
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fetime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
  }
  max=RCDFMaxRecZ(id,varlist[0]);
  if (max>0) {
     status=RCDFReadZ(id,0,varlist,data);
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fstime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
     status=RCDFReadZ(id,max-1,varlist,data);
     dptr=(double *) data[0].data;
     EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
     fetime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
  }
  CDFclose(&id);

  if (stime>fetime) return 0;
  if (etime<fstime) return 0;
  return 1;
}

struct file_list *locate_files(char *fpath,char *prefix,
                               double stime,double etime) {
  char filename[1024];
  char tmp[256];
  char dir[256];
  char fname[256];
  char ofname[256];

  char *flist[4096];
  int fcnt=0;

  int status;
  int dyval;
  struct dirent **dlist=NULL;
  int yr,mo,dy,hr,mt;
  double sc;
  int m;
  int state=0;
  int syr,eyr;
  int sdyval,edyval;
  
  struct file_list *ptr;
  ptr=malloc(sizeof(struct file_list));
  ptr->cnt=0;
  ptr->fname=NULL;
  
  ofname[0]=0;

  /* The yearc check should look one month either side
     of the desired period to make sure that we catch all
     possible files that might contain the requested period, even if
     they span directories.  */

  TimeEpochToYMDHMS(stime-ONE_MONTH,&syr,&mo,&dy,&hr,&mt,&sc);
  TimeEpochToYMDHMS(etime+ONE_MONTH,&eyr,&mo,&dy,&hr,&mt,&sc);

  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  sdyval=yr*10000+mo*100+dy;
  TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);
  edyval=yr*10000+mo*100+dy;

  
  for (yr=syr;yr<=eyr;yr++) {
    
 
    sprintf(dir,"%s/%.4d",fpath,yr);
    fprintf(stderr,"%s\n",dir);
   
    status=scandir(dir,&dlist,dsel,alphasort);
    
    fprintf(stderr,"yr=%d\n",yr);
    if (status<=0) continue;

    /* now get only the most recent version numbers of the files */

    fcnt=0;
    fprintf(stderr,"status=%d\n",status);

    for (m=0;m<status;m++) {
    fprintf(stderr,"m=%d/%d\n",m,status);
      
    fprintf(stderr,"prefix=%s\n",prefix);
fprintf(stderr,"%d\n",infname(dlist[m]->d_name,prefix));
      if ((prefix !=NULL) && (infname(dlist[m]->d_name,prefix) ==0)) continue;
      
fprintf(stderr,"%s\n",dlist[m]->d_name);
fprintf(stderr,"fcnt=%d\n",fcnt);
      if (fcnt==0) {
       flist[fcnt]=dlist[m]->d_name;
       fcnt++;
      } else {
        strcpy(tmp,dlist[m]->d_name);
        strcpy(tmp+128,flist[fcnt-1]);
        tmp[18]=0;
        tmp[128+18]=0;
       
        
        if (strcmp(tmp,tmp+128)==0) flist[fcnt-1]=dlist[m]->d_name;
        else {
          flist[fcnt]=dlist[m]->d_name;
          fcnt++;
        }
      
      }
    }
   
    for (m=0;m<fcnt;m++) {
        strcpy(tmp,flist[m]);
        strcpy(fname,flist[m]);
        tmp[18]=0;
        dyval=atoi(tmp+10); 
      
        if ((dyval>=sdyval) && (state==0)) {
          if (ofname[0] !=0) {
             sprintf(filename,"%s/%s",dir,ofname);
             status=test_file_epoch(filename,stime,etime);
             if (status==1) {
               if (ptr->fname==NULL) ptr->fname=malloc(sizeof(char *));
               else ptr->fname=realloc(ptr->fname,
                                      (ptr->cnt+1)*sizeof(char *));
               ptr->fname[ptr->cnt]=malloc(strlen(filename)+1);
               strcpy(ptr->fname[ptr->cnt],filename);
               ptr->cnt++;
	    }
	  }
          state=1;
	}
	      
        if ((state==1) && (dyval<=edyval)) {
          sprintf(filename,"%s/%s",dir,fname);
  fprintf(stderr,"%s\n",filename);
          status=test_file_epoch(filename,stime,etime);
          if (status==1) {
             if (ptr->fname==NULL) ptr->fname=malloc(sizeof(char *));
             else ptr->fname=realloc(ptr->fname,
                                    (ptr->cnt+1)*sizeof(char *));
             ptr->fname[ptr->cnt]=malloc(strlen(filename)+1);
             strcpy(ptr->fname[ptr->cnt],filename);
             ptr->cnt++;
	  }
	} else if (state==1) state=2;
        strcpy(ofname,fname);
             
      }
  }
  
  fprintf(stderr,"last line of locate.c\n");
  return ptr;
}
   




