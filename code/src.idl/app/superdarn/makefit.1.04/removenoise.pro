; removenoise.pro
; ===============
; Author: R.J.Barnes - Based on C code by K.Baker
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
; PublicFunctions
; ---------------
;
; FitACFRemoveNoise 


; ---------------------------------------------------------------
; $Log: removenoise.pro,v $
; Revision 1.1  2005/07/01 00:57:20  barnes
; Initial revision
;
; Revision 1.1  2005/07/01 00:31:17  barnes
; Initial revision
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFRemoveNoise
;
; PURPOSE:
;       Subtract the noise ACF from the actual ACF.
;       
;
; CALLING SEQUENCE:
;       FitMakeRemoveNoise,mplgs,range,acf,ncf
;
;       This subtracts the noise ACF from an ACF at a given range.
;       The number of lags is given by mplgs, the range to process
;       is given by range, the array of ACF's is given by acf and
;       the noise ACF is given by ncf.
;
;       The array acf is modified.
;
;
;-----------------------------------------------------------------
;


pro FitACFRemoveNoise,mplgs,range,acf,ncf

  plim=abs(ncf[0])
  pa=abs(acf[range,0])

  if (pa gt plim) then acf[range,*]=acf[range,*]-ncf $
  else acf[range,*]=dcomplex(0.0d,0.0d)
end
