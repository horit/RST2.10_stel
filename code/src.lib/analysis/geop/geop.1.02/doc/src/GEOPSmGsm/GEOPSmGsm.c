/* GEOPSmGsm.c
   =========== 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "geop.h"

int main() {

  double sm[3];
  double gsm[3];

  int yr=2000;
  int mo=06;
  int dy=10;
  int hr=5;
  int mt=0;
  int sc=0;

  sm[0]=2.15;
  sm[1]=-2;
  sm[2]=2.15;

  GEOPSmGsm(yr,mo,dy,hr,mt,sc,sm,gsm);

  fprintf(stdout,"SMx=%g SMy=%g SMz=%g\n", 
          sm[0],sm[1],sm[2]);
 
  fprintf(stdout,"GSMx=%g GSMy=%g GSMz=%g\n", 
          gsm[0],gsm[1],gsm[2]);

  return 0;
}

 






