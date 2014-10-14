/* rmap.h
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

/*
 $Log: rmap.h,v $
 Revision 1.1  2003/04/11 23:37:03  barnes
 Initial revision

*/

#ifndef _RMAP_H
#define _RMAP_H

typedef int (*MapTransform)(int,void *,int,void *,void *);

int MapCylindrical(int ssze,void *src,int dsze,void *dst,void *data);
int MapMercator(int ssze,void *src,int dsze,void *dst,void *data);
int MapTransverseMercator(int ssze,void *src,int dsze,void *dst,void *data);
int MapStereographic(int ssze,void *src,int dsze,void *dst,void *data);
int MapOrthographic(int ssze,void *src,int dsze,void *dst,void *data);
int MapGeneralVerticalPerspective(int ssze,
                                  void *src,int dsze,
                                  void *dst,void *data);

struct PolygonData *MapFread(FILE *fp);
struct PolygonData *MapBndFread(FILE *fp);

struct PolygonData *MapCircleClip(float step);
struct PolygonData *MapSquareClip();
 
void MapPlotPolygon(struct Rplot *rplot,struct RplotMatrix *matrix,
                    float x,float y,float w,float h,int fill,
                    unsigned int color,unsigned char mask,
                    float width,struct RplotDash *dash,
                    struct PolygonData *poly,int type);
 

void MapPlotOpenPolygon(struct Rplot *rplot,struct RplotMatrix *matrix,
                    float x,float y,float w,float h,
                    unsigned int color,unsigned char mask,
                    float width,struct RplotDash *dash,
		    struct PolygonData *poly,int type);

struct PolygonData *MapWraparound(struct PolygonData *map);

#endif
