/* SGP4DecodeTLE.c
   ===============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "sgp4.h"

char *txt[2]={
  "1 26998U 01055B   03359.04479272  .00000892  00000-0  11364-3 0  6691",
  "2 26998  74.0751 345.6618 0002762 268.5931  91.4883 14.83161036110721"
};



int main(int argc,char *argv[]) {

  struct SGP4TLE tle;
 
  SGP4DecodeTLE(txt,&tle);


  fprintf(stdout,"SGP4DecodeTLE\n");
  fprintf(stdout,"Spacecraft ID: %d\n",tle.catnr);
  fprintf(stdout,"Epoch: %g\n",tle.epoch);
  fprintf(stdout,"Revolution Number: %d\n",tle.revnum);


  return 0;
}
