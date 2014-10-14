/* nnet.c
   ==========
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
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rnnet.h"

/*
 $Log: nnet.c,v $
 Revision 1.6  2005/04/13 19:44:51  barnes
 Changed function name.

 Revision 1.5  2004/06/22 17:28:57  barnes
 Changed references of Long to Int.

 Revision 1.4  2004/06/16 16:44:54  barnes
 Used the rand function.

 Revision 1.3  2004/05/04 18:01:16  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:03:57  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/02/24 23:37:54  barnes
 Initial revision

*/


float NNetSigActv(float val) {
  return 1/(1+exp(-val));
}

float NNetSigDactv(float val,float sum) {
  return val*(1-val);
}


int NNetActvCode(float (*actv)(float)) {

  /* converts an activation function into a code number */

  if (actv==NNetSigActv) return 's';
  return 0;
}

void NNetCodeActv(int code,float (**actv)(float),
                  float (**dactv)(float,float)) {
  /* converts an activation function into a code */

  *actv=NULL;
  *dactv=NULL;

  if (code=='s') {
    *actv=NNetSigActv;
    *dactv=NNetSigDactv;
  }
     
}

float NNetFltRnd(float max){ 
  return (float) rand()/RAND_MAX*max;
}

void NNetSetInputLayer(struct NNetLayer *ptr,float *input) {
  int i;
  if (ptr==NULL) return;
  if (input==NULL) return;
  for (i=0;i<ptr->num;i++) (ptr->cell[i])->val=input[i];
}

float NNetCalcLayerError(struct NNetLayer *ptr,float *target) {
  float error=0;
  int i;
  if (ptr==NULL) return 0;
  if (target==NULL) return 0;

  for (i=0;i<ptr->num;i++) {
     error+=(target[i]-(ptr->cell[i])->val)*(target[i]-(ptr->cell[i])->val);
  }
  return error*0.5;
}

void NNetBackLayer(struct NNetLayer *ptr,float *target,float b,
                   float m,int mode) {

  /* mode determines what action is taken:
   *
   * mode=1 calculate change in weights  (batch learning)
   * mode=2 update weights              (batch learning)
   * mode=3 calculate change and update (online learning)
   */

  int i,j;
  if (ptr==NULL) return;
  for (i=0;i<ptr->num;i++) {

    if (mode & 0x01) {

      if (target !=NULL) 
        (ptr->cell[i])->delta.val=(target[i]-(ptr->cell[i])->val);

   
      if ((ptr->cell[i])->dactv !=NULL) 
          (ptr->cell[i])->delta.val=(ptr->cell[i])->delta.val *
          (ptr->cell[i])->dactv( (ptr->cell[i])->val,(ptr->cell[i])->sum);
    }

    for (j=0;j<(ptr->cell[i])->num;j++) {

      if (mode & 0x01) {

        (ptr->cell[i])->parent[j]->delta.val+=
        (ptr->cell[i])->delta.val*(ptr->cell[i])->wgt[j];
   
        /* calculate the our change in weights */

        (ptr->cell[i])->delta.wgt[j]=m*(ptr->cell[i])->delta.wgt[j]+
                                    b*(ptr->cell[i])->delta.val*
                                      (ptr->cell[i])->parent[j]->val;
      }

      /* update our weights */
 
      if (mode & 0x02) (ptr->cell[i])->wgt[j]+=(ptr->cell[i])->delta.wgt[j];
         
    }
    if (mode & 0x01) (ptr->cell[i])->delta.thr=m*(ptr->cell[i])->delta.thr+
                                               b*(ptr->cell[i])->delta.val;


    if (mode & 0x02) (ptr->cell[i])->thr+=(ptr->cell[i])->delta.thr;


    if (mode & 0x01) (ptr->cell[i])->delta.val=0;
  }
}

