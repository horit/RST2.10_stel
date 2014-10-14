; fitacf.pro
; ==========
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
; FitACFRngOverlap
; FitACFLagOverlap
; FitACFBadlags
; FitACFCkRng
;
; ---------------------------------------------------------------
; $Log: fitacf.pro,v $
; Revision 1.3  2006/05/10 21:37:01  barnes
; Changed the size of some of the defaults.
;
; Revision 1.2  2005/07/01 00:33:49  barnes
; Fixed pwr_ratio.
;
; Revision 1.1  2004/08/18 23:09:00  barnes
; Initial revision
;
; ---------------------------------------------------------------
;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFRngOverlap
;
; PURPOSE:
;       Find overlapping ranges for the bad lag determination
;       
;
; CALLING SEQUENCE:
;       FitACFRngOverlap,mppul,mpinc,smsep,ptab,roverlap
;
;       Find overlapping ranges for the bad lag determination

;
;
;-----------------------------------------------------------------
;



pro FitACFRngOverlap,mppul,mpinc,smsep,ptab,roverlap

  PULSE_SIZE=64 

  roverlap=lonarr(PULSE_SIZE,PULSE_SIZE)
  tau=mpinc/smsep

  for ckpulse=0,mppul-1 do begin
     for pulse=0,mppul-1 do begin
        diffpulse=ptab[ckpulse]-ptab[pulse]
        roverlap[ckpulse,pulse]=diffpulse *tau
    endfor
  endfor
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFLagOverlap
;
; PURPOSE:
;       Find overlapping lags for the bad lag determination
;       
;
; CALLING SEQUENCE:
;       FitACFLagOverLap,range,nave,nrang,mppul,mplgs,roverlap,pwr0,
;                        ltab,ptab,badlag
;
;       Find overlapping lags for the bad lag determination
;
;
;-----------------------------------------------------------------
;



pro FitACFLagOverLap,range,nave,nrang,mppul,mplgs,roverlap,pwr0,$
                     ltab,ptab,badlag

  
  PULSE_SIZE=64

  bad_pulse=lonarr(PULSE_SIZE)
 
  MIN_PWR_RATIO=0.3
  pwr_ratio=long(nave*MIN_PWR_RATIO)


  for ckpulse=0,mppul-1 do begin
     for pulse=0,mppul-1 do begin
        ckrange=roverlap[ckpulse,pulse]+range
        if ((pulse ne ckpulse) and (0 le ckrange) and $
            (ckrange lt nrang)) then begin
           min_pwr=pwr_ratio*pwr0[range]
           if (min_pwr lt pwr0[ckrange]) then bad_pulse[ckpulse]=1
      endif
    endfor
  endfor
  for pulse=0,mppul-1 do begin
     if (bad_pulse[pulse] eq 1) then begin
       for i=0,1 do begin
         for lag=0,mplgs-1 do begin
           if (ltab[lag,i] eq ptab[pulse]) then badlag[lag]=1
         endfor
       endfor
     endif
  endfor


end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFBadLags
;
; PURPOSE:
;       Bad lag determination
;       
;
; CALLING SEQUENCE:
;       FitACFBadlags,mppul,lagfr,smsep,mpinc,txpl,ptab,nbad,badsmp
;
;       Bad lag determination.
;
;
;-----------------------------------------------------------------
;



pro FitACFBadlags,mppul,lagfr,smsep,mpinc,txpl,ptab,nbad,badsmp

   i=-1
   badsmp=lonarr(100)
   ts=long(lagfr)
   sample=0L
   k=0
   t2=0L
   t1=0L
   while (i lt (mppul-1)) do begin
      while ((ts gt t2) and (i lt (mppul-1))) do begin
         i++
         t1=long(ptab[i])*long(mpinc)
         t2=t1+3*txpl/2+100L
     endwhile

      while (ts lt t1) do begin
         sample++
         ts=ts+smsep
     endwhile

     while ((ts ge t1) and (ts le t2)) do begin
         badsmp[k]=sample
         k++
         sample++
         ts=ts+smsep
     endwhile
  endwhile
  nbad=k
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFCkRng
;
; PURPOSE:
;       Check ranges for bad lags
;       
;
; CALLING SEQUENCE:
;       FitACFCkRng,range,mplgs,mpinc,smsep,ltab,nbad,badsmp,badlag,old=old
;
;       Check ranges for bad lags.
;
;
;-----------------------------------------------------------------
;


pro FitACFCkRng,range,mplgs,mpinc,smsep,ltab,nbad,badsmp,badlag,old=old
  
    LAG_SIZE=96

    if (n_elements(badlag) eq 0) then badlag=intarr(LAG_SIZE)
    for i=0,mplgs-1 do begin
     badlag[i]=0
     sam1=ltab[i,0]*(mpinc/smsep)+range
     sam2=ltab[i,1]*(mpinc/smsep)+range
     for j=0,nbad-1 do begin
       if ((sam1 eq badsmp[j]) or (sam2 eq badsmp[j])) then badlag[i]=1
       if (sam2 lt badsmp[j]) then break
     endfor
  endfor
  if ((mplgs eq 17) and (KEYWORD_SET(old))) then badlag[13]=1
end







  

