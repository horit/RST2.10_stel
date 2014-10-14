/* NNetCodeActv
   ============ 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rnnet.h"



int main(int argc,char *argv[]) {
  float i;
  float o=0;
  float d=0;
  
  char code='s';

  float (*actv)(float)=NULL;
  float (*dactv)(float,float)=NULL;

  NNetCodeActv(code,&actv,&dactv); 
 
  for (i=-1;i<1;i+=0.1) {
  
    if (actv !=NULL)  o=(*actv)(i);
    if (dactv !=NULL) d=(*dactv)(o,i);
    fprintf(stdout,"(actv=%x)(%.3g)=%.3g, (dactv=%x)(%.3g,%.3g)=%.3g\n",
            (int) actv,i,o,(int) dactv,o,i,d);
  }
 
  return 0;
}
   

   
