; morebadlags.pro
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
; FitACFMoreBadlags


; ---------------------------------------------------------------
; $Log: morebadlags.pro,v $
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
;       FitACFMoreBadlags
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        nump=FitACFMoreBadlags(w, badlag, noise_lev,mplgs,nave)
;
;
;-----------------------------------------------------------------
;




function FitACFMoreBadlags, w, badlag, noise_lev,mplgs,nave

  badflag_1 = 0
  badflag_2 = 0

  fluct0 = w[0]/sqrt(2.0d*nave)
  fluct = w[0] + 2.0d*noise_lev+fluct0
  fluct_old = fluct
  sum_np = 0
  k_old = 0

 
  for k=0,mplgs-1 do begin
    if (badlag[k] eq 0) then begin
    
      if (badflag_2 ne 0) then badlag[k]=7 $
      else begin
       if (w[k] le 1.0) then begin
         badlag[k]=3
         badflag_2=badflag_1
         badflag_1=1
       endif else begin
          badflag_1=0
          if (w[k] gt fluct) then begin
            badlag[k]=5
            if (k lt (mplgs-1)) then begin
               if ((w[k] lt fluct_old) and (w[k+1] gt fluct) and $
                   (w[k+1] lt w[k])) then begin
                 badlag[k_old]=9
                 sum_np=sum_np-1 
                 badlag[k]=0   
               endif
            endif
          endif

          fluct_old=fluct
          fluct=2.0d*noise_lev + w[k] + fluct0
      endelse

      endelse
      if (badlag[k] eq 0) then begin
        sum_np=sum_np+1
        k_old=k
    endif
endif

endfor

  return, sum_np
end


