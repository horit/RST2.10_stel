/* geopidl.c
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

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "geop.h"

/*
 $Log: geopidl.c,v $
 Revision 1.3  2004/08/18 02:29:30  barnes
 Fixed bug in passing arguments to GsmGse

 Revision 1.2  2004/06/16 22:14:28  barnes
 Added missing headers.

 Revision 1.1  2004/06/15 22:21:48  barnes
 Initial revision

*/

int GEOPIDLGeoGsm(int argc,void *argv[]) {

   double dsec;
   int16 *syear;
   int16 *smonth;
   int16 *sday;
   int16 *shour;
   int16 *smin;
   int16 *ssec;
   double *bgeo;
   double *bgsm;

   syear = (int16 *) argv[0];
   smonth = (int16 *) argv[1];
   sday = (int16 *) argv[2];
   shour = (int16 *) argv[3];
   smin = (int16 *) argv[4];
   ssec = (int16 *) argv[5];

   dsec=*ssec;

   bgeo = (double *) argv[6];
   bgsm= (double *) argv[7];


   GEOPGeoGsm(*syear,*smonth,*sday,*shour,*smin,dsec,bgeo,bgsm);

   return 0;

}

int GEOPIDLGsmGeo(int argc,void *argv[]) {

   double dsec;
   int16 *syear;
   int16 *smonth;
   int16 *sday;
   int16 *shour;
   int16 *smin;
   int16 *ssec;
   double *bgsm;
   double *bgeo;

   syear = (int16 *) argv[0];
   smonth = (int16 *) argv[1];
   sday = (int16 *) argv[2];
   shour = (int16 *) argv[3];
   smin = (int16 *) argv[4];
   ssec = (int16 *) argv[5];

   dsec=*ssec;

   bgsm = (double *) argv[6];
   bgeo= (double *) argv[7];

   GEOPGsmGeo(*syear,*smonth,*sday,*shour,*smin,dsec,bgsm,bgeo);

   return 0;

}

int GEOPIDLGseGsm(int argc,void *argv[]) {


   double dsec;
   int16 *syear;
   int16 *smonth;
   int16 *sday;
   int16 *shour;
   int16 *smin;
   int16 *ssec;
   double *bgse;
   double *bgsm;

   syear = (int16 *) argv[0];
   smonth = (int16 *) argv[1];
   sday = (int16 *) argv[2];
   shour = (int16 *) argv[3];
   smin = (int16 *) argv[4];
   ssec = (int16 *) argv[5];

   dsec=*ssec;

   bgse = (double *) argv[6];
   bgsm= (double *) argv[7];

   GEOPGseGsm(*syear,*smonth,*sday,*shour,*smin,dsec,bgse,bgsm);
   return 0;
}

int GEOPIDLGsmGse(int argc,void *argv[]) {


   double dsec;
   int16 *syear;
   int16 *smonth;
   int16 *sday;
   int16 *shour;
   int16 *smin;
   int16 *ssec;
   double *bgse;
   double *bgsm;

   syear = (int16 *) argv[0];
   smonth = (int16 *) argv[1];
   sday = (int16 *) argv[2];
   shour = (int16 *) argv[3];
   smin = (int16 *) argv[4];
   ssec = (int16 *) argv[5];

   dsec=*ssec;

   bgsm= (double *) argv[6];
   bgse = (double *) argv[7];
   
   GEOPGsmGse(*syear,*smonth,*sday,*shour,*smin,dsec,bgsm,bgse);
   return 0;
}



int GEOPIDLGsmSm(int argc,void *argv[]) {

   double dsec;
   int16 *syear;
   int16 *smonth;
   int16 *sday;
   int16 *shour;
   int16 *smin;
   int16 *ssec;
   double *bgsm;
   double *bsm;

   syear = (int16 *) argv[0];
   smonth = (int16 *) argv[1];
   sday = (int16 *) argv[2];
   shour = (int16 *) argv[3];
   smin = (int16 *) argv[4];
   ssec = (int16 *) argv[5];

   dsec=*ssec;

   bgsm = (double *) argv[6];
   bsm= (double *) argv[7];


   GEOPGsmSm(*syear,*smonth,*sday,*shour,*smin,dsec,bgsm,bsm);

   return 0;

}


int GEOPIDLSmGsm(int argc,void *argv[]) {


   double dsec;
   int16 *syear;
   int16 *smonth;
   int16 *sday;
   int16 *shour;
   int16 *smin;
   int16 *ssec;
   double *bgsm;
   double *bsm;

   syear = (int16 *) argv[0];
   smonth = (int16 *) argv[1];
   sday = (int16 *) argv[2];
   shour = (int16 *) argv[3];
   smin = (int16 *) argv[4];
   ssec = (int16 *) argv[5];

   dsec=*ssec;

   bsm = (double *) argv[6];
   bgsm= (double *) argv[7];


   GEOPSmGsm(*syear,*smonth,*sday,*shour,*smin,dsec,bsm,bgsm);

   return 0;

}
