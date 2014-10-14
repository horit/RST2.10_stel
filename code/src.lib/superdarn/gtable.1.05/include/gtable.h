/* gtable.h
   ========
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
 $Log: gtable.h,v $
 Revision 1.4  2006/03/14 22:24:43  barnes
 Added the ability to pass in the altitude.

 Revision 1.3  2006/03/14 22:16:58  barnes
 Modification to deal with radars with more than 16 beams.

 Revision 1.2  2004/10/07 19:43:49  barnes
 Removed write routines.

 Revision 1.1  2004/08/20 13:18:26  barnes
 Initial revision

*/


#ifndef _GTABLE_H
#define _GTABLE_h

#define MAJOR_REVISION 0
#define MINOR_REVISION 91

struct GridPnt {
  int cnt;
  int npix;
  int ref;
  double mlat;
  double mlon;
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
  
};

struct GridTable {
  double st_time;
  double ed_time;
  int chn;
  int status;
  int st_id;
  int prog_id;
  int nscan;
  int npnt;
  int frang[MAX_BEAM],rsep[MAX_BEAM],rxrise[MAX_BEAM];
  double freq;
  struct {
    double mean;
    double sd;
  } noise;
  int gsct; 
  double min[4],max[4];
 
  int ltable[MAX_RANGE*MAX_BEAM];
  double itab[MAX_RANGE*MAX_BEAM];
  double azm[MAX_RANGE*MAX_BEAM]; 
  int pnum;
  struct GridPnt *pnt;
};

int GridTableTest(struct GridTable *ptr,struct RadarScan *scan,int tlen);
void GridTableMap(struct GridTable *ptr,struct RadarScan *scan,
                  struct RadarSite *pos,int tlen,int iflg,double alt);
 
#endif
