; noiseacf.pro
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
; FitACFNoiseACF 


; ---------------------------------------------------------------
; $Log: noiseacf.pro,v $
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
;       FitACFNoiseACF
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        omega=FitACFNoiseACF(mnpwr,prm,pwrd,nbad,badsmp,acfd,ncfd)
;
;
;
;-----------------------------------------------------------------
;



function FitACFNoiseACF,mnpwr,prm,pwrd,nbad,badsmp,acfd,ncfd

  LAG_SIZE=48
  PLIMC=1.6D

  ncfd=dcomplexarr(LAG_SIZE)
  np=intarr(LAG_SIZE)
 
  plim=PLIMC*mnpwr

  q=where((pwrd lt plim) and $
          ((abs(double(acfd[*,0]))+abs(imaginary(acfd[*,0]))) gt 0.0D) and $
          ((abs(double(acfd[*,0]))) lt plim) and $
          ((abs(imaginary(acfd[*,0]))) lt plim) ,c)

  for i=0,c-1 do begin

     FitACFRngOverLap,prm.mppul,prm.mpinc,prm.smsep, $
                      prm.pulse,roverlap

     FitACFCkRng,q[i],prm.mplgs,prm.mpinc,prm.smsep,prm.lag, $
                        nbad,badsmp,bdlag

     FitACFLagOverLap,q[i],prm.nave,prm.nrang, $
                   prm.mppul,prm.mplgs,roverlap, $
                   prm.pwr0,prm.lag, $
                   prm.pulse,bdlag



     ql=where((bdlag[0:prm.mplgs-1] eq 0) and $
              (abs(double(acfd[q[i],0:prm.mplgs-1])) lt plim) and $
              (abs(imaginary(acfd[q[i],0:prm.mplgs-1])) lt plim),cl)
     if (cl gt 0) then begin
       ncfd[ql]=ncfd[ql]+acfd[q[i],ql]
       np[ql]=np[ql]+1
     endif
  endfor
  
   if (np[0] le 2) then begin
      ncfd[*]=dcomplex(0.0D,0.0D)
      return, 0.0D
   endif

   ql=where(np[0:prm.mplgs-1] gt 2,cl)

   if (cl gt 0) then ncfd[ql]=ncfd[ql]/np[ql]
 
   ql=where(np[0:prm.mplgs-1] le 2,cl)
 
   if (cl gt 0) then ncfd[ql]=dcomplex(0.0D,0.0D)
 
   P=total(abs(ncfd[1:prm.mplgs-1]))

   P=P/(prm.mplgs-1)
  
  return, P
end

