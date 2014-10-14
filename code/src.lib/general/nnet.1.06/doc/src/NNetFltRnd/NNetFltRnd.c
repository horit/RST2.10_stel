/* NNetFltRnd
   ========== 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rnnet.h"


int main(int argc,char *argv[]) {

  int i;
  float o;
  float max=2;
 
  for (i=0;i<10;i++) {
    o=NNetFltRnd(max);
    fprintf(stdout,"fltrnd(%.3g)=%.3g\n",max,o);
  }
 
  return 0;
}
   


   
