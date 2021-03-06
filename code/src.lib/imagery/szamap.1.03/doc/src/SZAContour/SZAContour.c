/* SZAContour.c
   ============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rtime.h"
#include "polygon.h"
#include "rmap.h"
#include "contour.h"
#include "sza.h"
#include "szamap.h"

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;


double zval[2]={95,105};

struct PolygonData **zenith;
struct PolygonData **nzenith;
struct PolygonData *pzenith[2];

struct PolygonData *clip;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc;

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffff;
  unsigned int lndcol=0xa0ffa0;
  unsigned int seacol=0xa0a0ff;
  unsigned int grdcol[2]={0xa0a0a0,0x808080};

 
  MapTransform  tfunc;
  float marg[3];

  int c;  
  char *envstr;
  FILE *mapfp;

  yr=2002;
  mo=31;
  dy=26;
  hr=22;
  mt=0;
  sc=0;

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   


  zenith=SZAContour(yr,mo,dy,hr,mt,sc,1,0,1,2,zval);


  marg[0]=90.0;
  marg[1]=0.0;
  marg[2]=1.0;

  tfunc=MapStereographic;

  clip=MapCircleClip(10); 

  nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);

  pmap=PolygonClip(clip,nmap); 
 
  nzenith=ContourTransform(2,zenith,tfunc,marg);

  
  for (c=0;c<2;c++) {
    pzenith[c]=PolygonClip(clip,nzenith[c]); 
  }
  

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"SZAContour",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);
  RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2,hgt/2,
                  1,seacol,0x0f,0,NULL);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,lndcol,0x0f,0,NULL,
                   pmap,1);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,lndcol,0x0f,0,NULL,
                   pmap,3);
  
  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,seacol,0x0f,0,NULL,
                   pmap,0);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,grdcol[0],0x0f,0,NULL,
                   pzenith[0],1);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,grdcol[1],0x0f,0,NULL,
                   pzenith[1],1);



  RplotEndPlot(rplot);
  return 0;
}
