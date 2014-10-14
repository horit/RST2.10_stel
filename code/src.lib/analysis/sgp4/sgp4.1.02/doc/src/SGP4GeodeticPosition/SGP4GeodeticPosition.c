/* SGP4GeodeticPosition.c
   ======================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"
#include "sgp4.h"

char *txt[2]={
  "1 26998U 01055B   03359.04479272  .00000892  00000-0  11364-3 0  6691",
  "2 26998  74.0751 345.6618 0002762 268.5931  91.4883 14.83161036110721"
};



int main(int argc,char *argv[]) {

  int yr=2004;
  int mo=8;
  int dy=30;
  int hr=12;
  int mt=0;
  double sc=0;
  double tval,tsince,jdoy;

  struct SGP4TLE tle;
  struct SGP4 sgp4;
  struct SGP4XYZVector pos,vel;
  struct SGP4Geodetic geo;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  SGP4DecodeTLE(txt,&tle);
  SGP4Setup(&sgp4,&tle);

  tsince=SGP4EpochToTsince(tval,&tle);
  jdoy=SGP4EpochToJdoy(tval);

  SGP4Calculate(tsince,&sgp4,&tle,&pos,&vel);

  SGP4GeodeticPosition(jdoy,&pos,&geo);

  fprintf(stdout,"SGP4GeodeticPosition\n");
  fprintf(stdout,"Spacecraft ID: %d\n",tle.catnr);
  fprintf(stdout,"Date: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt,(int) sc);
  fprintf(stdout,"Latitude: %g\n",geo.lat);
  fprintf(stdout,"Longitude: %g\n",geo.lon);
  fprintf(stdout,"Altitude:  %g\n",geo.alt);



  return 0;
}
