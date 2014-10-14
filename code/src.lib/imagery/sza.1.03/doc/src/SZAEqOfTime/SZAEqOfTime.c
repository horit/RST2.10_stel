/* SZAEqOfTime.c
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

  double mlon,dec,eqt;

  yr=2002;
  mo=8;
  dy=30;
  hr=10;
  mt=0;
  sc=0;

  yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);

  SZASolarLoc(yr,yrsec,&mlon,&dec);
  eqt=SZAEqOfTime(mlon,yr);



  fprintf(stdout,"SZAEqOfTime\n");
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",yr,mo,dy,hr,mt,sc);

  fprintf(stdout,"Output: Equation of Time=%g\n",eqt);
  return 0;
}
