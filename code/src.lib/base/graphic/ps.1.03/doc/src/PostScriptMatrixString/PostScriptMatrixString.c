/* PostScriptMatrixString
   ======================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"
#include "rps.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 


int main(int argc,char *argv[]) {

  struct PostScript *ps=NULL;
  struct PostScriptMatrix *m=NULL;

  float wdt=400,hgt=400;
  float xoff=50,yoff=50;

  unsigned int fgcol=0xff000000;

  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);   
  PostScriptMakePlot(ps,xoff,yoff,wdt,hgt,0);

  m=PostScriptMatrixString("0.5 0 0 0.5");

  PostScriptRectangle(ps,m,10,10,wdt-20,hgt-20,
                  1,fgcol,0,NULL,NULL);


 
  PostScriptEndPlot(ps);
  return 0;
}

