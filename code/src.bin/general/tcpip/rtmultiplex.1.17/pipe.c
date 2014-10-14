/* pipe.c
   ======
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
#include <sys/time.h>
#include <unistd.h>
#ifdef _QNX
#include <sys/select.h>
#endif

#include "connex.h"


/*
 $Log: pipe.c,v $
 Revision 1.4  2007/02/06 21:42:43  code
 Fixed types for 64-bit systems.

 Revision 1.3  2004/04/29 20:25:06  barnes
 Added missing header.

 Revision 1.2  2004/04/28 18:57:59  barnes
 Added missing header.

 Revision 1.1  2004/04/27 18:02:29  barnes
 Initial revision

*/


int pipewrite(int fildes,char st,
	      unsigned char *buffer,unsigned int size) {
  int byte;
  int cnt=0;
  int hsze=0;

  char hdr[sizeof(int)+1]; 

  hdr[0]=st;
  *((int *)(hdr+1))=size;
 
  hsze=sizeof(int)+1;
  while (cnt<hsze) { 
    byte=write(fildes,hdr+cnt,hsze-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<hsze) return -1;

  cnt=0;  /* write out the message */
  while (cnt<size) {
    byte=write(fildes,buffer+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<size) return -1;
  return size;
}  


int piperead(int fildes,unsigned char **buffer,unsigned char *st) {

  int byte;
  int cnt=0;

  int hsze=sizeof(int)+1;
  char hdr[sizeof(int)+1];

  unsigned int size;   

  while (cnt<hsze) { 
    byte=read(fildes,hdr,hsze);
    if (byte<=0) {
       ConnexErr=MSGERR_PIPE_CLOSED;
       break;
    }
    cnt+=byte;
  }
  if (cnt<hsze) return -1;
 
  *st=hdr[0];
  size=*((int *) (hdr+1));


  if (size==0) {
    if (*buffer !=NULL) free(*buffer);
    *buffer=NULL;
    return 0;
  }

  if (*buffer !=NULL) {  
    unsigned char *tmp;  
    if ((tmp=realloc(*buffer,size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
      return -1;
    }
    *buffer=tmp;
  } else {
    if ((*buffer=malloc(size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
     return -1;
    }
  }

 
  cnt=0;  /* read in the message */
  while (cnt<size) {
    byte=read(fildes,(*buffer)+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
 
  if (cnt<size) {
     free(*buffer);
     *buffer=NULL;
     return -1;
  }
 
  return size;
}  
