/* padppm.c
   ======== */

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
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"


/*
 $Log: padppm.c,v $
 Revision 1.3  2005/08/05 20:09:29  barnes
  Added command line option to dump the possible command line options.

 Revision 1.2  2004/08/09 21:20:15  barnes
 Version control.

 Revision 1.1  2004/04/27 13:53:47  barnes
 Initial revision
 
*/


struct OptionData opt;

int main(int argc,char *argv[]) {
  
  
  int arg=0;
  FILE *fp;
  unsigned int bgcolor=0;
  char *bgtxt=NULL; 
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;
 
 

  int wdt=-1;
  int hgt=-1;
  int xpad=0;
  int ypad=0;

  unsigned char help=0;
  unsigned char option=0;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);
  OptionAdd(&opt,"xpad",'i',&xpad);
  OptionAdd(&opt,"ypad",'i',&ypad);

  OptionAdd(&opt,"bgcol",'t',&bgtxt);


  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  bgcolor=FrameBufferColor(0x00,0x00,0x00,0xff);

  if (bgtxt !=NULL) {
     sscanf(bgtxt,"%x",&bgcolor);
     bgcolor=bgcolor | 0xff000000;
  }

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  src=FrameBufferLoadPPM(fp,NULL);

  if (src==NULL) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp);

  if (wdt==-1) wdt=src->wdt;
  if (hgt==-1) hgt=src->hgt;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
  FrameBufferClear(dst,bgcolor,0x0f);  
  FrameBufferImage(dst,NULL,src,0x0f,xpad,ypad,0,NULL);  
  FrameBufferSavePPM(dst,stdout); 

  return 0;
}




