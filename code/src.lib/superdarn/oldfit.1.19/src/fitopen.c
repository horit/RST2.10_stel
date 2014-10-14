/* fitopen.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"

#include "fitversions.h"
#include "fitstr.h"

/*
 $Log: fitopen.c,v $
 Revision 1.7  2004/08/04 20:54:46  barnes
 Changed function name.

 Revision 1.6  2004/08/04 20:51:18  barnes
 Removed old code for memory buffering.

 Revision 1.5  2004/08/04 20:34:35  barnes
 Added function to create file pointer for open files.

 Revision 1.4  2004/06/16 21:46:39  barnes
 Added missing header.

 Revision 1.3  2004/05/04 18:32:13  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:23:36  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

struct OldFitFp *OldFitOpenFd(int fitfp,int inxfp) {

  
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,ORIG_PULSE_PAT_LEN,2,2*ORIG_LAG_TAB_LEN,1,
                ORIG_COMBF_SIZE,4,3,
		2,2*ORIG_MAX_RANGE,1,ORIG_MAX_RANGE,0,0};
  int status=0;
  union fit_out r;
  struct OldFitFp *ptr=NULL;
  int16 tmp,i,j;
  int32 inx_buf[4];
  struct radar_parms *prms=NULL;
  char *tmpbuf;
  ptr=malloc(sizeof(struct OldFitFp));
  if (ptr==NULL) return NULL;
 
  ptr->fitfp=-1;
  ptr->inxfp=-1;
  ptr->ctime=-1;
  ptr->stime=-1;
  ptr->etime=-1;
  ptr->fitfp=fitfp;
  ptr->inxfp=inxfp;
 
  fstat(ptr->fitfp,&ptr->fstat);
  fstat(ptr->inxfp,&ptr->istat);

 
  
  ConvertReadShort(ptr->fitfp,&tmp);
  ptr->fit_recl=tmp;
  ConvertReadShort(ptr->fitfp,&tmp);
  ptr->inx_recl=tmp;
  
  tmpbuf=malloc(ptr->fit_recl);
  if (tmpbuf !=NULL) { /* get the header information */
     memset(tmpbuf,0,ptr->fit_recl);
     status=(read(ptr->fitfp,tmpbuf,ptr->fit_recl) !=ptr->fit_recl);
     /* now decode the header information */
     if (status==0) {
        for (i=0;(tmpbuf[i] !='\n') && (tmpbuf[i] !=0) && (i<80);i++) 
          ptr->header[i]=tmpbuf[i];
        ptr->header[i]=0;
        j=i+1;
        for (i=0;(tmpbuf[j+i] !='\n') && (tmpbuf[j+i] !=0) && (i<32);i++) 
          ptr->date[i]=tmpbuf[j+i];
        ptr->date[i]=0;
        j+=i+1;
        for (i=0;(tmpbuf[j+i] !=0) && (i<256);i++) 
          ptr->extra[i]=tmpbuf[j+i];
        ptr->extra[i]=0;
         
        /* okay extra the version information - look for the '.' */
        for (i=0;(ptr->header[i] !='.') && (ptr->header[i] !=0);i++);
        if (ptr->header[i] !=0) { 
           tmpbuf[i]=0; /* work backwards for the major number */
           for (j=0;(ptr->header[i-1-j] !=' ') && ((i-1-j)>0);j++)
             tmpbuf[i-1-j]=ptr->header[i-1-j];
           ptr->major_rev=atoi(tmpbuf+i-j);
           for (j=0;(ptr->header[i+1+j] !=' ') && 
               (ptr->header[i+1+j] !=0);i++) tmpbuf[j]=ptr->header[i+1+j]; 
           tmpbuf[j]=0;
           ptr->minor_rev=atoi(tmpbuf);
           
        }
     }
     free(tmpbuf);
  }
    
  lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);

  /* read the first parameter block se we can determine the year */
  status=(read(ptr->fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out));
  if (status==0) {
    ConvertBlock( (unsigned char *) &r,r1_pat);
    status=(r.r1.rrn !=0);
    prms=(struct radar_parms *) &(r.r1.plist[0]);
  }
  lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);

  if (status !=0) {
    if (ptr->fitfp !=-1) close(ptr->fitfp);
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    free(ptr);
    return NULL; 
  }

  ptr->ctime=TimeYMDHMSToEpoch(prms->YEAR,prms->MONTH,prms->DAY,
			prms->HOUR,prms->MINUT,prms->SEC);

  ptr->stime=ptr->ctime;  
  


  if (OldFitReadInx(ptr,inx_buf,-1)==0) {

    if (inx_buf[0] > inx_buf[1]) prms->YEAR++;
    /* generate the end time of the file */
 
    ptr->etime=TimeYMDHMSToEpoch(prms->YEAR,1,1,0,0,0)
                +inx_buf[1];   
    ptr->inx_srec=inx_buf[2]-2;
    ptr->inx_erec=inx_buf[3];     
  } else { /* corrupt index file */
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    ptr->inxfp=-1;
  } 
  ptr->fitread=OldFitReadCurrent;
  return ptr;        
}

struct OldFitFp *OldFitOpen(char *fitfile,char *inxfile) {

  int fitfp=-1,inxfp=-1;
  fitfp=open(fitfile,O_RDONLY);
  if (fitfp==-1) return NULL;
  if (inxfile !=NULL) inxfp=open(inxfile,O_RDONLY);
  return OldFitOpenFd(fitfp,inxfp);
}
