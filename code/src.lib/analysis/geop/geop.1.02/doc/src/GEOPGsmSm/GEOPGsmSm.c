/* GEOPGsmSm.c
   =========== 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "geop.h"

int main() {

  double gsm[3];
  double sm[3];

  int yr=2000;
  int mo=06;
  int dy=10;
  int hr=5;
  int mt=0;
  int sc=0;

  gsm[0]=5000;
  gsm[1]=2000;
  gsm[2]=7000;

  GEOPGsmSm(yr,mo,dy,hr,mt,sc,gsm,sm);

  fprintf(stdout,"GSMx=%g GSMy=%g GSMz=%g\n", 
          gsm[0],gsm[1],gsm[2]);

  fprintf(stdout,"SMx=%g SMy=%g SMz=%g\n", 
          sm[0],sm[1],sm[2]);


  return 0;
}

 






