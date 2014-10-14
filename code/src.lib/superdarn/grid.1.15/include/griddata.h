/* griddata.h
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
 $Log: griddata.h,v $
 Revision 1.3  2004/10/07 21:47:00  barnes
 modified data structure.

 Revision 1.2  2004/10/07 20:08:39  barnes
 Removed read functions from header.

 Revision 1.1  2004/07/12 22:23:12  barnes
 Initial revision

*/


#ifndef _GRIDDATA_H
#define _GRIDDATA_H


struct GridSVec {
  int st_id;
  int chn;
  int npnt;
  double freq0;
  char major_revision;
  char minor_revision;
  int prog_id;
  char gsct;

  struct {
    double mean;
    double sd;
  } noise;

  struct {
    double min;
    double max;
  } vel;

  struct {
    double min;
    double max;
  } pwr;

  struct {
    double min;
    double max;
  } wdt;

  struct {
    double min;
    double max;
  } verr;


};

struct GridGVec {
  double mlat,mlon;
  double azm;

  struct {
   double median;
   double sd;
  } vel;

  struct {
   double median;
   double sd;
  } pwr;

  struct {
   double median;
   double sd;
  } wdt;

  int st_id;
  int chn;
  int index;
 
};

struct GridData {
  double st_time;
  double ed_time;
 
  int stnum;
  int vcnum;
  unsigned char xtd;
  struct GridSVec *sdata;
  struct GridGVec *data;
};

int GridLocateCell(int npnt,struct GridGVec *ptr,int index);
void GridMerge(struct GridData *,struct GridData *);
void GridAverage(struct GridData *,struct GridData *,int flg);

void GridCopy(struct GridData *a,struct GridData *b);
void GridAdd(struct GridData *a,struct GridData *b,int recnum);
void GridSort(struct GridData *ptr);
void GridIntegrate(struct GridData *a,struct GridData *b,double *err);


#endif



