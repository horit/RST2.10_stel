/* expr.c
   ======
   Author: R.J.Barnes
*/

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
#include "reval.h"
#include "rawdata.h"
#include "fitdata.h"
#include "tplot.h"


/*
 $Log: expr.c,v $
 Revision 1.3  2004/06/21 22:47:50  barnes
 Added missing header.

 Revision 1.2  2004/05/10 21:57:49  barnes
 Removed redundant code.

 Revision 1.1  2003/04/25 14:50:12  barnes
 Initial revision

*/


struct edata {
  struct tplot *ptr;
  int rng;
};

char *varlist[]={
  "rng",
  "beam",
  "channel",
  "cpid",
  "scan",
  "nave",
  "noise",
  "freq",
  "atten",
  "gsct",
  "v",
  "v_e",
  "w_l",
  "w_l_e",
  "p_l",
  "p_l_e",
  0
};


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

int decode_value(char *ptr,double *value,void *data) {
  
  int i; 
  struct edata *e;
  e=(struct edata *)data;

 
  if ((isdigit(ptr[0])) || (ptr[0]=='.')) {
     *value=atof(ptr);
     return 0;
   }
 
  for (i=0;varlist[i] !=0;i++) if (strcmp(ptr,varlist[i])==0) break;

  switch (i) {
  case 0:
    *value=e->rng;
    break;
  case 1:
    *value=(e->ptr)->bmnum;
    break;
  case 2:
    *value=(e->ptr)->channel;
    break;
  case 3:
    *value=(e->ptr)->cpid;
    break;
  case 4:
    *value=(e->ptr)->scan;
    break;
  case 5:
    *value=(e->ptr)->nave;
    break;
  case 6:
    *value=(e->ptr)->noise;
    break;
  case 7:
    *value=(e->ptr)->tfreq;
    break;
  case 8:
    *value=(e->ptr)->atten;
    break;
  case 9:
    *value=(e->ptr)->gsct[e->rng];
    break;
  case 10:
    *value=(e->ptr)->v[e->rng];
    break;
  case 11:
    *value=(e->ptr)->v_e[e->rng];
    break;
  case 12:
    *value=(e->ptr)->w_l[e->rng];
    break;
  case 13:
    *value=(e->ptr)->w_l_e[e->rng];
    break;
  case 14:
    *value=(e->ptr)->p_l[e->rng];
    break;
  case 15:
    *value=(e->ptr)->p_l_e[e->rng];
    break;

  } 
  return 0;
}


double eval_expr(char *expr,
                 struct tplot *ptr,int rng) { 
  int s;
  double value=0;

  struct edata data;

  data.ptr=ptr;
  data.rng=rng;
 
  s=Eval(expr,&value,decode_value,&data,decode_function,&data);
  /* should report an error in here */

  return value;
  

}

