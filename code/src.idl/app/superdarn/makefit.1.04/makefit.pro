; fitacfwrapper.pro
; =================
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
; FitACFStart 
; FitACF

; ---------------------------------------------------------------
; $Log: makefit.pro,v $
; Revision 1.2  2005/07/06 20:56:00  barnes
; Fixed bug in not writing out the index file.
;
; Revision 1.1  2005/07/01 00:57:20  barnes
; Initial revision
;
; Revision 1.1  2005/07/01 00:31:17  barnes
; Initial revision
;
; ---------------------------------------------------------------




@fitelv
@fitrange
@removenoise
@noiseacf
@noisestat
@morebadlags
@calcphires
@omegaguess
@dophasefit
@fitacfal
@fitnoise
@elevation
@elevgoose
@groundscatter
@dofit
@fitacfwrapper

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       MakeFit
;
; PURPOSE:
;       Initialize a structure to store parameters used by
;       the fitacf algorithm.
;       
;
; CALLING SEQUENCE:
;
;        MakeFit,input,output,index,/NEW
;
;
;        The filename of the dat or rawacf file is given by input.
;        The filename of the fit or fitacf file is given by output.
;        The filename of the optional index file is given by index.
;        If the keyword /NEW is set, then FitACF will assume that
;        the input files are in rawacf format and the output should
;        be in fitacf format.
;
;-----------------------------------------------------------------
;




pro MakeFit,input,output,index,new=new

  s=size(input)

  if (s[1] ne 7) then begin
    print, 'You must provide an input filename'
    stop
  endif

  s=size(output)

  if (s[1] ne 7) then begin
    print, 'You must provide an output filename'
    stop
  endif

  s=size(index)
  if (s[1] eq 7) then iflg=1 $
  else iflg=0

  
 

; Open the data table, this should be defined in the environment
; variable SD_RADAR

  fname=getenv('SD_RADAR')
  openr,inp,fname,/get_lun

; Load the data tables

  network=RadarLoad(inp)
  free_lun,inp

; Load up the hardware data, the environment variable SD_HDWPATH
; should point to the directory containing the hardware files


  s=RadarLoadHardware(network,path=getenv('SD_HDWPATH'))

  ; Open input file for reading


  n=0
  if KEYWORD_SET(new) then n=1
  
  if (n ne 0) then begin 
   
    inp=RawOpen(input,/read)

    out=FitOpen(output,/write)

    s=RawRead(inp,prm,raw)
  endif else begin  
    rawfp=OldRawOpen(input)
  
    openw,out,output,bufsize=0,/get_lun,/stdio
    if (iflg ne 0) then openw,inx,index,bufsize=0,/get_lun,/stdio

    s=OldRawRead(rawfp,prm,raw)

    irec=1L 
    drec=2L
    dnum=0L

  endelse


  if (s eq -1) then begin
     print, 'Error reading file'
     stop
  endif
  
  radar=RadarGetRadar(network,prm.stid)
 
   if (n_tags(radar) eq 0) then begin
     print, 'Could not get radar information'
     stop
  endif


  site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,prm.time.dy, $
                          prm.time.hr,prm.time.mt,prm.time.sc)

  if (n_tags(site) eq 0) then begin
     print, 'Could not get site information'
     stop
  endif
 
  FitACFStart,site,prm.time.yr,fitblk




  while s ne -1 do begin
   print, prm.time.hr,prm.time.mt,prm.time.sc

   s=FitACF(prm,raw,fitblk,fit)

   if (n ne 0) then s=FitWrite(out,prm,fit) $
   else begin
     tprm=prm
     if (drec eq 2) then s=OldFitWriteHeader(out,prm,fit)
     if ((iflg ne 0) and (irec eq 1)) then s=OldFitInxWriteHeader(inx,prm)
     dnum=OldFitWrite(out,prm,fit)
     if (iflg ne 0) then s=OldFitInxWrite(inx,drec,dnum,prm) 
     drec+=dnum
     irec++
   endelse



   if (n ne 0) then s=RawRead(inp,prm,raw) $
   else s=OldRawRead(rawfp,prm,raw)


  endwhile

   if (iflg ne 0) then begin
     s=OldFitInxClose(inx,tprm,irec-1)
     free_lun,inx
   endif

   
   free_lun,inp
   free_lun,out
   


end
