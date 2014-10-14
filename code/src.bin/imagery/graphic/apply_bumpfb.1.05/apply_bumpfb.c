/* apply_bumpfb.c
   ============== */

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
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"

#include "hlpstr.h"
#include "errstr.h"

/*
 $Log: apply_bumpfb.c,v $
 Revision 1.6  2005/08/11 14:12:15  barnes
 Added option to dump the command line options.

 Revision 1.5  2004/08/25 13:42:49  barnes
 Removed the ambient component (it is now including in the bump map).

 Revision 1.4  2004/06/22 17:50:46  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/06/21 22:07:37  barnes
 Added missing headers.

 Revision 1.2  2004/05/17 17:31:02  barnes
 Removed redundant code.

 Revision 1.1  2003/03/19 02:45:21  barnes
 Initial revision

*/


struct OptionData opt;

int32 bwdt,bhgt;
float *zbuf;
float *sbuf;
float *dbuf;

int load_bump(FILE *fp) {
  int pattern[4]={0,0,0,0};

  int s=0;
  s=ConvertFreadInt(fp,&bwdt);
  if (s !=0) return -1;
  s=ConvertFreadInt(fp,&bhgt);
  if (s !=0) return -1;
  zbuf=malloc(sizeof(float)*bwdt*bhgt);
  if (zbuf==NULL) s=-1;
  sbuf=malloc(sizeof(float)*bwdt*bhgt);
  if (sbuf==NULL) s=-1;
  dbuf=malloc(sizeof(float)*bwdt*bhgt);
  if (dbuf==NULL) s=-1;
 
  if (s==-1) {
    if (zbuf !=NULL) free(zbuf);
    if (sbuf !=NULL) free(sbuf);
    if (dbuf !=NULL) free(dbuf);
    return -1;
  }
  
  if (fread(zbuf,4,bwdt*bhgt,fp) !=bwdt*bhgt) s=-1;
  if ((s==0) && (fread(dbuf,4,bwdt*bhgt,fp) !=bwdt*bhgt)) s=-1;
  if ((s==0) && (fread(sbuf,4,bwdt*bhgt,fp) !=bwdt*bhgt)) s=-1;
  if (s==-1) {
    if (zbuf !=NULL) free(zbuf);
    if (sbuf !=NULL) free(sbuf);
    if (dbuf !=NULL) free(dbuf);
    return -1;
  }
  
  pattern[0]=4;
  pattern[1]=bwdt*bhgt;
  
  ConvertBlock( (unsigned char *) zbuf,pattern);
  ConvertBlock( (unsigned char *) dbuf,pattern);
  ConvertBlock( (unsigned char *) sbuf,pattern);
  
  return 0;
}


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
  
  int s;
  int arg=0;
  FILE *fp;
  struct FrameBuffer *img=NULL;
 
  unsigned char *r;
  unsigned char *g;  
  unsigned char *b;
  unsigned char *a;

  int x,y,tx,ty;

  int wdt=-1;
  int hgt=-1;

  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;

  unsigned char ippmflg=0;
  unsigned char ippmxflg=0;
  unsigned char ixmlflg=0;

  unsigned char help=0;
  unsigned char option=0;


  unsigned char mask=0;
  int level=0;
 
  int rv,gv,bv;
  float J,K; 


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"ippm",'x',&ippmflg);
  OptionAdd(&opt,"ippmx",'x',&ippmxflg);
  OptionAdd(&opt,"ixml",'x',&ixmlflg);

  OptionAdd(&opt,"mask",'x',&mask);
  OptionAdd(&opt,"level",'i',&level);


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

  if (ippmflg) img=FrameBufferLoadPPM(fp,"image");
  else if (ippmxflg) img=FrameBufferLoadPPMX(fp);
  else img=loadxml(fp);
  fclose(fp);

  if (img==NULL) {
    fprintf(stderr,"Error loading image.\n");
    exit(-1);
  }

  fp=fopen(argv[arg+1],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  s=load_bump(fp);
  
  fclose(fp);
  if (s !=0) {
    fprintf(stderr,"Error loading bump map.\n");
    exit(-1);
  }

  wdt=img->wdt;
  hgt=img->hgt;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  r=img->img;
  g=img->img+img->wdt*img->hgt;
  b=img->img+img->wdt*img->hgt*2;
  a=img->msk;  

  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) { 
      tx=x*bwdt/wdt;
      ty=y*bhgt/hgt;
      if (mask) {
        if (zbuf[ty*bwdt+tx]<level) a[y*wdt+x]=0;
      } else {
        if (zbuf[ty*bwdt+tx]<level) continue;
        J=dbuf[ty*bwdt+tx];
        K=sbuf[ty*bwdt+tx];
        
        rv=r[y*wdt+x]*J+255*K;
        gv=g[y*wdt+x]*J+255*K;
        bv=b[y*wdt+x]*J+255*K;
        if (rv>255) rv=255;
        if (gv>255) gv=255;
        if (bv>255) bv=255;
        if (rv<0) rv=0;
        if (gv<0) gv=0;
        if (bv<0) bv=0;
        r[y*wdt+x]=rv;
        g[y*wdt+x]=gv;
        b[y*wdt+x]=bv;
      }
    }
  }

  if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0;
}




