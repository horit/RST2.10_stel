/* NNetActvCode
   ============ 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rnnet.h"

float test_actv(float i) {
  if (i<0.5) i=0;
  i=1;
  return i;
}

int main(int argc,char *argv[]) {
  fprintf(stdout,"NNetSigActv() = %d\n",NNetActvCode(NNetSigActv));
  fprintf(stdout,"testactv() = %d\n",NNetActvCode(test_actv)); 
  return 0;
}
   
