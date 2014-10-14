/* polygon.h
   =========
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
 $Log: polygon.h,v $
 Revision 1.1  2003/03/17 18:28:00  barnes
 Initial revision

*/


#ifndef _POLYGON_H
#define _POLYGON_H

struct PolygonData {
  int polnum;
  int pntnum;
  int *num;
  int *off;
  int *type;
  int sze;
  int *op;
  void *data;
  void *box;
  int (*bbox)(void *a,void *b,int);
};

int PolygonXYbbox(void *a,void *b,int s);
struct PolygonData *PolygonMake(int sze,int (*bbox)(void *a,void *b,int s));
void PolygonFree(struct PolygonData *a);
int PolygonAddPolygon(struct PolygonData *a,int type);
int PolygonAdd(struct PolygonData *a,void *pnt,int op);
void *PolygonRead(struct PolygonData *ptr,int n,int o);
int PolygonReadOp(struct PolygonData *ptr,int n,int o);
int PolygonReadType(struct PolygonData *ptr,int n);
int PolygonRemove(struct PolygonData *a);
struct PolygonData *PolygonCopy(struct PolygonData *src);
struct PolygonData *PolygonClip(struct PolygonData *apol,
                                struct PolygonData *bpol);
struct PolygonData *PolygonTransform(struct PolygonData *src,
                                     int sze,
				     int (*bbox)(void *x,
						void *y,int s),
                                     int (*trf)(int ssze,void *src,int dsze,
                                                void *dst,void *data),
                                                void *data);

void PolygonModify(struct PolygonData *src,
                   int (*trf)(int ssze,void *src,int dsze,
                              void *dst,void *data),void *data);


#endif



