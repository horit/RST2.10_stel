/* mergefb.c
   ========= */

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
#include "errstr.h"

/*
 $Log: mergefb.c,v $
 Revision 1.4  2005/08/11 14:42:39  barnes
 Added option to dump command line options.

 Revision 1.3  2004/06/21 22:21:22  barnes
 Added missing header.

 Revision 1.2  2004/05/17 21:11:48  barnes
 Added weighting factors to control transparency on PPM files.

 Revision 1.1  2003/04/21 14:22:07  barnes
 Initial revision

*/


struct OptionData opt;

struct FrameBuffer *loadxml(FILE *fp) {
  struct FrameBuffer *img=NULL;
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;

  char lbuf[256];
 
  int s=0;

  xmldata=XMLMake();
  xmlimg.xml=xmldata;
  xmlimg.img=&img;

  XMLSetStart(xmldata,FrameBufferXMLStart,&xmlimg);
  XMLSetEnd(xmldata,FrameBufferXMLEnd,&xmlimg);

  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }
  XMLFree(xmldata);
  return img;
}

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {
  

  int arg=0;
  FILE *fp;
 
  struct FrameBuffer *bg=NULL;
  struct FrameBuffer *bgm=NULL;
  struct FrameBuffer *fg=NULL;
  struct FrameBuffer *dst=NULL;
  struct FrameBuffer *msk=NULL;
  struct FrameBufferMatrix m;
 
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;
  unsigned char *a;

  unsigned char *rf;
  unsigned char *gf;  
  unsigned char *bf;
  unsigned char *af;

  int rv,gv,bv;
  int x,y,tx,ty;

  unsigned char addflg=0;
  float tf=1.0;
  float fd=1.0;
  float ratio,alpha;
  float c;


  unsigned char sflg=0;
  int wdt=-1;
  int hgt=-1;

  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char bppmflg=0;
  unsigned char bppmxflg=0;
  unsigned char bxmlflg=0;

  unsigned char fppmflg=0;
  unsigned char fppmxflg=0;
  unsigned char fxmlflg=0;

  unsigned char help=0;
  unsigned char option=0;
 
  OptionAdd(&opt,"-help",'x',&help); 
  OptionAdd(&opt,"-option",'x',&option);
  
  OptionAdd(&opt,"smooth",'x',&sflg);

  OptionAdd(&opt,"tf",'f',&tf);
  OptionAdd(&opt,"fd",'f',&fd);

  OptionAdd(&opt,"add",'x',&addflg);
 
  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"bppm",'x',&bppmflg);
  OptionAdd(&opt,"bppmx",'x',&bppmxflg);
  OptionAdd(&opt,"bxml",'x',&bxmlflg);

  OptionAdd(&opt,"fppm",'x',&fppmflg);
  OptionAdd(&opt,"fppmx",'x',&fppmxflg);
  OptionAdd(&opt,"fxml",'x',&fxmlflg);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }


  fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (bppmflg) bg=FrameBufferLoadPPM(fp,"image");
  else if (bppmxflg) bg=FrameBufferLoadPPMX(fp);
  else bg=loadxml(fp);
  fclose(fp);

 fp=fopen(argv[arg+1],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (fppmflg) fg=FrameBufferLoadPPM(fp,"image");
  else if (fppmxflg) fg=FrameBufferLoadPPMX(fp);
  else fg=loadxml(fp);
  fclose(fp);

  if ((bg==NULL) || (fg==NULL)) {
    fprintf(stderr,"Error loading images.\n");
    exit(-1);
  }

  wdt=bg->wdt;
  hgt=bg->hgt;

  if (fg->wdt>wdt) wdt=fg->wdt;
  if (fg->hgt>hgt) hgt=fg->hgt;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  bgm=FrameBufferMake(NULL,bg->wdt,bg->hgt,8);
  memcpy(bgm->img,bg->msk,bg->wdt*bg->hgt);

  dst=FrameBufferMake(fg->name,wdt,hgt,24);
  msk=FrameBufferMake(fg->name,wdt,hgt,8); 

  if ((wdt==bg->wdt) && (hgt==bg->hgt)) {
    FrameBufferImage(dst,NULL,bg,0x0f,0,0,0,NULL); 
    FrameBufferImage(msk,NULL,bgm,0x0f,0,0,0,NULL);
  } else {
    float xsc=wdt/bg->wdt;
    float ysc=hgt/bg->hgt;
    m.a=xsc;
    m.b=0;
    m.c=0;
    m.d=ysc;
    FrameBufferImage(dst,&m,bg,0x0f,0,0,sflg,NULL); 
    FrameBufferImage(msk,&m,bgm,0x0f,0,0,sflg,NULL);
  }

  memcpy(dst->msk,bgm->img,wdt*hgt);


  r=dst->img;
  g=dst->img+dst->wdt*dst->hgt;
  b=dst->img+dst->wdt*dst->hgt*2;
  a=dst->msk;
 

  rf=fg->img;
  gf=fg->img+fg->wdt*fg->hgt;
  bf=fg->img+fg->wdt*fg->hgt*2;
  af=fg->msk;

  
  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) { 
      tx=x*fg->wdt/wdt;
      ty=y*fg->hgt/hgt;
     
      if (fppmflg) 
        c=fd*(0.3*rf[ty*fg->wdt+tx]+0.4*bf[ty*fg->wdt+tx]+
              0.3*bf[ty*fg->wdt+tx])/255.0;
      else    
        c=fd*af[y*fg->wdt+x];

      rv=r[y*wdt+x];
      gv=g[y*wdt+x];
      bv=b[y*wdt+x];
      alpha=a[y*wdt+x]/255.0;

      if (addflg) {
        rv=rv*(1-c*tf)+tf*fd*rf[ty*fg->wdt+x];
        gv=gv*(1-c*tf)+tf*fd*gf[ty*fg->wdt+x];
        bv=bv*(1-c*tf)+tf*fd*bf[ty*fg->wdt+x];
        alpha=alpha*(1-c*tf)+tf*c;

      } else {
        ratio=(1-tf)+c*tf;
        rv=rv*(1-ratio)+ratio*fd*rf[ty*fg->wdt+x];
        gv=gv*(1-ratio)+ratio*fd*gf[ty*fg->wdt+x];
        bv=bv*(1-ratio)+ratio*fd*bf[ty*fg->wdt+x];
        alpha=1-(1-ratio)*(1-alpha);
      }    

      if (alpha<0) alpha=0;
      if (alpha>1) alpha=1;

      if (rv<0) rv=0;
      if (gv<0) gv=0;
      if (bv<0) bv=0;
      if (rv>255) rv=255;
      if (gv>255) gv=255;
      if (bv>255) bv=255;
 
      r[y*wdt+x]=rv;
      g[y*wdt+x]=gv;
      b[y*wdt+x]=bv;
      a[y*wdt+x]=255.0*alpha;

    }
  } 
  if (xmlflg==1) FrameBufferSaveXML(dst,stream,stdout);
  else if (ppmflg==1) FrameBufferSavePPM(dst,stdout);
  else FrameBufferSavePPMX(dst,stdout);
  return 0;
}




