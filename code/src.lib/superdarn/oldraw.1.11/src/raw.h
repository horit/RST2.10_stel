/* raw.h
   =====
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
 $Log: raw.h,v $
 Revision 1.1  2004/07/13 22:31:47  barnes
 Initial revision

*/


#ifdef SEC
#undef SEC
#endif

struct radar_parms {

  struct {
    char MAJOR,MINOR;
  } REV;/* REV.MAJOR is the major revision #, REV.MINOR is the minor # */

  int16  NPARM,	/* the total number of 16-bit words in the parameter block */
	 ST_ID,	/* station ID code: 1=Goose Bay, 2=Schefferville,
					4=Halley Station, 8=Syowa */
	 YEAR,	/* date and start time of the record.  Year=19xx */
	 MONTH,
	 DAY,
	 HOUR,
	 MINUT,
	 SEC,
	 TXPOW,	/* transmitted power (kW) */
	 NAVE,	/* number of times the pulse sequence was transmitted */
	 ATTEN,	/* attenuation setting of the receiver (0-3) */
	 LAGFR,	/* the lag to the first range (microsecs) (see note 2 below) */
	 SMSEP,	/* the sample separation (microsecs) (see note 2) */
	 ERCOD,	/* error flag (see error definitions) */
	 AGC_STAT,	/* AGC status word */
	 LOPWR_STAT,	/* Low power status word */
	 NBAUD; 		/* number of elements in pulse code */
  int32 NOISE,	/* the noise level determined during the clear freq. search */
        NOISE_MEAN;  /* average noise across frequency band */
  int16 CHANNEL;
  int16 RXRISE;	/* receiver rise time */
  /* The second set of parameters are set by the user */	
  /* These parameters can either be set manually or by a RADLANG program */
  /* There are a total of 24 words in this parameter list */
  int16 INTT,	/* the integration period */
	TXPL,	/* the pulse length (in micro seconds) */
	MPINC,	/* the basic lag separation (in microseconds) */
	MPPUL,	/* the number of pulses in the pulse pattern */
	MPLGS,	/* the number of lags in the pulse pattern (note 1 below) */
	NRANG,	/* the number of range gates */
	FRANG,	/* the distance to the first range (see note 2 below) */
	RSEP,	/* the range separation (in km) */
	BMNUM,	/* beam number */				
	XCF,	/* flag indicating if cross-correlations were being done */
	TFREQ,	/* the transmitted frequency (in KHz) */
	SCAN;	/* flag indicating the scan mode  */
	int32 MXPWR,	/*	Maximum power allowed (see note 3) */
	      LVMAX;	/*  Maximum noise level allowed (see note 3) */
	/* user defined variables */
	int32 usr_resL1,
              usr_resL2;
	int16 CP,
	      usr_resS1,
	      usr_resS2,
	      usr_resS3;
};

/*	The total length of the parameter block is 48 words */
/*  This does not include the length of the comment buffer,
	the pulse table and the lag table */

/*
	1.The number of lags in the pulse pattern is the true number of
	  lags which are present in the table LAG_TABLE.  It is NOT the
	  value of the maximum lag.  If the maximum lag is 33 but only
	  22 out of the 33 lags are actually calculated then MPLGS is 22.

	2.The user set the first range gate by specifying FRANG in km.
	  The system uses this value to set the lag to the first range
	  in microseconds.
    	
	  Similarly, the user sets the range separation by specifying RSEP
	  in km.The system uses this value to set SMSEP in terms of microsecs.

	3.During the gain setting routine, the system will attempt to add
	  enough attenuation so that the maximum reflected power is less than
	  MXPWR.  If this is not possible, the system will set the error code
	  (ERCOD) to indicate the receiver overload condition.
	
	  During the clear frequency search, the system will find the clearest
 	  frequency in the range specified.  The Noise level determined for
	  that frequency will be stored in the parameter NOISE.  If NOISE is
	  greater than LVMAX, the error code will be set to indicate the
	  no clear frequency condition.
*/

struct rawdata
	{
	struct radar_parms PARMS;
	int16 PULSE_PATTERN[ORIG_PULSE_PAT_LEN];
	int16 LAG_TABLE[2][ORIG_LAG_TAB_LEN];
	char COMBF[ORIG_COMBF_SIZE];
	float pwr0[ORIG_MAX_RANGE];
	float acfd[ORIG_MAX_RANGE][ORIG_LAG_TAB_LEN][2];
	float xcfd[ORIG_MAX_RANGE][ORIG_LAG_TAB_LEN][2];
	};


















