; rprm.pro
; ========
; Author: R.J.Barnes
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
; Public Functions
; ----------------
;
; RadarMakeRadarPrm          
;
; ---------------------------------------------------------------
; $Log: rprm.pro,v $
; Revision 1.5  2006/07/12 17:55:47  code
; Added origin code and beam azimuth.
;
; Revision 1.4  2006/05/10 21:37:01  barnes
; Changed the size of some of the defaults.
;
; Revision 1.3  2006/01/06 16:31:52  barnes
; Changed microseconds to long integer.
;
; Revision 1.2  2004/08/09 19:05:22  barnes
; Minor bug fixes to select the right code to compile.
;
; Revision 1.1  2004/08/03 21:01:15  barnes
; Initial revision
;
;
; ---------------------------------------------------------------
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeRadarPrm
;
; PURPOSE:
;       Create a structure to store the radar operating 
;       parameters.
;       
;
; CALLING SEQUENCE:
;       RadarMakeRadarPrm,prm
;
;       This procedure creates a structure to store the rawACF data,
;       the structure is returns in rawdata.
;
;
;-----------------------------------------------------------------
;


pro RadarMakeRadarPrm,prm

  PULSE_SIZE=64
  LAG_SIZE=96
  MAX_RANGE=300

  prm={RadarPrm, $
         revision: {rbstr, major: 0B, minor: 0B}, $ 
         origin: { ogstr, code: 0B, time: ' ', command: ' '}, $
         cp: 0, $
         stid: 0, $
         time: {tmstr, yr:0, $
                      mo:0, $
                      dy:0, $
                      hr:0, $
                      mt:0, $
                      sc:0, $
                      us:0L  $
               }, $
         txpow: 0, $
         nave: 0, $
         atten: 0, $
         lagfr: 0, $
         smsep: 0, $
         ercod: 0, $
         stat: {ststr, agc:0, lopwr:0}, $
         noise: {nsstr, search:0.0, mean:0.0}, $
         channel: 0, $
         bmnum: 0, $
         bmazm: 0.0, $
         scan: 0, $
         rxrise: 0, $
         intt: {itstr, sc:0, us:0L}, $
         txpl: 0, $
         mpinc: 0, $
         mppul: 0, $
         mplgs: 0, $
         nrang: 0, $
         frang: 0, $
         rsep: 0, $
         xcf: 0, $
         tfreq: 0, $
         offset: 0, $
         mxpwr: 0L, $
         lvmax: 0L, $
         pulse: intarr(PULSE_SIZE), $
         lag: intarr(LAG_SIZE,2), $
         combf: '' $
     }
end