void NNetForwardLayer(struct NNetLayer *ptr) {
  int i,j;
  if (ptr==NULL) return;
  for (i=0;i<ptr->num;i++) {
     
    (ptr->cell[i])->sum=0;
  

    if ((ptr->cell[i])->parent==NULL) continue;
    if ((ptr->cell[i])->wgt==NULL) continue;
 
    for (j=0;j<(ptr->cell[i])->num;j++) {
      (ptr->cell[i])->sum+=(ptr->cell[i])->parent[j]->val*
                           (ptr->cell[i])->wgt[j];
    }
    if ((ptr->cell[i])->actv !=NULL) 
      (ptr->cell[i])->val=((ptr->cell[i])->actv)((ptr->cell[i])->sum+
                                           (ptr->cell[i])->thr);
    else (ptr->cell[i])->val=(ptr->cell[i])->sum;
  }
 

}

void NNetRndWgt(struct NNetLayer *ptr) {
  int i,j;
  if (ptr==NULL) return;
  for (i=0;i<ptr->num;i++) {
    if ((ptr->cell[i])->wgt==NULL) continue;
    (ptr->cell[i])->thr=NNetFltRnd(2.0)-1.0;
    for (j=0;j<(ptr->cell[i])->num;j++) (ptr->cell[i])->wgt[j]=
         NNetFltRnd(2.0)-1.0;
  }
}



void NNetFreeCell(struct NNetCell *ptr) {

  if (ptr->parent !=NULL) free(ptr->parent);
  if (ptr->wgt !=NULL) free(ptr->wgt);
  if (ptr->delta.wgt !=NULL) free(ptr->delta.wgt);
}

void NNetFreeLayer(struct NNetLayer *ptr) {
  int i; 
  if (ptr==NULL) return;
  
  for (i=0;i<ptr->num;i++) NNetFreeCell(ptr->cell[i]);
  if (ptr->cell !=NULL) free(ptr->cell);
  free(ptr);
}

void NNetFreeNet(struct NNet *ptr) {
  int i;
  if (ptr==NULL) return;
  for (i=0;i<ptr->num;i++) NNetFreeLayer(ptr->lyr[i]);
  if (ptr->info !=NULL) free(ptr->info);
  if (ptr->lyr !=NULL) free(ptr->lyr);
  free(ptr);
}


struct NNetCell *NNetMakeCell(int idx,struct NNetLayer *parent,
                         float (*actv)(float),
                         float (*dactv)(float,float)) {
  int i;
  struct NNetCell *ptr;
  ptr=malloc(sizeof(struct NNetCell));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct NNetCell));
  ptr->idx=idx;

  if (parent !=NULL) {
     ptr->num=parent->num;

      /* set the activation function */
      ptr->actv=actv;

      /* set the first order derivative activation function */
      ptr->dactv=dactv;

      /* make the weights */
 
      ptr->wgt=malloc(sizeof(float)*ptr->num);
      if (ptr->wgt==NULL) {
        free(ptr);
        return NULL;
      }

      ptr->delta.wgt=malloc(sizeof(float)*ptr->num);
      if (ptr->delta.wgt==NULL) {
          free(ptr->wgt);
          free(ptr);
          return NULL;
      }
      memset(ptr->delta.wgt,0,sizeof(float)*ptr->num);

 
       ptr->parent=malloc(sizeof(struct NNetCell *)*ptr->num);
       if (ptr->parent==NULL) {
          free(ptr->delta.wgt);
          free(ptr->wgt);
          free(ptr);
       }
          
       for (i=0;i<parent->num;i++) ptr->parent[i]=parent->cell[i];
  }
  return ptr;
}

struct NNetLayer *NNetMakeLayer(int num,struct NNetLayer *parent,
                         float (*actv)(float),
                         float (*dactv)(float,float)) {
  int i;
  struct NNetLayer *ptr;

  ptr=malloc(sizeof(struct NNetLayer));

  if (ptr==NULL) return NULL;
  ptr->num=num;
  ptr->cell=malloc(sizeof(struct NNetCell *)*num);
  if (ptr->cell==NULL) {
    free(ptr);
    return NULL;
  }

 
  for (i=0;i<num;i++) 
    if ((ptr->cell[i]=NNetMakeCell(i,parent,actv,dactv))==NULL) break;

   if (i<num) {
      int j;
      for (j=0;j<i;j++) NNetFreeCell(ptr->cell[j]);
      free(ptr);
      return NULL;
   }
   return ptr;  
}

