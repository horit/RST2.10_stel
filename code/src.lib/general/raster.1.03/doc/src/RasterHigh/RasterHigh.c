/* RasterHigh.c
   ============
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
#include "polygon.h"
#include "rplot.h"
#include "raster.h"

double value[]={2.0, 1.5, 1.0, 1.1, 1.2,
                1.5, 1.7, 1.0, 1.1, 1.1,
		1.0, 1.0, 1.0, 0.6, 0.8,
	        0.6, 1.0, 0.6, 0.1, 0.5,
		0.2, 1.0, 0.6, 0.5, 0.5};

double pnt[]={ 0,0,   100,0,   200,0,   300,0,   400,0,
            0,100,  100,100,  200,100,  300,100,  400,100,
	    0,200, 100,200, 200,200, 300,200, 400,200,
	    0,300, 100,300, 200,300, 300,300, 400,300,
	    0,400, 100,400, 200,400, 300,400, 400,400};
	    
	   
int vertex[]={0,1,6,5,     1,2,7,6,     2,3,8,7,   3,4,9,8,
              5,6,11,10,   6,7,12,11,   7,8,13,12, 8,9,14,13,
	      10,11,16,15, 11,12,17,16, 12,13,18,17, 13,14,19,17,
	      15,16,21,20, 16,17,22,21, 17,18,23,22, 18,19,24,23};

struct FrameBuffer *img;


int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  struct Rplot *rplot=NULL;
  int x,y;
  float wdt=400,hgt=400;
  double zeroval=1e31;
  double *zbuf=NULL;

  img=FrameBufferMake("dummy",wdt,hgt,8);

  zbuf=RasterHigh(wdt,hgt,0,raster_DOUBLE,&zeroval,16,pnt,vertex,value);

  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      img->img[y*(int) wdt+x]=255*zbuf[y*(int) wdt+x]/2.0;
    }
  }

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Raster",wdt,hgt,24);

  RplotImage(rplot,NULL,img,0x0f,0,0,1);  
             
  RplotEndPlot(rplot);
  return 0;
}
