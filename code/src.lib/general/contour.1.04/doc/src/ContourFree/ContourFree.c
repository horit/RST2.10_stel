/* ContourFree.c
   =============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rmath.h"
#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "fontdb.h"
#include "polygon.h"
#include "rplot.h"
#include "rplotgraw.h"
#include "contour.h"


struct FrameBuffer *img;
double val[32*32];

double cval[]={15,30,45,60,75};

struct PolygonData **ctr;

int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data) {
  
  struct FrameBufferFontDB *fontdb;
  struct FrameBufferFont *fnt;
  int tbox[3];

  fontdb=(struct FrameBufferFontDB *)data;
  if (fontdb==NULL) return -1;
  fnt=FrameBufferFontDBFind(fontdb,fntname,sze);

  FrameBufferTextBox(fnt,num,txt,tbox);
  
  box[0]=tbox[0];
  box[1]=tbox[1];
  box[2]=tbox[2];
  return 0;
}

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  struct FrameBufferFontDB *fontdb=NULL;
  struct Rplot *rplot=NULL;
  struct RplotMatrix m;

  unsigned int bgcol=0xffffff;
  unsigned int fgcol=0x000000;
  int i,j;
 

  int smth=2; /* trace all cells */
  int cflg=0; /* include open contours */
  int rflg=0; /* no wrapping */
  int bflg=0; /* trace along boundaries */

  char txt[16];
  float wdt=400,hgt=400;
  float width=0.5;
 
  char *envstr;
  FILE *fontfp;


  envstr=getenv("FONTDB");
  fontfp=fopen(envstr,"r");
  if (fontfp !=NULL) {
   fontdb=FrameBufferFontDBLoad(fontfp);
   fclose(fontfp);
  }
 
  if (fontdb==NULL) {
   fprintf(stderr,"Could not load fonts.\n");
   exit(-1);
  }


  img=FrameBufferMake("dummy",32,32,8);
  
  for (i=0;i<32;i++) {
    for (j=0;j<32;j++) {
      val[i*32+j]=50+50.0*cos(j*2.8*PI/64)*cos(i*3.5*PI/32);
      img->img[i*32+j]=255*val[i*32+j]/100;
      
    }
  }
  
  ctr=Contour(val,contour_DOUBLE,32,32,5,cval,smth,cflg,rflg,bflg);

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"ContourFree",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);
 
 
  m.a=wdt/32;
  m.b=0;
  m.c=0;
  m.d=hgt/32;
  RplotImage(rplot,&m,img,0x0f,0,0,1);  

  for (i=0;i<5;i++) {
    sprintf(txt,"%.2d",(int) cval[i]);
    PlotRawContour(rplot,ctr[i],0,0,wdt,hgt,0.1,0.5,
                30,i,5,strlen(txt),txt,fgcol,0x0f,width,
		NULL,"Helvetica",10.0,txtbox,fontdb);
  }              

  ContourFree(5,ctr);

  RplotEndPlot(rplot);
  return 0;
}