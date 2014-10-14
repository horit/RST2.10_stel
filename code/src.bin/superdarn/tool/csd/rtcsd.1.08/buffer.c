/* buffer.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"

/* 
 $Log: buffer.c,v $
 Revision 1.2  2004/06/22 18:37:35  barnes
 Replaced references of integers with "int".

 Revision 1.1  2004/05/10 21:12:12  barnes
 Initial revision

*/

#define TMPBUFMAX 32*1024
#define TMPBUFLEN 32*1024

#define HDR_SZE (3*sizeof(double)+sizeof(int16)+2*sizeof(int32)+8)


unsigned char *buffer=NULL;
unsigned int sze=0;
unsigned int ptr=0;


unsigned char tmp_buffer[TMPBUFMAX];
unsigned int tmp_ptr=0;

int makebuffer(int size) {
  if ((buffer=malloc(size))==NULL) return -1;
  sze=size;
  return 0;
}
  


void adddelta(int magic,unsigned char *ibuf,int size) {
 
  if ((ptr+size+8)>=sze) {
    int p=0;
    int32 bsze;
    while ((p< (size+8)) && (p<ptr)) {
      ConvertToInt(buffer+p+4,&bsze);
      p+=8+bsze;
    }
    if (p>=ptr) return;
    memmove(buffer,buffer+p,ptr-p);
    ptr-=p;
  }

  ConvertFromInt(magic,buffer+ptr);
  ConvertFromInt(size,buffer+ptr+4);
  memcpy(buffer+ptr+8,ibuf,size);
  ptr+=8+size;


  /* temporary buffer used for daily files */
  
  if ((tmp_ptr+size+8)>=TMPBUFLEN) {
    int p=0;
    int32 bsze;
    while ((p< (size+8)) && (p<tmp_ptr)) {
      ConvertToInt(tmp_buffer+p+4,&bsze);
      p+=8+bsze;
    }
    if (p>=tmp_ptr) return;
    memmove(tmp_buffer,tmp_buffer+p,tmp_ptr-p);
    tmp_ptr-=p;
  }

  ConvertFromInt(magic,tmp_buffer+tmp_ptr);
  ConvertFromInt(size,tmp_buffer+tmp_ptr+4);
  memcpy(tmp_buffer+tmp_ptr+8,ibuf,size);
  tmp_ptr+=8+size;

}

int loaddelta(char *fname) {
  FILE *fp;
  int fsze;
  ptr=0;
  fp=fopen(fname,"r");
  if (fp==NULL) return -1;

  /* skip the header */  
 
  fseek(fp,0L,SEEK_END);
  fsze=ftell(fp);
  fseek(fp,HDR_SZE,SEEK_SET);
  fsze-=HDR_SZE;

  if ((fsze >0) && (fread(buffer,fsze,1,fp) ==1)) ptr=fsze;
    else ptr=0;
  fclose(fp);
  return 0;
}     


int writedelta(int fid,double stime) {
  int p=0,s;
  int32 bsze;
  int32 magic;
  double time=0;
 
  /* scan through the buffer to find a start time later than that requested */

  while (p<ptr) {
     ConvertToInt(buffer+p,&magic);
     ConvertToInt(buffer+p+4,&bsze);
     if (magic !='d') {
       p+=8+bsze;
       continue;
     }
     ConvertToDouble(buffer+p+8,&time);
    
  
     if (time>stime) break;
     p+=8+bsze;

  }

  if (p>=ptr) return 0;
  s=write(fid,buffer+p,ptr-p);

  /* okay anything we skipped we don't need */
  
  if (p>0) {
    memmove(buffer,buffer+p,ptr-p);
    ptr-=p;
  }

  return s;
}

int writetmpdelta(int fid,double stime) {
  int s;
  int p=0;
  int32 bsze;
  int32 magic;
  double time;
 
  /* scan through the buffer and write out things before the specified time */
  while (p<tmp_ptr) {
     ConvertToInt(tmp_buffer+p,&magic);
     ConvertToInt(tmp_buffer+p+4,&bsze);
     if (magic !='d') {
       p+=8+bsze;
       continue;
     }

     ConvertToDouble(tmp_buffer+p+8,&time);
     if (time>stime) break;
     p+=8+bsze;

  }
  
  s=write(fid,tmp_buffer,p);
  if (s==-1) return -1;
  if (p>=tmp_ptr) {
    tmp_ptr=0;
    return 0;
  }

  memmove(tmp_buffer,tmp_buffer+p,tmp_ptr-p);
  tmp_ptr-=p;
  
  return 1; /* there is something left to write later than requested time */
 

  
}






