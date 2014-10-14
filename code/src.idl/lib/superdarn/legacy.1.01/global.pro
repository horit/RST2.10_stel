; aacgmlib.pro
; ============
; Author: K.Baker & R.J.Barnes
; 
; Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
; All rights reserved.
; 
; This material may be used, modified, or reproduced by or for the U.S.
; Government pursuant to the license rights granted under the clauses at DFARS
; 252.227-7013/7014.
; 
; For any other permissions, please contact the Space Department
; Program Office at JHU/APL.
; 
; This Distribution and Disclaimer Statement must be included in all copies of
; "Radar Software Toolkit - SuperDARN Toolkit" (hereinafter "the Program").
; 
; The Program was developed at The Johns Hopkins University/Applied Physics
; Laboratory (JHU/APL) which is the author thereof under the "work made for
; hire" provisions of the copyright law.  
; 
; JHU/APL assumes no obligation to provide support of any kind with regard to
; the Program.  This includes no obligation to provide assistance in using the
; Program or to provide updated versions of the Program.
; 
; THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
; OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
; TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
; HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
; PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
; ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
; SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
; LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
; CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
; PROGRAM."
; 
; 
; 
; 
; 
; 
;
;
; ---------------------------------------------------------------
; $Log: global.pro,v $
; Revision 1.2  2004/08/19 21:10:09  barnes
; Removed some conflicting code.
;
; Revision 1.1  2004/08/19 15:21:23  barnes
; Initial revision
;
; ---------------------------------------------------------------
; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;	This section of code initializes the common blocks and some of
;	the fundamental parameters associated with the radar data
;
;
;
;	initialize the common blocks
;
	common rawdata_com, rawfileptr, raw_data, badlag, $
	                    more_badrange, lags, rd_byte
	common fitdata_com, fitfileptr, fit_data
	common globals, scan_type, mflag
;
;
;       define constants
;
        TRUE = 1
        FALSE = 0
        PARMS_SIZE = 48
        PULSE_PAT_LEN = 16
        LAG_TAB_LEN = 48
        COMBF_SIZE = 80
        MAX_RANGE = 75
        loop = TRUE
        EOF = -1
        COLOR_TABLE = 12
;
;       data declarations
;	
	scan_type = 0
	mflag = 0
;	x = fltarr(17,76)
;	y = fltarr(17,76)
	fitfileptr = 0L
	fitdef, fit_data
	rawfileptr = 0L
	rawdef, raw_data
	badlag = intarr(LAG_TAB_LEN)
	more_badrange = -1
	lags = intarr(LAG_TAB_LEN,2)
	rd_byte = bytarr(58300)
;
;	return to the user
;
	end



