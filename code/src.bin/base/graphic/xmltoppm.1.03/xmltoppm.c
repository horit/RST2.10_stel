/* xmltoppm.c
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
 $Log: xmltoppm.c,v $
 Revision 1.4  2005/08/05 20:25:05  barnes
 Added option to dump the command line options

 Revision 1.3  2004/06/21 21:37:13  barnes
 Added missing header.

 Revision 1.2  2004/04/27 14:03:53  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/18 18:46:29  barnes
 Initial revision

*/


struct OptionData opt;

int main(int argc,char *argv[]) {
  char lbuf[256];
  int s=0;
  int arg=0;
  FILE *fp;
  struct FrameBuffer *src=NULL;
  struct FrameBuffer *dst=NULL;

  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;

  unsigned int bgcolor=0;
  char *bgtxt=NULL;

  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

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

  xmldata=XMLMake();
  xmlimg.xml=xmldata;
  xmlimg.img=&src;

  XMLSetStart(xmldata,FrameBufferXMLStart,&xmlimg);
  XMLSetEnd(xmldata,FrameBufferXMLEnd,&xmlimg);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }
  if (s !=0) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp);

  bgcolor=FrameBufferColor(0x00,0x00,0x00,0xff);


  if (bgtxt !=NULL) {
     sscanf(bgtxt,"%x",&bgcolor);
     bgcolor=bgcolor | 0xff000000;
  }

  if (src==NULL) {
    fprintf(stderr,"Error decoding image.\n");
    exit(-1);
  }
  
  dst=FrameBufferMake(NULL,src->wdt,src->hgt,24);
  if (dst==NULL) {
    fprintf(stderr,"Error creating image.\n");
    exit(-1);
  }
  
  FrameBufferClear(dst,bgcolor,0x0f);
  FrameBufferImage(dst,NULL,src,0x0f,0,0,0,NULL);  

  if (dst !=NULL) FrameBufferSavePPM(dst,stdout);
  return 0;
}





