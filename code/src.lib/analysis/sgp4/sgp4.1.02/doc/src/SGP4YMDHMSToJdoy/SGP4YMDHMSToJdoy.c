/* SGP4YMDHMSToJdoy.c
   ==================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"
#include "sgp4.h"




int main(int argc,char *argv[]) {

  int yr=2004;
  int mo=8;
  int dy=30;
  int hr=12;
  int mt=0;
  double sc=0;
  double jdoy;

  jdoy=SGP4YMDHMSToJdoy(yr,mo,dy,hr,mt,sc);

  fprintf(stdout,"SGP4YMDHMSToJdoy\n");

  fprintf(stdout,"Date: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt,(int) sc);
  fprintf(stdout,"Julian decimal day : %g\n",jdoy);


  return 0;
}