struct NNet *NNetMakeNet(int num,char *info,int *nodes,
                     float (*actv)(float),
                     float (*dactv)(float,float)) {
  int i;
  struct NNet *ptr;

  if (nodes==NULL) return NULL;
  ptr=malloc(sizeof(struct NNet));
  if (ptr==NULL) return NULL;
  ptr->num=num;

  ptr->major=MAJOR_REVISION;
  ptr->minor=MINOR_REVISION;
  
  if (info !=NULL) {
    ptr->info=malloc(strlen(info)+1);
    if (ptr->info==NULL) {
      free(ptr);
      return NULL;
    }
    strcpy(ptr->info,info);
  } else ptr->info=NULL;

  ptr->lyr=malloc(num*sizeof(struct NNetLayer *));
  if (ptr->lyr==NULL) {
    if (ptr->info !=NULL) free(ptr->info);
    free(ptr);
    return NULL;
  }

  ptr->lyr[0]=NNetMakeLayer(nodes[0],NULL,NULL,NULL);
  if (ptr->lyr[0]==NULL) {
    if (ptr->info !=NULL) free(ptr->info);
    free(ptr->lyr);
    free(ptr);
    return NULL;
  }
 
  for (i=1;i<num;i++) {
    ptr->lyr[i]=NNetMakeLayer(nodes[i],ptr->lyr[i-1],actv,dactv);
    if (ptr->lyr[i]==NULL) break;
  }
   
  if (i<num) {
    int j;
    for (j=0;j<i;j++) NNetFreeLayer(ptr->lyr[i]);
    if (ptr->info !=NULL) free(ptr->info);
    free(ptr->lyr);
    free(ptr);
    return NULL;
  }
  return ptr;
}



void NNetRndNet(struct NNet *ptr) {
  int i;
  if (ptr==NULL) return;
  for (i=1;i<ptr->num;i++) NNetRndWgt(ptr->lyr[i]);
}


void NNetSetInput(struct NNet *ptr,float *input) {
  if (ptr==NULL) return;
  if (input==NULL) return;
  NNetSetInputLayer(ptr->lyr[0],input);
}

float NNetCalcError(struct NNet *ptr,float *target) {
  if (ptr==NULL) return -1;
  if (target==NULL) return -1;

  return NNetCalcLayerError(ptr->lyr[ptr->num-1],target);
}

void NNetForward(struct NNet *ptr) {
  int i;
  if (ptr==NULL) return;
  
  for (i=1;i<ptr->num;i++) NNetForwardLayer(ptr->lyr[i]);  

}

void NNetBackward(struct NNet *ptr,float *target,float b,float m,int mode) {
  int i;
  if (ptr==NULL) return;
  if (target==NULL) return;

  i=ptr->num-1;
  NNetBackLayer(ptr->lyr[i],target,b,m,mode);
  do {
     i--;
     NNetBackLayer(ptr->lyr[i],NULL,b,m,mode);
  } while (i>0);
}


void NNetOutputValue(struct NNet *ptr,float *output) {
  int i;
  struct NNetLayer *lyr;
  if (ptr==NULL) return;
  if (output==NULL) return;

  lyr=ptr->lyr[ptr->num-1];
  for (i=0;i<lyr->num;i++) output[i]=(lyr->cell[i])->val;
}


int NNetWriteCell(int fileno,struct NNetCell *ptr,
	       int (*actvc)(float (*a)(float))) {


  int i;
  int code=-1;
  if (ConvertWriteInt(fileno, (int32) ptr->num) !=0) return -1;
  if (ConvertWriteInt(fileno, (int32) ptr->idx) !=0) return -1;

  if (ptr->num==0) return 0; /* an input node */
  
  /* store activation function here */

  if (actvc !=NULL) code=(*actvc)(ptr->actv);
  else code=NNetActvCode(ptr->actv);

  if (ConvertWriteInt(fileno, (int32) code) !=0) return -1;

  /* store threshold */
     
  if (ConvertWriteFloat(fileno,ptr->thr) !=0) return -1;

  /* store delta threshold */
     
  if (ConvertWriteFloat(fileno,ptr->delta.thr) !=0) return -1;

  /* store weight */

  for (i=0;i<ptr->num;i++) 
      if (ConvertWriteFloat(fileno,ptr->wgt[i]) !=0) return -1;
  

  /* store delta weight */

  for (i=0;i<ptr->num;i++) 
      if (ConvertWriteFloat(fileno,ptr->delta.wgt[i]) !=0) return -1;


  /* store parent index */
  for (i=0;i<ptr->num;i++) 
      if (ConvertWriteInt(fileno, 
          (int32) (ptr->parent[i])->idx) !=0) return -1;

  return 0;

}


