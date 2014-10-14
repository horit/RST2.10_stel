/* NNetSigDactv
   ============ 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rnnet.h"

int main(int argc,char *argv[]) {

  float o;
  float d;
  float i;
 
  for (i=-1;i<1;i+=0.1) {
    o=NNetSigActv(i);
    d=NNetSigDactv(o,i);
    fprintf(stdout,"NNetSigActv(%.3g)=%.3g, NNetSigDactv(%.3g,%.3g)=%.3g\n",
            i,o,o,i,d);
  }
 
  return 0;
}
   
