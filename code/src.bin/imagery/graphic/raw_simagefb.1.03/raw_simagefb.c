/* raw_simagefb
   ============ 
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
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"

#include "rfbuffer.h"
#include "rtime.h"

#include "simage.h"

#include "key.h"

#include "hlpstr.h"
#include "version.h"

/*
 $Log: raw_simagefb.c,v $
 Revision 1.6  2005/08/11 14:46:02  barnes
 Added option to dump command line options.

 Revision 1.5  2004/06/21 22:23:37  barnes
 Added missing header.

 Revision 1.4  2004/05/05 16:32:29  barnes
 Added missing header.

 Revision 1.3  2004/05/04 20:08:42  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/27 20:33:05  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/03 18:48:26  barnes
 Initial revision

*/

unsigned char gry[256];

struct key key;

struct OptionData opt;
struct OptionFile *optf=NULL;

struct SImage simage;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int xmldecode(char *buf,int sze,void *data) {
  struct XMLdata *xmldata;
  xmldata=(struct XMLdata *) data;
  return XMLDecode(xmldata,buf,sze);
} 

int main(int argc,char *argv[]) {
  int s=0;

  struct FrameBuffer *simg=NULL;
  struct FrameBuffer *img=NULL;
 
  struct FrameBuffer *smsk=NULL;
  struct FrameBuffer *msk=NULL;
 
  int arg;

  char *cfname=NULL;
  FILE *fp;

  float wdt=0,hgt=0;



  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char help=0; 
  unsigned char option=0; 

  char *key_fname=NULL;

  int inx=0;
  int num=0;
  float min=1e6,max=-1e6;
  unsigned char sflg=0;
  unsigned char lflg=0;
  unsigned char glflg=0; 

  unsigned char smflg=0;

  int x,y,i;
  float c;
  int ci;
  unsigned char *r,*g,*b,*a;

  for (i=0;i<256;i++) gry[i]=i;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"wdt",'f',&wdt);
  OptionAdd(&opt,"hgt",'f',&hgt);

  OptionAdd(&opt,"gl",'x',&glflg);
  OptionAdd(&opt,"log",'x',&lflg);
  OptionAdd(&opt,"stretch",'x',&sflg);

  OptionAdd(&opt,"min",'f',&min);
  OptionAdd(&opt,"max",'f',&max);

  OptionAdd(&opt,"smooth",'x',&smflg);

  OptionAdd(&opt,"inx",'i',&inx);
  OptionAdd(&opt,"key",'t',&key_fname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  if (cfname !=NULL) { /* load the configuration file */
    int farg;
    do {
      fp=fopen(cfname,"r");
      if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        farg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
        OptionFreeFile(optf);
       }   
       fclose(fp);
    } while (cfname !=NULL);
  }

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
       fprintf(stderr,"File not found.\n");
       exit(-1);
    }
  } else fp=stdin;


  s=SImageFread(fp,&simage);

  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
  if (arg<argc) fclose(fp); 
 
  num=simage.colors;

  if (inx<0) inx=0;
  if (inx>=num) inx=num-1;
 
  if ((wdt==0) || (hgt==0)) {
    wdt=simage.wdt;
    hgt=simage.hgt;
  }

  if (wdt>1000) wdt=1000;
  if (hgt>1000) hgt=1000;

  if ((min==1e6) && (simage.min !=NULL)) min=simage.min[inx];
  if ((max==-1e6)&& (simage.max !=NULL)) max=simage.max[inx];
  if ((lflg) && (min<=0)) min=1;

  if (lflg) min=log10(min);
  if (lflg) max=log10(max);



  if (key_fname !=NULL) {
    fp=fopen(key_fname,"r");
    if (fp !=NULL) {
      load_key(fp,&key);
      fclose(fp);
    }
  }


  if (key.max==0) {
    key.max=255;
    key.r=gry;
    key.g=gry;
    key.b=gry;
  }

  if ((ppmflg==0) && (xmlflg==0) && (ppmxflg==0)) xmlflg=1;

  img=FrameBufferMake(NULL,wdt,hgt,24);
  msk=FrameBufferMake(NULL,wdt,hgt,8);
  FrameBufferClear(img,0,0x0f);
  FrameBufferClear(img,0,0x0f);


  simg=FrameBufferMake(NULL,simage.wdt,simage.hgt,24);
  smsk=FrameBufferMake(NULL,simage.wdt,simage.hgt,24);
  FrameBufferClear(simg,0xff000000,0x0f);
  FrameBufferClear(smsk,0xff000000,0x0f);


  r=simg->img;
  g=simg->img+simage.wdt*simage.hgt;
  b=simg->img+simage.wdt*simage.hgt*2;
  a=smsk->img;

  for (i=0;i<simage.numpnt;i++) {
    x=simage.pnt[i].x;
    y=simage.pnt[i].y;
    c=0;
   

    if ((glflg) && (simage.pnt[i].flg)) continue;
  
    if ((lflg) && (simage.data[i*num+inx]>0)) c=log10(simage.data[i*num+inx]);
    else c=simage.data[i*num+inx];
 
    if (c<min) continue;
    if (c>max) c=max;
    if (sflg) c=(c-min)/(max-min);
    else c=c/max;
     
    if (c<0.0) c=0.0;
    if (c>1.0) c=1.0;
    
    ci=(key.max-1)*c;
    
    if (key.r !=NULL) r[y*simage.wdt+x]=key.r[ci];
    if (key.g !=NULL) g[y*simage.wdt+x]=key.g[ci];
    if (key.b !=NULL) b[y*simage.wdt+x]=key.b[ci];
    a[y*simage.wdt+x]=ci;
  }  

 
 
  if ((wdt !=simg->wdt) || (hgt !=simg->hgt)) {
    struct FrameBufferMatrix m;
    m.a=wdt/simg->wdt;
    m.b=0;
    m.c=0;
    m.d=hgt/simg->hgt;
    FrameBufferImage(img,&m,simg,0x0f,0,0,smflg,NULL); 
    FrameBufferImage(msk,&m,smsk,0x0f,0,0,smflg,NULL);  
  } else {
    FrameBufferImage(img,NULL,simg,0x0f,0,0,smflg,NULL);
    FrameBufferImage(msk,NULL,smsk,0x0f,0,0,smflg,NULL);
  }
  memcpy(img->msk,msk->img,wdt*hgt);
  if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0; 
}  


