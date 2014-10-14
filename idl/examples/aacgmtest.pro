; aacgmtest.pro
; =============
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
; A Simply demonstration program for testing the AACGM library


pro aacgmtest


; Open a set of coefficients and load them.
; 
; NOTE: This step can be left out as the AACGM routines will default
; to the year 2000 coefficients

;  openr,unit,'2000.dat',/get_lun,/stdio
;  c=AACGMLoadCoef(unit)
;  free_lun,unit

  glat=85.5
  glon=70.5
  hgt=300.0

  mlat=0.0
  mlon=0.0
  r=0.0

  s=AACGMConvert(glat,glon,hgt,mlat,mlon,r)
  print, 'Geomagnetic Coordinates=',mlat,mlon

  s=AACGMConvert(mlat,mlon,hgt,glat,glon,r,/geo)
  print, 'Inverse Geographic Coordinates=',glat,glon

  yrsec=TimeYMDHMSToYrSec(2002,08,30,10,30,0)

  mlt=AACGMConvertMLT(2002,yrsec,0.0)

  print, 'MLT=',mlt

end