int NNetWriteLayer(int fileno,struct NNetLayer *ptr, 
              int (*actvc)(float (*a)(float))) {

  int i;
  if (ConvertWriteInt(fileno, (int32) ptr->num) !=0) return -1;
  for (i=0;i<ptr->num;i++) {
    if (NNetWriteCell(fileno,ptr->cell[i],actvc) !=0) break;
  }
  if (i<ptr->num) return -1;
  return 0;
}

int NNetWriteNet(int fileno,struct NNet *ptr,
              int (*actvc)(float (*a)(float))) {
  int i;
  int len=0;
  /* write some for of identifier */
 
  if (ConvertWriteInt(fileno, (int32) ptr->num) !=0) return -1;

  if (ConvertWriteInt(fileno, (int32) ptr->major) !=0) return -1;
  if (ConvertWriteInt(fileno, (int32) ptr->minor) !=0) return -1;
  
  if (ptr->info !=NULL) len=strlen(ptr->info)+1;
  if (ConvertWriteInt(fileno, (int32) len) !=0) return -1;

  if (write(fileno,ptr->info,len) !=len) return -1;


  for (i=0;i<ptr->num;i++) {
    if (NNetWriteLayer(fileno,ptr->lyr[i],actvc) !=0) break;
  }
  if (i<ptr->num) return -1;
  return 0;
}

int NNetFwriteNet(FILE *fp,struct NNet *ptr,
               int (*actvc)(float (*a)(float))) {
  return NNetWriteNet(fileno(fp),ptr,actvc);
}  


struct NNetCell *NNetReadCell(int fileno,struct NNetLayer *parent,
                       void (*cactv)(int,float (**a)(float),
                                         float (**d)(float,float))) {
  int32 num;
  int32 idx;
  int32 code;
  int i;
  struct NNetCell *ptr;
  

  if (ConvertReadInt(fileno,&num) !=0) return NULL;
  if (ConvertReadInt(fileno,&idx) !=0) return NULL;

  ptr=malloc(sizeof(struct NNetCell));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct NNetCell));
  ptr->idx=idx;
  ptr->num=num;

  if (ptr->num==0) return ptr; /* input node so no other information */

  /* read activation functions */

  if (ConvertReadInt(fileno,&code) !=0) return NULL;

 
  if (cactv !=NULL) (*cactv)(code,&ptr->actv,&ptr->dactv);
  else NNetCodeActv(code,&ptr->actv,&ptr->dactv);

 
  /* read threshold */

  if (ConvertReadFloat(fileno,&ptr->thr) !=0) {
     free(ptr);
     return NULL;
   }

  /* read delta threshold */
   if (ConvertReadFloat(fileno,&ptr->delta.thr) !=0) {
     free(ptr);
     return NULL;
   } 

   /* allocate memory for everthing */

   ptr->wgt=malloc(sizeof(float)*ptr->num);
   if (ptr->wgt==NULL) {
     free(ptr);
     return NULL;
   }
   ptr->delta.wgt=malloc(sizeof(float)*ptr->num);
   if (ptr->delta.wgt==NULL) {
     free(ptr->delta.wgt);
     free(ptr);
     return NULL;
   }
   ptr->parent=malloc(sizeof(struct NNetCell *)*ptr->num);
   if (ptr->delta.wgt==NULL) {
     free(ptr->wgt);
     free(ptr);
     return NULL;
   }


   /* read the weights */

   
   for (i=0;i<ptr->num;i++) 
      if (ConvertReadFloat(fileno,&ptr->wgt[i]) !=0) break;
   if (i<ptr->num) {
      free(ptr->delta.wgt);
      free(ptr->wgt);
      free(ptr);
      return NULL;
   }
 

   /* read the delta weights */

   
   for (i=0;i<ptr->num;i++) 
      if (ConvertReadFloat(fileno,&ptr->delta.wgt[i]) !=0) break;
   if (i<ptr->num) {
      free(ptr->delta.wgt);
      free(ptr->wgt);
      free(ptr);
      return NULL;
   }
 
  /* read parent indices */
 
  for (i=0;i<ptr->num;i++) {
    if (ConvertReadInt(fileno,&num) !=0) break;

    /* num contains the index of a node in the parent layer that
     * this node connects to. In theory "parent" should always be
     * a valid pointer to layer, if it isn't something is screwed
     * up in the file - but we will ignore it.
     */


    if ((parent !=NULL) && (num<parent->num)) { 
      int j;

      /* we have a valid parent layer and a valid node number within
         that layer - now we have to find which one it is */

      for (j=0;j<parent->num;j++) if (num==(parent->cell[j])->idx) break;   
      if (j<parent->num) ptr->parent[i]=parent->cell[j];
    }
  }

  if (i<ptr->num) {
      free(ptr->delta.wgt);
      free(ptr->wgt);
      free(ptr);
      return NULL;
   }


  return ptr;


}


