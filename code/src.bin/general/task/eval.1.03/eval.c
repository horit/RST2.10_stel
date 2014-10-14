/* eval.c
   ====== */

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Software Toolkit - SuperDARN Toolkit" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"

#include "errstr.h"
#include "hlpstr.h"

#include "reval.h"

#include "rmath.h"

struct OptionData opt;
int arg=0;

/*
 $Log: eval.c,v $
 Revision 1.7  2005/08/08 15:10:10  barnes
 Added option to dump the command line options

 Revision 1.6  2004/05/05 16:09:46  barnes
 Added missing type.

 Revision 1.5  2004/05/04 23:00:36  barnes
 Changed math header name.

 Revision 1.4  2004/05/04 19:13:05  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/04/27 17:47:47  barnes
 Audit - Enforced warning set to "all".

 Revision 1.2  2004/04/22 20:28:37  barnes
 Fixed bug in decoding format information.

 Revision 1.1  2003/03/20 17:28:50  barnes
 Initial revision

 */


char *fnlist[]={
  "abs",
  "acos",
  "acosh",
  "asin",
  "asinh",
  "atan",
  "atan2",
  "atanh",
  "ceil",
  "cos",
  "cosh",
  "exp",
  "floor",
  "int",
  "log",
  "log10",
  "sin",
  "sinh",
  "sqrt",
  "tan",
  "tanh",
  0};
  


char *varlist[]={
  "PI",0
};


void decode_format(char *txt) {
  int i=0;
  int j=0;
  for (i=0;txt[i] !=0;i++) {
    if (txt[i] !='\\') {
      if (j !=i) txt[j]=txt[i];
      j++;
      continue;
    }
    if (txt[i+1]==0) break;
    if (txt[i+1]=='n') txt[j]='\n';
    if (txt[i+1]=='t') txt[j]='\t';
    if (txt[i+1]=='\"') txt[j]='\"';
    if (txt[i+1]=='\\') txt[j]='\\';
    j++;
    i++;
  }
  txt[j]=0;
}

int decode_value(char *ptr,double *value,void *data) {
  
  int i; 
  double *store;
   
  store=(double *)data;

  if ((isdigit(ptr[0])) || (ptr[0]=='.')) {
     *value=atof(ptr);
     return 0;
   }
 
  for (i=0;varlist[i] !=0;i++) if (strcmp(ptr,varlist[i])==0) break;
  *value=store[i];
  return 0;
}


int decode_function(char *ptr,int argnum,double *argptr,
                    double *value,void *data) {


  int i; 

  for (i=0;fnlist[i] !=0;i++) if (strcmp(ptr,fnlist[i])==0) break;

  switch(i) {
    case 0:
      *value=fabs(argptr[0]);
      break;
    case 1: 
      *value=acos(argptr[0]);
      break;
    case 2: 
      *value=acosh(argptr[0]);
      break;
    case 3: 
      *value=asin(argptr[0]);
      break;
    case 4: 
      *value=asinh(argptr[0]);
      break;
    case 5: 
      *value=atan(argptr[0]);
      break;
    case 6: 
      *value=atan2(argptr[0],argptr[1]);
      break;
    case 7: 
      *value=atanh(argptr[0]);
      break;
   case 8: 
      *value=ceil(argptr[0]);
      break;
   case 9: 
      *value=cos(argptr[0]);
      break;
  case 10: 
      *value=cosh(argptr[0]);
      break;
  case 11: 
      *value=exp(argptr[0]);
      break;
  case 12: 
      *value=floor(argptr[0]);
      break;
  case 13: 
      *value=(int) argptr[0];
      break;
  case 14: 
      *value=log(argptr[0]);
      break;
  case 15:
      *value=log10(argptr[0]);
      break;
  case 16: 
      *value=sin(argptr[0]);
      break;
  case 17: 
      *value=sinh(argptr[0]);
      break;
  case 18: 
      *value=sqrt(argptr[0]);
      break;
  case 19: 
      *value=tan(argptr[0]);
      break;
  case 20: 
      *value=tanh(argptr[0]);
      break;
      
  }
  return 0;
}

int main(int argc,char *argv[]) {
  unsigned char help=0;
  unsigned char option=0;

  int s;
  double store[2];
  double val;
  char *cast=NULL;
  char *format=NULL;
  char *dformat="%lg\n";
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"c",'t',&cast);
  OptionAdd(&opt,"f",'t',&format);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if (arg==argc) {
    OptionPrintInfo(stderr,errstr);
    exit(0);
  }
  if (format==NULL) format=dformat;
  else decode_format(format);
  store[0]=PI;
  s=Eval(argv[arg],&val,decode_value,store,decode_function,store);
  if (cast==NULL) fprintf(stdout,format,val);
  else if (cast[0]=='c') fprintf(stdout,format,(unsigned char) val);
  else if (cast[0]=='u') fprintf(stdout,format,(unsigned int) val);
  else if (cast[0]=='i') fprintf(stdout,format,(int) val);
  else if (cast[0]=='d') fprintf(stdout,format,val);
  return 0;
} 







