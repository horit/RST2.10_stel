/* SZAAngle.c
   ==========
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"
#include "sza.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc;
  int yrsec;

  double lat=50.3,lon=0.0;
  double mlon,dec,eqt,LsoT,Hangle,sza;

  yr=2002;
  mo=8;
  dy=30;
  hr=10;
  mt=0;
  sc=0;

  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  SZASolarLoc(yr,yrsec,&mlon,&dec);
  eqt=SZAEqOfTime(mlon,yr);

  LsoT=(hr*3600+mt*60)+(lon*4*60)+eqt;
  Hangle=15*((LsoT/3600)-12);

  sza=SZAAngle(lon,lat,dec,Hangle);  

  fprintf(stdout,"SZAAngle\n");
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",yr,mo,dy,hr,mt,sc);

  fprintf(stdout,"Input: lat=%g, lon=%g\n",lat,lon);
  fprintf(stdout,"Output: SZA=%g\n",sza);
  return 0;
}
