/* NNetFread
   ========= 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rnnet.h"


#define MAX 3000
#define FNAME "parity.net"


int main(int argc,char *argv[]) {

   FILE *fp;

   int i,c;
   int order=4;
   int bit=0;
   int max;

   float inp[64];
   float val,out;
  
   struct NNet *net=NULL;
  
   srandom(time(NULL));

   fp=fopen(FNAME,"r");
   if (fp==NULL) {
     fprintf(stderr,"File not found.\n");
     exit(-1); 
   }

   net=NNetFreadNet(fp,NNetCodeActv);
   if (net==NULL) {
     fprintf(stderr,"Error reading file.\n");
     exit(-1);
   }
  
   fclose(fp); 


   fprintf(stdout,"Neural Network Information:");
   if (net->info !=NULL) fprintf(stdout,"%s\n",net->info);
   else fprintf(stdout,"None.\n");  
 
   fprintf(stdout,"Libary Version Number:%d:%d\n",net->major,net->minor);
   fprintf(stdout,"Layers:%.2d\n\n",net->num);
   for (i=0;i<net->num;i++) 
     fprintf(stdout,"Layer %.2d Nodes %.2d\n",i,(net->lyr[i])->num);
   fprintf(stdout,"\n");

   order=(net->lyr[0])->num;

   max=2<<(order-1);


   for (bit=0;bit<max;bit++) {

     for (i=0;i<order;i++) if (bit & (2<<i)) inp[i]=1;
     else inp[i]=0;
 
     c=0;
     for (i=0;i<order;i++) if (inp[i] !=0) c++;
     out=(float) (c % 2); 
  

     NNetSetInput(net,inp);
     NNetForward(net);  
     
     NNetOutputValue(net,&val);

     fprintf(stdout,"%.2d:",bit);
     for (i=0;i<order;i++)  if (bit & (2<<i)) fprintf(stdout,"1");   
     else fprintf(stdout,"0");
    
     fprintf(stdout," %g %g",out,val);
     fprintf(stdout,"\n");
   }


  return 0;
}
   
