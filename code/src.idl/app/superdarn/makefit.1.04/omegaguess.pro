; omegaguess.pro
; ==============
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
; FitACFOmegaGuess 


; ---------------------------------------------------------------
; $Log: omegaguess.pro,v $
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
;       FitACFOmegaGuess
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        omega=FitACFOmegaGuess(acf,tau,badlag,phi_res,omega_err,mpinc,mplgs)
;
;
;
;
;-----------------------------------------------------------------
;




function FitACFOmegaGuess,acf,tau,badlag,phi_res,omega_err,mpinc,mplgs

  nave=0
  tau_lim=1.0D
  omega=0.0d
  omega2=0.0d
  sum_W=0.0d
  omega_err=9999.0D
  two_sigma=2*!DPI
  sigma=2*!DPI
  average=0.0D
   while ((tau_lim lt 3) and (nave lt 3)) do begin
     
     for j=1,tau_lim do begin
       for i=0,mplgs-1-j do begin
         if ((badlag[i+j] eq 0) and (badlag[i] eq 0)) then begin
         
           delta_tau=tau[i+j]-tau[i]
           if (delta_tau eq tau_lim) then begin
             delta_phi=phi_res[i+j]-phi_res[i]
             W=(abs(acf[i])+abs(acf[i+j]))/2.0D
             W=W*W
           
             if (delta_phi gt !DPI) then delta_phi=delta_phi - 2*!DPI
             if (delta_phi lt -!DPI) then delta_phi=delta_phi + 2*!DPI
             
             if ((average ne 0) and $
                 (abs(delta_phi-average) gt two_sigma)) then dummy=0.0 $
             else begin
               temp = delta_phi/tau_lim
	       omega = omega + temp*W
	       omega2 = omega2 + W*(temp*temp)
	       sum_W = sum_W + W
	       nave=nave+1              
             endelse
           endif
         endif
       endfor
   endfor

   if ((nave ge 3) and (sigma eq 2*!DPI)) then begin
     average = omega/sum_W
     sigma = ((omega2/sum_W) - average*average)/(nave-1)

     if (sigma gt 0.0D) then sigma=sqrt(sigma) $
     else sigma=0.0D 
     two_sigma = 2.0D*sigma
     omega = 0.0D
     omega2 = 0.0D 
     sum_W = 0.0D
     nave = 0
     tau_lim = 1
   endif else begin
     if (nave ge 3) then begin
       omega = omega/sum_W
       omega = omega/(mpinc*1.0e-6)
       sigma = ((omega2/sum_W) - average*average)/(nave-1)
       if (sigma gt 0.0D) then sigma=sqrt(sigma) $
       else sigma=0.0D 
       omega_err = sigma/(mpinc*1.0e-6)
       return,omega
     endif else tau_lim=tau_lim+1
   endelse
  endwhile

  return, 0.0
end
