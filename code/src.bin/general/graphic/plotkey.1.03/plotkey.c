/* plotkey.c
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
#include <sys/types.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"

#include "rfbuffer.h"
#include "hlpstr.h"
#include "errstr.h"

/*
 $Log: plotkey.c,v $
 Revision 1.6  2005/08/08 15:08:04  barnes
 Added option to dump the command line options

 Revision 1.5  2004/06/21 21:55:11  barnes
 Added missing header.

 Revision 1.4  2004/05/05 16:06:15  barnes
 Added missing header.

 Revision 1.3  2004/05/04 19:11:45  barnes
  Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 17:36:37  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/05/01 15:34:48  barnes
 Initial revision

*/


struct key {
  int max;
  unsigned char *r;
  unsigned char *g;
  unsigned char *b;
};

struct key key;



int farg;
struct OptionData opt;


int load_key(FILE *fp,struct key *key) {

  int i,j=0;
  unsigned int colval;
  int num=0;
  char line[1024];
  char *tkn;
  while (fgets(line,1024,fp) !=0) {
  
    for (i=0; (line[i] !=0) && 
              ((line[i] ==' ') || (line[i]=='\n'));i++);

   
    if ((line[i]==0) || (line[i]=='#')) continue;
    tkn=line+i;
    if (num==0) {
      sscanf(tkn,"%d",&num);
      if (num !=0) {
        key->r=malloc(num);
        key->g=malloc(num);  
        key->b=malloc(num);
        memset(key->r,0,num);
        memset(key->g,0,num);
        memset(key->b,0,num);
        key->max=num;
        j=0;
      }
    } else if (j<num) {
      sscanf(tkn,"%x",&colval);
      key->r[j]=(colval>>16) & 0xff;
      key->g[j]=(colval>>8) & 0xff;
      key->b[j]=colval & 0xff;
      j++;
    }
  }
  return num;
}

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {
 
  
  int i;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vertical=0;
  FILE *fp;
  struct FrameBuffer *img=NULL;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vertical",'x',&vertical);

  farg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
      OptionDump(stdout,&opt);
      exit(0);
    }

  if (farg==argc) {
   OptionPrintInfo(stderr,errstr);
   exit(0);
  }


  fp=fopen(argv[farg],"r");
  if (fp !=NULL) {
      load_key(fp,&key);
      fclose(fp);
  } else exit(0);

  if (vertical) img=FrameBufferMake(NULL,1,key.max,24);
  else img=FrameBufferMake(NULL,key.max,1,24);
 

  for (i=0;i<key.max;i++) {
    img->img[i]=key.r[i];
    img->img[key.max+i]=key.g[i];
    img->img[2*key.max+i]=key.b[i];
  }
  

  FrameBufferSavePPM(img,stdout);
  return 0;
}  


