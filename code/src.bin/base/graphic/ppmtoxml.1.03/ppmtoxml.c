/* ppmtoxml.c
   ========== */

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
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"


/*
 $Log: ppmtoxml.c,v $
 Revision 1.5  2005/08/05 20:25:05  barnes
 Added option to dump the command line options

 Revision 1.4  2004/06/21 21:14:10  barnes
 Added missing header.

 Revision 1.3  2004/04/27 13:55:00  barnes
 Audit - Enforced warning set to "all".

 Revision 1.2  2004/04/26 18:50:39  barnes
 Fixed bug in testing for percentange and removed redundant test for negative alpha.

 Revision 1.1  2003/03/06 22:41:03  barnes
 Initial revision

*/


struct OptionData opt;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}


int main(int argc,char *argv[]) {
  unsigned int color=0;
  unsigned int  alpha=255;
  int arg=0;
  FILE *fp;
  char *name=NULL;
  char *atxt=NULL;
  struct FrameBuffer *img=NULL;
  

  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"name",'t',&name);
  OptionAdd(&opt,"alpha",'t',&atxt);

  
  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  img=FrameBufferLoadPPM(fp,name);
 
  if (img==NULL) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp);

  if (atxt !=NULL) {
    int alen;
    alen=strlen(atxt);

    if ((alen>2) && (atxt[0]=='0') && (atxt[1]=='x'))
       sscanf(atxt+2,"%x",&alpha);
    else if (atxt[alen-1]=='%') {
      float aflt;
      atxt[alen-1]=0;
      sscanf(atxt,"%g",&aflt);
      if (aflt<0) aflt=0;
      if (aflt>100) aflt=100;
      alpha=255*aflt/100.0;
    } else alpha=atoi(atxt);
    if (alpha>255) alpha=255;
  }


  color=FrameBufferColor(0,0,0,alpha);
  FrameBufferClear(img,color,0x08);

  if (img !=NULL) FrameBufferSaveXML(img,stream,stdout);
  return 0;
}