struct NNetLayer *NNetReadLayer(int fileno,struct NNetLayer *parent,
                    void (*cactv)(int,float (**a)(float),
                                      float (**d)(float,float))) {



  struct NNetLayer *ptr;
  int32 num;
  int i;
  if (ConvertReadInt(fileno,&num) !=0) return NULL;

  ptr=malloc(sizeof(struct NNetLayer));

  if (ptr==NULL) return NULL;
  ptr->num=num;
  ptr->cell=malloc(sizeof(struct NNetCell *)*ptr->num);
  if (ptr->cell==NULL) {
    free(ptr);
    return NULL;
  }
  
  for (i=0;i<ptr->num;i++) 
     if ((ptr->cell[i]=NNetReadCell(fileno,parent,cactv))==NULL) break;


  if (i<ptr->num) {
     int j;
     for (j=0;j<i;j++) NNetFreeCell(ptr->cell[j]);
     free(ptr);
     return NULL;
  }   
  return ptr;
} 


struct NNet *NNetReadNet(int fileno,
                         void (*cactv)(int,float (**a)(float),
                                           float (**d)(float,float))) {
  struct NNet *ptr;
  int32 num;
  int i;
 


  if (ConvertReadInt(fileno,&num) !=0) return NULL;

  ptr=malloc(sizeof(struct NNet));
  if (ptr==NULL) return NULL;
  ptr->num=num;
  ptr->lyr=malloc(num*sizeof(struct NNetLayer *));
  if (ptr->lyr==NULL) {
    free(ptr);
    return NULL;
  }

  if (ConvertReadInt(fileno,&num) !=0) {
   free(ptr);
   return NULL;
  }
  ptr->major=num;

  if (ConvertReadInt(fileno,&num) !=0) {
   free(ptr);
   return NULL;
  }
  ptr->minor=num;

  if (ConvertReadInt(fileno,&num) !=0) {
   free(ptr);
   return NULL;
  }
   
  if (num !=0) {
    if ((ptr->info=malloc(num))==NULL) { 
       free(ptr);
       return NULL;
    }
   
    /* read in info here */
    if (read(fileno,ptr->info,num) !=num) {
       free(ptr->info);
       free(ptr);
       return NULL;
    }
  } else ptr->info=NULL;

  ptr->lyr[0]=NNetReadLayer(fileno,NULL,cactv);

  if (ptr->lyr[0]==NULL) {
     if (ptr->info !=NULL) free(ptr->info);
     free(ptr->lyr);
     free(ptr);
     return NULL;
  }

  for (i=1;i<ptr->num;i++) ptr->lyr[i]=NNetReadLayer(fileno,ptr->lyr[i-1],
                                                     cactv);  
  if (i<ptr->num) {
    int j;
    for (j=0;j<i;j++) NNetFreeLayer(ptr->lyr[j]);
    if (ptr->info !=NULL) free(ptr->info);
    free (ptr->lyr);
    free(ptr);
  }
  return ptr;

}

struct NNet *NNetFreadNet(FILE *fp,
                      void (*cactv)(int,float (**a)(float),
                                        float (**d)(float,float))) {

  return NNetReadNet(fileno(fp),cactv);
}
















