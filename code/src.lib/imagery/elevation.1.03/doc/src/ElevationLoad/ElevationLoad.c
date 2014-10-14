/* ElevationLoad.c
   ===============
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
#include "eldata.h"
#include "gmap.h"

struct ElevationData eldata;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  char *fname="elv.dat";
  FILE *fp;
 
  struct Rplot *rplot=NULL;
  struct FrameBuffer *img=NULL;
 
  float wdt=400,hgt=400;
  int x,y,px,py,c,s;
  float glat,glon,minlat,minlon,maxlat,maxlon;
  float fx,fy; 

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

  s=ElevationLoad(fp,&eldata);

  if (s != 0) {
    fprintf(stderr,"Error loading elevation data.\n");
    exit(-1);
  }
  fclose(fp);

  minlat=eldata.lat+eldata.latsec/3600.0;
  minlon=eldata.lon+eldata.lonsec/3600.0;
  maxlat=minlat+eldata.numlat*eldata.latstp/36000.0;
  maxlon=minlon+eldata.numlon*eldata.lonstp/36000.0;

  img=FrameBufferMake("dummy",wdt,hgt,8);

  tfunc=MapOrthographic;

  GeoMap(wdt,hgt,tfunc,marg,&lat,&lon);

  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      if (lat[y*(int) wdt+x]<-90) continue;
      glat=lat[y*(int) wdt+x];
      glon=lon[y*(int) wdt+x];

      if (glon>180) glon-=360;
      fy=(glat-minlat)/(maxlat-minlat);
      fx=(glon-minlon)/(maxlon-minlon);
     
      px=eldata.numlon*fx;
      py=eldata.numlat*fy;

       c=255*eldata.data[px*eldata.numlat+py]/3000.0;
       if (c>255) c=255;
       if (c<0) c=0;
       img->img[y*(int) wdt+x]=c;
    }
  }


  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"ElevationLoad",wdt,hgt,24);

  RplotImage(rplot,NULL,img,0x0f,0,0,1);  


  RplotEndPlot(rplot);
  return 0;
}
