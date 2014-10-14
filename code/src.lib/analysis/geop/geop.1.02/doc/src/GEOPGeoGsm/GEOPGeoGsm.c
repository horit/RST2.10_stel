/* GEOPGeoGsm.c
   ==-========= 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "geop.h"

int main() {

  double geo[3];
  double gsm[3];

  int yr=2000;
  int mo=06;
  int dy=10;
  int hr=5;
  int mt=0;
  int sc=0;

  geo[0]=3000;
  geo[1]=4500;
  geo[2]=18000;

  GEOPGeoGsm(yr,mo,dy,hr,mt,sc,geo,gsm);

  fprintf(stdout,"GEOx=%g GEOy=%g GEOz=%g\n", 
          geo[0],geo[1],geo[2]);
 

  fprintf(stdout,"GSMx=%g GSMy=%g GSMz=%g\n", 
          gsm[0],gsm[1],gsm[2]);
  return 0;
}

 






