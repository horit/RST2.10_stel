/* GEOPGseGsm.c
   =======-==== 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "geop.h"

int main() {

  double gse[3];
  double gsm[3];

  int yr=2000;
  int mo=06;
  int dy=10;
  int hr=5;
  int mt=0;
  int sc=0;

  gse[0]=22000;
  gse[1]=10000;
  gse[2]=18000;

  GEOPGseGsm(yr,mo,dy,hr,mt,sc,gse,gsm);

  fprintf(stdout,"GSEx=%g GSEy=%g GSEz=%g\n", 
          gse[0],gse[1],gse[2]);
 

  fprintf(stdout,"GSMx=%g GSMy=%g GSMz=%g\n", 
          gsm[0],gsm[1],gsm[2]);
  return 0;
}

 






