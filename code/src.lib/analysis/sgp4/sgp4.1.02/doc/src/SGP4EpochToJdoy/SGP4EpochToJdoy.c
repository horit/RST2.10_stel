/* SGP4EpochToJdoy.c
   =================
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
  double tval,jdoy;


  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  jdoy=SGP4EpochToJdoy(tval);

  fprintf(stdout,"SGP4EpochToJdoy\n");

  fprintf(stdout,"Date: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt,(int) sc);
  fprintf(stdout,"Julian decimal day : %g\n",jdoy);


  return 0;
}
