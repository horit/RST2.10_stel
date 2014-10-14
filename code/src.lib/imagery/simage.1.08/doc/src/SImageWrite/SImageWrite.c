/* SImageWrite.c
   ==============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "simage.h"

struct SImage img;

int main(int argc,char *argv[]) {
 
  FILE *fp;
  int fd;

  int yr,mo,dy,hr,mt;
  double sc;

  fp=fopen(argv[1],"r");
 
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  fd=fileno(fp);

  while(SImageRead(fd,&img) !=-1) {

    TimeEpochToYMDHMS(img.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
            yr,mo,dy,hr,mt,(int) sc,img.numpnt);

    SImageWrite(fileno(stdout),&img);
    
  }
 
  fclose(fp);


  return 0;
}
