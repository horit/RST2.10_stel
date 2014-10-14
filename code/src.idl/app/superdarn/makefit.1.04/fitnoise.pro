; fitnoise.pro
; ============
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
; FitACFFitNoise


; ---------------------------------------------------------------
; $Log: fitnoise.pro,v $
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
;       FitACFFitNoise
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;       
;     FitACFFitNoise,ncf,badlag,nbad,badsmp,skynoise,prm,ptr
;
;
;-----------------------------------------------------------------
;



pro FitACFFitNoise,ncf,badlag,nbad,badsmp,skynoise,prm,ptr

  lag_lim = prm.mplgs/2 ; require at least 1/2 of lags be OK

  istat = FitACFFitACF(ncf,0,badlag,nbad,badsmp, $ 
                       lag_lim,prm,0.0,0,0.0,ptr)

  tau = prm.mpinc * 1.0d-6

  if ((istat eq 1) and (ptr.v eq 0.0)) then ptr.v = 1.0
  if ((istat eq 1) and (abs(ptr.v_err/ptr.v) < 0.05)) then begin
    if (ptr.w_l_err le ptr.w_s_err) then begin
      if (ptr.p_s gt skynoise) then ptr.p_s = skynoise
      A = exp(ptr.p_s)
      for j=0,prm.mplgs-1 do begin
        t = (prm.lag[j,1] - prm.lag[j,0])*tau
        ncf[j]=dcomplex(A*exp(-ptr.w_l*t)*cos(ptr.v*t), $
                        A*exp(-ptr.w_l*t)*sin(ptr.v*t))
      endfor
    endif else begin
      if (ptr.p_s gt skynoise) then ptr.p_s = skynoise
      A = exp(ptr.p_s)
      for j=0,prm.mplgs-1 do begin
        t = (prm.lag[1,j] - prm.lag[0,j])*tau
        ncf[j]=dcomplex(A*exp(-(ptr.w_s*t)*(ptr.w_s*t))*cos(ptr.v*t), $
                        A*exp(-(ptr.w_s*t)*(ptr.w_s*t))*sin(ptr.v*t))
      endfor
    endelse
  endif else ncf=0.0d;
end
