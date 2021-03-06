/* rawopen.c
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
#include "rawdata.h"
#include "lmt.h"
#include "raw.h"
#include "oldrawread.h"
#include "rawversions.h"

/*
 $Log: rawopen.c,v $
 Revision 1.6  2006/09/15 19:00:43  code
 Fixed signs.

 Revision 1.5  2004/08/12 01:59:24  barnes
 Set the initial start time to be the time in the first record.

 Revision 1.4  2004/08/04 13:23:46  barnes
 Added function to create file pointer for open files.

 Revision 1.3  2004/06/22 17:47:16  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 18:34:31  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/01/07 21:18:23  barnes
 Initial revision

*/

struct rawrechdr { /* this is the header for the output record */
    unsigned char size[2];
    unsigned char number[4];
    char stamp[8];
};


struct OldRawFp *OldRawOpenFd(int rawfd,int inxfd) {

  int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};

  unsigned char *inbuf=NULL;
  int16 num_byte;
  int32 rec_num;
  int stat;
  struct radar_parms *prm;
  struct OldRawFp *ptr=NULL;
  int status=0;
  int j;

  inbuf=malloc(sizeof(struct rawdata));
  if (inbuf==NULL) return NULL;

  ptr=malloc(sizeof(struct OldRawFp));
  
  if (ptr==NULL) return NULL;

  ptr->rawfp=rawfd;
  ptr->stime=-1;
  ptr->ctime=-1;
  ptr->frec=0;
  ptr->rlen=0;
  ptr->ptr=0;

  fstat(ptr->rawfp,&ptr->rstat);

  if (ConvertReadShort(ptr->rawfp,&num_byte) !=0) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->rawfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }
  ConvertToInt(inbuf,&rec_num);

  ptr->frec=num_byte+2;
  ptr->rlen=num_byte+2;
  ptr->ptr=num_byte+2;

  if (rec_num==0) { /* decode header to get the threshold */
    char tmp[256];
    char *str;
    strcpy(tmp,(char *) (inbuf+sizeof(struct rawrechdr)-sizeof(int16)+1));
    ptr->thr=3;
    str=strtok(tmp," ");
    if (str==NULL) status=-1;
    if ((status==0) && (strcmp(str,"version") !=0)) status=-1;
    if (status==0) str=strtok(NULL," ");
    if (str==NULL) status=-1;
    for (j=0;str[j] !=0;j++) if (str[j]=='.') break;
    if (str[j]==0) {
      ptr->major_rev=atoi(str);
    } else {
      str[j]=0;
      ptr->major_rev=atoi(str);
      ptr->minor_rev=atoi(str+j+1);
    }
    if (status==0) str=strtok(NULL," ");
    if (str==NULL) status=-1;
    if ((status==0) && (strcmp(str,"threshold") !=0)) status=-1;
    if (status==0) str=strtok(NULL," ");
    if (str==NULL) status=-1;
    if (status==0) ptr->thr=atoi(str);
  }

  if (rec_num !=0) { /* not the header so rewind the file */
    lseek(ptr->rawfp,0L,SEEK_SET);
    ptr->rlen=0;
  }

  /* read the first record so that we can determine the start time of 
     the file */

  
 if (ConvertReadShort(ptr->rawfp,&num_byte) !=0) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->rawfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  ConvertToInt(inbuf,&rec_num);

  /* now decode the parameter block */

  ConvertBlock(inbuf+12,radar_parms_pat);
  prm=(struct radar_parms *) (inbuf+12);   

  ptr->stime=TimeYMDHMSToEpoch(prm->YEAR,prm->MONTH,prm->DAY,
	  prm->HOUR,prm->MINUT,prm->SEC);

  ptr->ctime=ptr->stime;

  /* rewind to the first record */

  lseek(ptr->rawfp,ptr->frec,SEEK_SET);

  ptr->rawread=OldRawReadCurrent;

  free(inbuf);
  return ptr;
}

struct OldRawFp *OldRawOpen(char *rawfile,char *inxfile) {
  int rawfd=0;
  int inxfd=0;

  rawfd=open(rawfile,O_RDONLY);
  if (rawfd==-1) return NULL;
  
  return OldRawOpenFd(rawfd,inxfd);
}
