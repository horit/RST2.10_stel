; noisestat.pro
; =============
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
; FitACFNoiseStat


; ---------------------------------------------------------------
; $Log: noisestat.pro,v $
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
;       FitACFNoiseStat
;
; PURPOSE:
;       Unkown.
;       
;
; CALLING SEQUENCE:
;
;        sigma=FitACFNoiseStat(mnpwr,prm,nbad,badsmp,acfd,signal)
;
;
;-----------------------------------------------------------------
;



function FitACFNoiseStat,mnpwr,prm,nbad,badsmp,acfd,signal

  PLIMC=1.6D
  ROOT_3=1.7D

  plim = PLIMC * mnpwr

  q=where((double(acfd[*,0]) le plim) and (double(acfd[*,0]) gt 0.0D),c)

  npt=0
  np0=0
  P=0.0D
  P2=0.0D
  for i=0,c-1 do begin

     FitACFRngOverLap,prm.mppul,prm.mpinc,prm.smsep, $
                      prm.pulse,roverlap


     FitACFCkRng,q[i],prm.mplgs,prm.mpinc,prm.smsep,prm.lag, $
                        nbad,badsmp,bdlag

     FitACFLagOverLap,q[i],prm.nave,prm.nrang, $
                   prm.mppul,prm.mplgs,roverlap, $
                   prm.pwr0,prm.lag, $
                   prm.pulse,bdlag

     np0++
     fluct=double(acfd[q[i],0])/sqrt(prm.nave)

     low_lim=double(acfd[q[i],0])-2.0*fluct

     if (low_lim lt 0) then low_lim=low_lim+fluct

     high_lim=double(acfd[q[i],0])+fluct    
       
     ql=where((bdlag[1:prm.mplgs] eq 0) and $
              (abs(acfd[q[i],1:prm.mplgs-1]) ge low_lim) and $
              (abs(acfd[q[i],1:prm.mplgs-1]) le high_lim),cl)
     if (cl gt 0) then begin
       P=P+total(abs(acfd[q[i],ql+1]))
       P2=P2+total(abs(acfd[q[i],ql+1])^2)
       npt=npt+cl
     endif
 endfor

  if (npt lt 2) then begin
    signal=0
    return, plim/sqrt(double(prm.nave))
  endif
 

  P = P/npt;
  var = (P2 - P*P*npt)/(double(npt-1))

  sigma=0.0D
  if (var gt 0.0D) then sigma=sqrt(var)

  signal=0.0D

  if ((P ge sigma*ROOT_3) and (sigma gt 0.0)) then signal=P
 

  if (P gt sigma) then  return, P
  return, sigma
end
