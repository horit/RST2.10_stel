/* sgp4.h
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
 $Log: sgp4.h,v $
 Revision 1.1  2004/07/12 21:43:03  barnes
 Initial revision

*/

struct SGP4Geodetic {
  double lat,lon,alt;
  double theta;
};

struct SGP4XYZVector {
  double x,y,z;
};

struct SGP4TLE {
    double epoch;
    double xndt2o;
    double xndd6o;
    double bstar;
    double xincl;
    double xnodeo;
    double eo;
    double omegao;
    double xmo;
    double xno;
    int catnr;
    int elset;
    int revnum; 
    char idesg[256];
};

struct SGP4 {
  double aodp;
  double aycof;
  double c1,c4,c5;
  double cosio;
  double d2,d3,d4;
  double delmo;
  double omgcof;
  double eta;
  double omgdot;
  double sinio;
  double xnodp;
  double sinmo;
  double t2cof;
  double t3cof;
  double t4cof;
  double t5cof;
  double x1mth2,x3thm1,x7thm1;
  double xmcof;
  double xmdot;
  double xnodcf;
  double xnodot;
  double xlcof;
  int simple;
};

double SGP4TLEToJulian(struct SGP4TLE *TLE);

int SGP4DecodeTLE(char *txt[2],struct SGP4TLE *TLE);

int SGP4Setup(struct SGP4 *SGP4,struct SGP4TLE *TLE);

int SGP4Calculate(double tsince,struct SGP4 *SGP4,
                  struct SGP4TLE *TLE,
		  struct SGP4XYZVector *pos,
                  struct SGP4XYZVector *vel);

int SGP4GeodeticPosition(double time,struct SGP4XYZVector *pos,
                      struct SGP4Geodetic *geo);

double SGP4YMDHMSToTsince(int yr,int mo,int dy,int hr,int mt,double sc,
                          struct SGP4TLE *TLE);

double SGP4EpochToTsince(double tval,struct SGP4TLE *TLE);

double SGP4EpochToJdoy(double tval);
double SGP4YMDHMSToJdoy(int yr,int mo,int dy,int hr,int mt,double sc);
