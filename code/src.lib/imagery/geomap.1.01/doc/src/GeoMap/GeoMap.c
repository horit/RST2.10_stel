/* GeoMap.c
   ========
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rmap.h"
#include "gmap.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  char *fname="globe.ppm";
  FILE *fp;
  struct Rplot *rplot=NULL;
  struct FrameBuffer *map=NULL;
  struct FrameBuffer *img=NULL;
 
  float wdt=400,hgt=400;
  int x,y,px,py;
 
  MapTransform  tfunc;
  float marg[3];

  float *lat=NULL,*lon=NULL;

  marg[0]=90.0;
  marg[1]=0.0;
  marg[2]=1.0;

  fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  map=FrameBufferLoadPPM(fp,fname);

  if (map==NULL) {
    fprintf(stderr,"Error loading image map.\n");
    exit(-1);
  }
  fclose(fp);


  img=FrameBufferMake("dummy",wdt,hgt,24);

  tfunc=MapOrthographic;

  GeoMap(wdt,hgt,tfunc,marg,&lat,&lon);

  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      if (lat[y*(int) wdt+x]<-90) continue;
      px=map->wdt*lon[y*(int) wdt+x]/360.0;
      py=map->hgt*(0.5-lat[y*(int) wdt+x]/180.0);
      img->img[y*(int) wdt+x]=map->img[py*map->wdt+px];
      img->img[(int) (wdt*hgt)+y*(int) wdt+x]=
               map->img[map->wdt*map->hgt+py*map->wdt+px];
      img->img[(int) (2*wdt*hgt)+y*(int) wdt+x]=
               map->img[2*map->wdt*map->hgt+py*map->wdt+px];

    }
  }


  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Geomap",wdt,hgt,24);

  RplotImage(rplot,NULL,img,0x0f,0,0,1);  


  RplotEndPlot(rplot);
  return 0;
}
