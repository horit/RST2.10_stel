; calcphires.pro
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
; FitACFCalcPhiRes


; ---------------------------------------------------------------
; $Log: calcphires.pro,v $
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
;       FitACFCalcPhiRes
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        s=FitACFCalcPhiRes(acf,badlag,phires,mplgs)
;
;
;-----------------------------------------------------------------
;




function FitACFCalcPhiRes, acf,badlag,phires,mplgs
 
  LAG_SIZE=48

  if (n_elements(phires) eq 0) then phires=dblarr(LAG_SIZE)

  q=where(badlag[0:mplgs-1] eq 0,c)

  if (c ne 0) then phires[q]=atan(imaginary(acf[q]),double(acf[q]))
  
  q=where(phires ne 0,nzc)
 
  if (nzc gt 0) then return, 0
  return, -1
end
