/* rprmidl.c
   ========= 
   Author: R.J.Barnes
*/

/*
 $Log: rprmidl.h,v $
 Revision 1.3  2006/07/12 17:01:16  code
 Added origin code and beam azimuth

 Revision 1.2  2006/01/31 19:51:15  barnes
 Modification to use 32-bit integers for microseconds.

 Revision 1.1  2004/09/30 15:04:52  barnes
 Initial revision
 
*/


/*
 $Log: rprmidl.h,v $
 Revision 1.3  2006/07/12 17:01:16  code
 Added origin code and beam azimuth

 Revision 1.2  2006/01/31 19:51:15  barnes
 Modification to use 32-bit integers for microseconds.

 Revision 1.1  2004/09/30 15:04:52  barnes
 Initial revision

*/

#ifndef _RPRMIDL_H
#define _RPRMIDL_H


struct RadarIDLParm {
  struct {
    char major;
    char minor;
  } revision;

  struct {
    char code;
    IDL_STRING time;
    IDL_STRING command;
  } origin;


  int16 cp;
  int16 stid;

  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    int16 sc;
    int32 us;
  } time;

  int16 txpow;
  int16 nave;
  int16 atten;
  int16 lagfr;
  int16 smsep;
  int16  ercod;
  
  struct {
    int16 agc;
    int16 lopwr;
  } stat;

  struct {
    float search;
    float mean;
  } noise;

  int16 channel;
  int16 bmnum;
  float bmazm;
  int16 scan;
 
  int16 rxrise;
  struct {
    int16 sc;
    int32 us;
  } intt;
  int16 txpl;
  int16 mpinc;
  int16 mppul;
  int16 mplgs;
  int16 nrang;
  int16 frang;
  int16 rsep;
  int16 xcf;
  int16 tfreq;
  int16 offset; 

  int32 mxpwr;
  int32 lvmax;

  int16 pulse[PULSE_SIZE];
  int16 lag[LAG_SIZE*2];  
  IDL_STRING combf;
};

#endif
