/* NNetBackward
   ============ 
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rnnet.h"


#define MAX 3000

int main(int argc,char *argv[]) {

   int i;
   int order=4;

   int bit=0;
   int max;

   float inp[64];
   float out;
   float val;
   int cnt=0;

   float b=0.5;
   float m=0.2;  
   float e;
  
   struct NNet *net=NULL;
   int node[3];

   char *info={"Symmetry Network"};


   srandom(time(NULL));

   node[0]=order;
   node[1]=order;
   node[2]=1;
  
   net=NNetMakeNet(3,info,node,NNetSigActv,NNetSigDactv);
   NNetRndNet(net);   
 
   max=2<<(order-1);

   for (cnt=0;cnt<MAX;cnt++) {

     e=0;
     

     for (bit=0;bit<max;bit++) {

       for (i=0;i<order;i++) if (bit & (2<<i)) inp[i]=1;
       else inp[i]=0;
 
       
       for (i=0;i<(order/2);i++) if (inp[i] !=inp[order-1-i]) break;
       if (i<(order/2)) out=0;
       else out=1;
  
       NNetSetInput(net,inp);
       NNetForward(net);  
       
       e+=NNetCalcError(net,&out);
       NNetBackward(net,&out,b,m,3);   
      
          
     }

     fprintf(stdout,"Cycle=%.4d Mean Error=%g\n",cnt,e/16);
   }
   fprintf(stdout,"Result:\n");

   for (bit=0;bit<max;bit++) {

     for (i=0;i<order;i++) if (bit & (2<<i)) inp[i]=1;
     else inp[i]=0;
 
     for (i=0;i<(order/2);i++) if (inp[i] !=inp[order-1-i]) break;
     if (i<(order/2)) out=0;
     else out=1;
  

  

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















   
