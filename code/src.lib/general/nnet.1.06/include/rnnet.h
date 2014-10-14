/* rnnet.h
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

/*
 $Log: rnnet.h,v $
 Revision 1.1  2003/03/01 19:17:37  barnes
 Initial revision

*/


#ifndef _RNNET_H
#define _RNNET_H

#define MAJOR_REVISION 0
#define MINOR_REVISION 90

struct NNetCell {
  int num; /* number of connections to this cell */
  int idx; /* the index of this cell in the layer */

  float (*actv)(float);         /* activation function */
  float (*dactv)(float,float);  /* derivative of activation function */

  struct NNetCell **parent;         /* pointer to  parent cell */
  float *wgt;                   /* link weights */
  float thr;                    /* threshold */
  float sum;                    /* sum of weight.input */
  float val;                    /* output value */

  struct {
     float *wgt;
     float thr;
     float val;
  } delta;
};


struct NNetLayer {
   int num;
   struct NNetCell **cell;
};

struct NNet {
  int num;

  int major,minor; /* major and minor version numbers of the code */

  char *info; /* informational message */

  struct NNetLayer **lyr;
};


/* sigmoid activation function */

float NNetSigActv(float val);
float NNetSigDactv(float val,float sum);
 

/* convert activation functions from function pointers to codes
 * and back again.
 */

int NNetActvCode(float (*actv)(float));
void NNetCodeActv(int code,float (**actv)(float),float (**dactv)(float,float));


/* floating point random number */

float NNetFltRnd(float max);

/* the following are really internal functions but could 
 *  be called for some esoteric nets. 
 */

void NNetSetInputLayer(struct NNetLayer *ptr,float *input);
float NNetCalcLayerError(struct NNetLayer *ptr,float *target);
 
void NNetBackLayer(struct NNetLayer *ptr,float *target,float b,float m,
                   int mode);
void NNetForwardLayer(struct NNetLayer *ptr);

void NNetRndWgt(struct NNetLayer *ptr);

void NNetFreeCell(struct NNetCell *ptr);
void NNetFreeLayer(struct NNetLayer *ptr);

struct NNetCell *NNetMakeCell(int idx,struct NNetLayer *parent,
                         float (*actv)(float),
                         float (*dactv)(float,float));
 
struct NNetLayer *NNetMakeLayer(int num,struct NNetLayer *parent,
                         float (*actv)(float),
                         float (*dactv)(float,float));


int NNetWriteCell(int fileno,struct NNetCell *ptr,
	       int (*actvc)(float (*a)(float)));
int NNetWriteLayer(int fileno,struct NNetLayer *ptr, 
              int (*actvc)(float (*a)(float)));

struct NNetCell *NNetReadCell(int fileno,struct NNetLayer *parent,
                       void (*cactv)(int,float (**a)(float),
                                         float (**d)(float,float)));
struct NNetLayer *NNetReadLayer(int fileno,struct NNetLayer *parent,
                    void (*cactv)(int,float (**a)(float),
                                      float (**d)(float,float)));



/* These are the documented interfaces */

void NNetFreeNet(struct NNet *ptr); 
struct NNet *NNetMakeNet(int num,char *info,int *nodes,
                     float (*actv)(float),
                     float (*dactv)(float,float));
void NNetRndNet(struct NNet *ptr);
void NNetSetInput(struct NNet *ptr,float *input);
float NNetCalcError(struct NNet *ptr,float *target);
 

void NNetForward(struct NNet *ptr);
void NNetBackward(struct NNet *ptr,float *target,float b,float m,int mode);
void NNetOutputValue(struct NNet *ptr,float *output);
  




int NNetWriteNet(int fileno,struct NNet *ptr,
              int (*actvc)(float (*a)(float)));
int NNetFwriteNet(FILE *fp,struct NNet *ptr,
               int (*actvc)(float (*a)(float)));
  
struct NNet *NNetReadNet(int fileno,
                         void (*cactv)(int,float (**a)(float),
                                           float (**d)(float,float))); 
struct NNet *NNetFreadNet(FILE *fp,
                      void (*cactv)(int,float (**a)(float),
                                        float (**d)(float,float)));

#endif
















