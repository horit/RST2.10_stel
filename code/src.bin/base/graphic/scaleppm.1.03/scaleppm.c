/* scaleppm.c
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
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"


/*
 $Log: scaleppm.c,v $
 Revision 1.4  2006/11/13 19:08:38  code
 Added the ability to set a scale factor and a background color.

 Revision 1.3  2005/08/05 20:25:05  barnes
 Added option to dump the command line options

 Revision 1.2  2004/04/27 14:01:34  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/06 22:01:37  barnes
 Initial revision
 
*/


struct OptionData opt;


unsigned int ColorStringRGB(char *txt) {
  unsigned int colval;
  if (txt==NULL) return 0;
  sscanf(txt,"%x",&colval);
  return colval | 0xff000000;
}

int main(int argc,char *argv[]) {
  
  int arg=0;
  FILE *fp;
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;
  struct FrameBufferMatrix m;
 
  unsigned char sflg=0;
  float wdt=-1;
  float hgt=-1;
  float sf=1.0;

  char *bgcol_txt=NULL;
  int bgcol=0xffffff;

  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"smooth",'x',&sflg);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  OptionAdd(&opt,"sf",'f',&sf);
  OptionAdd(&opt,"bgcol",'t',&bgcol_txt);
 
  
  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (bgcol_txt !=NULL) bgcol=ColorStringRGB(bgcol_txt);

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

  if (wdt==-1) wdt=src->wdt*sf;
  if (hgt==-1) hgt=src->hgt*sf;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  dst=FrameBufferMake(NULL,wdt,hgt,src->depth);
 
  FrameBufferClear(dst,bgcol,0x0f);


  if ((wdt==src->wdt) && (hgt==src->hgt))
    FrameBufferImage(dst,NULL,src,0x0f,0,0,0,NULL);  
  else {
    float xsc=wdt/src->wdt;
    float ysc=hgt/src->hgt;
    m.a=xsc;
    m.b=0;
    m.c=0;
    m.d=ysc;
    FrameBufferImage(dst,&m,src,0x0f,0,0,sflg,NULL); 
  }
 
  FrameBufferSavePPM(dst,stdout); 

  return 0;
}




