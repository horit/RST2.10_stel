/* xmltorplot.c
   ============ */

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
#include <time.h>
#include <string.h>
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"

#include "hlpstr.h"
#include "version.h"

/*
 $Log: xmltorplot.c,v $
 Revision 1.4  2005/08/05 20:25:05  barnes
 Added option to dump the command line options

 Revision 1.3  2004/06/21 21:39:38  barnes
 Added missing header.

 Revision 1.2  2004/04/27 14:06:04  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/14 19:59:20  barnes
 Initial revision

*/


struct OptionData opt;

void record_info(struct Rplot *rplot,int argc,char *argv[]) {
  int i;
  char *date;
  char buf[256];
  time_t ltime;
  struct tm *time_of_day;
  time(&ltime);
  time_of_day=gmtime(&ltime);
  date=asctime(time_of_day);
  date[strlen(date)-1]=0;
  sprintf(buf,"<created>%s</created>\n",date);
  RplotInfo(rplot,buf,strlen(buf));
  sprintf(buf,"<command>\n");
  RplotInfo(rplot,buf,strlen(buf));
  for (i=0;i<argc;i++) {
    if (i !=0) RplotInfo(rplot," ",1);
    RplotInfo(rplot,argv[i],strlen(argv[i]));
  }
  sprintf(buf,"\n</command>\n");
  RplotInfo(rplot,buf,strlen(buf));
  sprintf(buf,"<version><major>%d</major><minor>%d</minor></version>\n",
          MAJOR_VERSION,MINOR_VERSION);
  RplotInfo(rplot,buf,strlen(buf));
}

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}


int main(int argc,char *argv[]) {
  
  int s=0;
  int arg=0;
  FILE *fp;
  char lbuf[256];
  struct FrameBuffer *img=NULL;
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;
  struct RplotMatrix m;
 
  struct Rplot *ptr;

  unsigned char sflg=0;
  float wdt=-1;
  float hgt=-1;

  unsigned char help=0;
  unsigned char option=0;

 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"smooth",'x',&sflg);
  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);
  
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
  xmlimg.img=&img;

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

  if (img==NULL) {
    fprintf(stderr,"No image.\n");
    exit(-1);
  }

  if (wdt==-1) wdt=img->wdt;
  if (hgt==-1) hgt=img->hgt;
  
  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid output size.\n");
    exit(-1);
  }

  if ((img->wdt==0) || (img->hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  ptr=RplotMake();
  RplotSetText(ptr,stream,stdout);
  RplotMakePlot(ptr,img->name,wdt,hgt,24);
  RplotStartInfo(ptr);
  record_info(ptr,argc,argv);
  RplotEndInfo(ptr);


  if ((wdt==img->wdt) && (hgt==img->hgt))
    RplotImage(ptr,NULL,img,0x0f,0,0,0);  
  else {
    float xsc=wdt/img->wdt;
    float ysc=hgt/img->hgt;
    m.a=xsc;
    m.b=0;
    m.c=0;
    m.d=ysc;
    RplotImage(ptr,&m,img,0x0f,0,0,(int) sflg);
  }
 
  RplotEndPlot(ptr);

  return 0;
}




