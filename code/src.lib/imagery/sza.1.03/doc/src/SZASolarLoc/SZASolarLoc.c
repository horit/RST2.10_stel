/* SZASolarLoc.c
   =============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"
#include "sza.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc;
  int yrsec;

  double mlon,dec;

  yr=2002;
  mo=8;
  dy=30;
  hr=10;
  mt=0;
  sc=0;

  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  SZASolarLoc(yr,yrsec,&mlon,&dec);



  fprintf(stdout,"SZASolarLoc\n");
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",yr,mo,dy,hr,mt,sc);

  fprintf(stdout,"Output: mean solar longitude=%g, declination=%g\n",mlon,dec);
  return 0;
}
