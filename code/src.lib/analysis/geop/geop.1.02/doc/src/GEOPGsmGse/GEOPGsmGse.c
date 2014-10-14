/* GEOPGsmGse.c
   ============ 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "geop.h"

int main() {

  double gsm[3];
  double gse[3];

  int yr=2000;
  int mo=06;
  int dy=10;
  int hr=5;
  int mt=0;
  int sc=0;

  gsm[0]=5000;
  gsm[1]=2000;
  gsm[2]=7000;
 
  GEOPGsmGse(yr,mo,dy,hr,mt,sc,gse,gse);

  fprintf(stdout,"GSMx=%g GSMy=%g GSMz=%g\n", 
          gsm[0],gsm[1],gsm[2]);

  fprintf(stdout,"GSEx=%g GSEy=%g GSEz=%g\n", 
          gse[0],gse[1],gse[2]);

  return 0;
}

 






