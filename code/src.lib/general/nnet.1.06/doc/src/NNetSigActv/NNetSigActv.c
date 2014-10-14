/* NNetSigActv
   =========== 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rnnet.h"

int main(int argc,char *argv[]) {

  float o;
  float i;
 
  for (i=-1;i<1;i+=0.1) {
    o=NNetSigActv(i);
    fprintf(stdout,"NNetSigActv(%.3g)=%.3g\n",i,o);
  }
 
  return 0;
}
   

   
