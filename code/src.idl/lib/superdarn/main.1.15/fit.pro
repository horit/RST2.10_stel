; fit.pro
; =======
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
;
; PublicFunctions
; ---------------
;
; FitMakeFitData  
; FitNativeRead        
; FitNativeWrite        
; FitMakeFitInx         
; FitLoadInx          
; FitGetTime          
; FitNativeSeek     
; FitNativePoint   
; FitExternalRead      
; FitExternalWrite      
; FitExternalSeek 
; FitExternalPoint     
; FitRead               
; FitWrite             
; FitSeek            
; FitOpen              
; FitPoint
;
; ---------------------------------------------------------------
; $Log: fit.pro,v $
; Revision 1.9  2007/02/05 22:22:53  code
; Fixed the lag table.
;
; Revision 1.8  2006/09/14 19:30:17  code
; Fixed bug in decoding time.
;
; Revision 1.7  2006/07/13 15:18:09  code
; Fixed bugs in the native read routines.
;
; Revision 1.6  2006/07/12 21:17:28  code
; Added origin flag.
;
; Revision 1.5  2006/02/06 20:03:27  barnes
; Changed micro-seconds to long integer.
;
; Revision 1.4  2004/08/27 22:18:57  barnes
; Fixed bug in the Point family of functions.
;
; Revision 1.3  2004/08/27 19:35:15  barnes
; Added functions for setting the file pointer.
;
; Revision 1.2  2004/08/09 19:05:22  barnes
; Minor bug fixes to select the right code to compile.
;
; Revision 1.1  2004/08/03 22:59:28  barnes
; Initial revision
;
; ---------------------------------------------------------------
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitMakeFitData
;
; PURPOSE:
;       Create a structure to store fitACF data.
;       
;
; CALLING SEQUENCE:
;       FitMakeFitData,fit
;
;       This procedure creates a structure to store the fitACF data,
;       the structure is returns in fitdata.
;
;
;-----------------------------------------------------------------
;


pro FitMakeFitData,fit

  MAX_RANGE=300

  fit={FitData, $
         revision: {rlstr, major: 0L, minor: 0L}, $ 
         noise: {nfstr, sky: 0.0, lag0: 0.0, vel: 0.0}, $
         pwr0: fltarr(MAX_RANGE), $
         nlag: intarr(MAX_RANGE), $
         qflg: bytarr(MAX_RANGE), $
         gflg: bytarr(MAX_RANGE), $
         p_l:  fltarr(MAX_RANGE), $ 
         p_l_e: fltarr(MAX_RANGE), $
         p_s: fltarr(MAX_RANGE), $
         p_s_e: fltarr(MAX_RANGE), $
         v: fltarr(MAX_RANGE), $
         v_e: fltarr(MAX_RANGE), $
         w_l: fltarr(MAX_RANGE), $
         w_l_e: fltarr(MAX_RANGE), $
         w_s: fltarr(MAX_RANGE), $
         w_s_e: fltarr(MAX_RANGE), $
         sd_l: fltarr(MAX_RANGE), $
         sd_s: fltarr(MAX_RANGE), $
         sd_phi: fltarr(MAX_RANGE), $
         x_qflg: bytarr(MAX_RANGE), $
         x_gflg: bytarr(MAX_RANGE), $
         x_p_l: fltarr(MAX_RANGE), $
         x_p_l_e: fltarr(MAX_RANGE), $
         x_p_s: fltarr(MAX_RANGE), $
         x_p_s_e: fltarr(MAX_RANGE), $
         x_v: fltarr(MAX_RANGE), $
         x_v_e: fltarr(MAX_RANGE), $
         x_w_l: fltarr(MAX_RANGE), $
         x_w_l_e: fltarr(MAX_RANGE), $
         x_w_s: fltarr(MAX_RANGE), $
         x_w_s_e: fltarr(MAX_RANGE), $
         phi0: fltarr(MAX_RANGE), $
         phi0_e: fltarr(MAX_RANGE), $
         elv: fltarr(MAX_RANGE), $
         elv_low: fltarr(MAX_RANGE), $
         elv_high: fltarr(MAX_RANGE), $
         x_sd_l: fltarr(MAX_RANGE), $
         x_sd_s: fltarr(MAX_RANGE), $
         x_sd_phi: fltarr(MAX_RANGE) $

      }

end
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitNativeRead
;
; PURPOSE:
;       Read a record from a fitacf file.
;       
;
; CALLING SEQUENCE:
;       status = FitNativeRead(unit,prm,fit)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and fit are
;       populated accordingly.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitNativeRead,unit,prm,fit

  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s
  
  RadarMakeRadarPrm,prm
  FitMakeFitData,fit
  
  sclname=['radar.revision.major','radar.revision.minor', $
           'origin.code','origin.time','origin.command','cp','stid', $
           'time.yr','time.mo','time.dy','time.hr','time.mt','time.sc', $
           'time.us','txpow','nave','atten','lagfr','smsep','ercod', $
           'stat.agc','stat.lopwr','noise.search','noise.mean','channel', $
           'bmnum','bmazm','scan','offset','rxrise','intt.sc','intt.us','txpl', $
           'mpinc','mppul','mplgs','nrang','frang','rsep','xcf','tfreq', $
           'mxpwr','lvmax','fitacf.revision.major','fitacf.revision.minor', $
           'combf','noise.sky','noise.lag0','noise.vel']

  scltype=[1,1,1,9,9,2,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,2,4,4,2,2,4,2,2,2,2,3, $
           2,2,2,2,2,2,2,2,2,3,3,3,3,9,4,4,4]
  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['ptab','ltab','slist','pwr0','nlag','qflg','gflg', $
           'p_l','p_l_e','p_s','p_s_e','v','v_e','w_l','w_l_e','w_s', $
           'w_s_e','sd_l','sd_s','sd_phi', $
           'x_qflg','x_gflg','x_p_l','x_p_l_e','x_p_s','x_p_s_e','x_v', $
           'x_v_e','x_w_l','x_w_l_e','x_w_s','x_w_s_e','phi0','phi0_e', $
           'elv','elv_low','elv_high','x_sd_l','x_sd_s','x_sd_phi']


  arrtype=[2,2,2,4,2,1,1,4,4,4,4,4,4,4,4,4,4,4,4,4,1,1,4,4,4,4,4, $
           4,4,4,4,4,4,4,4,4,4,4,4,4]

  arrid=intarr(n_elements(arrname))  
  arrid[*]=-1
  
  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif

  if (n_elements(arrvec) ne 0) then begin
    for n=0,n_elements(arrname)-1 do $
      arrid[n]=DataMapFindArray(arrname[n],arrtype[n],arrvec)
  endif
   
  q=where(sclid eq -1,count)

  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2
  endif
 
  q=where(arrid[[0,1,3]] eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -2  
  endif

  ; populate the structures

  prm.revision.major=*(sclvec[sclid[0]].ptr)
  prm.revision.minor=*(sclvec[sclid[1]].ptr)
  prm.origin.code=*(sclvec[sclid[2]].ptr)
  prm.origin.time=*(sclvec[sclid[3]].ptr)
  prm.origin.command=*(sclvec[sclid[4]].ptr)
  prm.cp=*(sclvec[sclid[5]].ptr)
  prm.stid=*(sclvec[sclid[6]].ptr)
  prm.time.yr=*(sclvec[sclid[7]].ptr)
  prm.time.mo=*(sclvec[sclid[8]].ptr)
  prm.time.dy=*(sclvec[sclid[9]].ptr)
  prm.time.hr=*(sclvec[sclid[10]].ptr)
  prm.time.mt=*(sclvec[sclid[11]].ptr)
  prm.time.sc=*(sclvec[sclid[12]].ptr)
  prm.time.us=*(sclvec[sclid[13]].ptr)
  prm.txpow=*(sclvec[sclid[14]].ptr)
  prm.nave=*(sclvec[sclid[15]].ptr)
  prm.atten=*(sclvec[sclid[16]].ptr)
  prm.lagfr=*(sclvec[sclid[17]].ptr)
  prm.smsep=*(sclvec[sclid[18]].ptr)
  prm.ercod=*(sclvec[sclid[19]].ptr)
  prm.stat.agc=*(sclvec[sclid[20]].ptr)
  prm.stat.lopwr=*(sclvec[sclid[21]].ptr)
  prm.noise.search=*(sclvec[sclid[22]].ptr)
  prm.noise.mean=*(sclvec[sclid[23]].ptr)
  prm.channel=*(sclvec[sclid[24]].ptr)
  prm.bmnum=*(sclvec[sclid[25]].ptr)
  prm.bmazm=*(sclvec[sclid[26]].ptr)
  prm.scan=*(sclvec[sclid[27]].ptr)
  prm.offset=*(sclvec[sclid[28]].ptr)
  prm.rxrise=*(sclvec[sclid[29]].ptr)
  prm.intt.sc=*(sclvec[sclid[30]].ptr)
  prm.intt.us=*(sclvec[sclid[31]].ptr)
  prm.txpl=*(sclvec[sclid[32]].ptr)
  prm.mpinc=*(sclvec[sclid[33]].ptr)
  prm.mppul=*(sclvec[sclid[34]].ptr)
  prm.mplgs=*(sclvec[sclid[35]].ptr)
  prm.nrang=*(sclvec[sclid[36]].ptr)
  prm.frang=*(sclvec[sclid[37]].ptr)
  prm.rsep=*(sclvec[sclid[38]].ptr)
  prm.xcf=*(sclvec[sclid[39]].ptr)
  prm.tfreq=*(sclvec[sclid[40]].ptr)
  prm.mxpwr=*(sclvec[sclid[41]].ptr)
  prm.lvmax=*(sclvec[sclid[42]].ptr)
  if (prm.mppul gt 0) then prm.pulse[0:prm.mppul-1]=*(arrvec[arrid[0]].ptr)
  if (prm.mplgs gt 0) then $
     prm.lag[0:prm.mplgs,*]=(*(arrvec[arrid[1]].ptr))[*,*]
  prm.combf=*(sclvec[sclid[45]].ptr)

  fit.revision.major=*(sclvec[sclid[43]].ptr)
  fit.revision.minor=*(sclvec[sclid[44]].ptr)
  fit.noise.sky=*(sclvec[sclid[46]].ptr)
  fit.noise.lag0=*(sclvec[sclid[47]].ptr)
  fit.noise.vel=*(sclvec[sclid[48]].ptr)

  if (prm.nrang gt 0) then fit.pwr0[0:prm.nrang-1]=*(arrvec[arrid[3]].ptr)

  if (arrid[2] eq -1) then begin
    st=DataMapFreeScalar(sclvec)
    st=DataMapFreeArray(arrvec)
    return, s
  endif

  if ~(ptr_valid(arrvec[arrid[2]].ptr)) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return, s
  endif

  slist=*(arrvec[arrid[2]].ptr)
  if (n_elements(slist) eq 0) then return,s

  fit.nlag[slist]= (*(arrvec[arrid[4]].ptr))[*]
  fit.qflg[slist]= (*(arrvec[arrid[5]].ptr))[*]
  fit.gflg[slist]= (*(arrvec[arrid[6]].ptr))[*]
  fit.p_l[slist]= (*(arrvec[arrid[7]].ptr))[*]
  fit.p_l_e[slist]= (*(arrvec[arrid[8]].ptr))[*]
  fit.p_s[slist]= (*(arrvec[arrid[9]].ptr))[*]
  fit.p_s_e[slist]= (*(arrvec[arrid[10]].ptr))[*]
  fit.v[slist]= (*(arrvec[arrid[11]].ptr))[*]
  fit.v_e[slist]= (*(arrvec[arrid[12]].ptr))[*]
  fit.w_l[slist]= (*(arrvec[arrid[13]].ptr))[*]
  fit.w_l_e[slist]= (*(arrvec[arrid[14]].ptr))[*]
  fit.w_s[slist]= (*(arrvec[arrid[15]].ptr))[*]
  fit.w_s_e[slist]= (*(arrvec[arrid[16]].ptr))[*]
  fit.sd_l[slist]= (*(arrvec[arrid[17]].ptr))[*]
  fit.sd_s[slist]= (*(arrvec[arrid[18]].ptr))[*]
  fit.sd_phi[slist]= (*(arrvec[arrid[19]].ptr))[*]
      
  if (prm.xcf ne 0) and (arrid[20] ne -1) then begin
      fit.x_qflg[slist]= (*(arrvec[arrid[20]].ptr))[*]
      fit.x_gflg[slist]= (*(arrvec[arrid[21]].ptr))[*]
      fit.x_p_l[slist]= (*(arrvec[arrid[22]].ptr))[*]
      fit.x_p_l_e[slist]= (*(arrvec[arrid[23]].ptr))[*]
      fit.x_p_s[slist]= (*(arrvec[arrid[24]].ptr))[*]
      fit.x_p_s_e[slist]= (*(arrvec[arrid[25]].ptr))[*]
      fit.x_v[slist]= (*(arrvec[arrid[26]].ptr))[*]
      fit.x_v_e[slist]= (*(arrvec[arrid[27]].ptr))[*]
      fit.x_w_l[slist]= (*(arrvec[arrid[28]].ptr))[*]
      fit.x_w_l_e[slist]= (*(arrvec[arrid[29]].ptr))[*]
      fit.x_w_s[slist]= (*(arrvec[arrid[30]].ptr))[*]
      fit.x_w_s_e[slist]= (*(arrvec[arrid[31]].ptr))[*]
      fit.phi0[slist]= (*(arrvec[arrid[32]].ptr))[*]
      fit.phi0_e[slist]= (*(arrvec[arrid[33]].ptr))[*]
      fit.elv[slist]= (*(arrvec[arrid[34]].ptr))[*]
      fit.elv_low[slist]= (*(arrvec[arrid[35]].ptr))[*]
      fit.elv_high[slist]= (*(arrvec[arrid[36]].ptr))[*]
      fit.x_sd_l[slist]= (*(arrvec[arrid[37]].ptr))[*]
      fit.x_sd_s[slist]= (*(arrvec[arrid[38]].ptr))[*]
      fit.x_sd_phi[slist]= (*(arrvec[arrid[39]].ptr))[*]
  endif
  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitNativeWrite
;
; PURPOSE:
;       Write a record from a fitacf file.
;       
;
; CALLING SEQUENCE:
;       status = FitNativeWrite(unit,prm,fit)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitNativeWrite,unit,prm,fit

  s=DataMapMakeScalar('radar.revision.major',prm.revision.major,sclvec,/new)
  s=DataMapMakeScalar('radar.revision.minor',prm.revision.minor,sclvec)
  s=DataMapMakeScalar('origin.code',prm.origin.code,sclvec)
  s=DataMapMakeScalar('origin.time',prm.origin.time,sclvec)
  s=DataMapMakeScalar('origin.command',prm.origin.command,sclvec)
  s=DataMapMakeScalar('cp',prm.cp,sclvec)
  s=DataMapMakeScalar('stid',prm.stid,sclvec)
  s=DataMapMakeScalar('time.yr',prm.time.yr,sclvec)
  s=DataMapMakeScalar('time.mo',prm.time.mo,sclvec)
  s=DataMapMakeScalar('time.dy',prm.time.dy,sclvec)
  s=DataMapMakeScalar('time.hr',prm.time.hr,sclvec)
  s=DataMapMakeScalar('time.mt',prm.time.mt,sclvec)
  s=DataMapMakeScalar('time.sc',prm.time.sc,sclvec)
  s=DataMapMakeScalar('time.us',prm.time.us,sclvec)
  s=DataMapMakeScalar('txpow',prm.txpow,sclvec)
  s=DataMapMakeScalar('nave',prm.nave,sclvec)
  s=DataMapMakeScalar('atten',prm.atten,sclvec)
  s=DataMapMakeScalar('lagfr',prm.lagfr,sclvec)
  s=DataMapMakeScalar('smsep',prm.smsep,sclvec)
  s=DataMapMakeScalar('ercod',prm.ercod,sclvec)
  s=DataMapMakeScalar('stat.agc',prm.stat.agc,sclvec)
  s=DataMapMakeScalar('stat.lopwr',prm.stat.lopwr,sclvec)
  s=DataMapMakeScalar('noise.search',prm.noise.search,sclvec)
  s=DataMapMakeScalar('noise.mean',prm.noise.mean,sclvec)
  s=DataMapMakeScalar('channel',prm.channel,sclvec)
  s=DataMapMakeScalar('bmnum',prm.bmnum,sclvec)
  s=DataMapMakeScalar('bmazm',prm.bmazm,sclvec)
  s=DataMapMakeScalar('scan',prm.scan,sclvec)
  s=DataMapMakeScalar('offset',prm.offset,sclvec)
  s=DataMapMakeScalar('rxrise',prm.rxrise,sclvec)
  s=DataMapMakeScalar('intt.sc',prm.intt.sc,sclvec)
  s=DataMapMakeScalar('intt.us',prm.intt.us,sclvec)
  s=DataMapMakeScalar('txpl',prm.txpl,sclvec)
  s=DataMapMakeScalar('mpinc',prm.mpinc,sclvec)
  s=DataMapMakeScalar('mppul',prm.mppul,sclvec)
  s=DataMapMakeScalar('mplgs',prm.mplgs,sclvec)
  s=DataMapMakeScalar('nrang',prm.nrang,sclvec)
  s=DataMapMakeScalar('frang',prm.frang,sclvec)
  s=DataMapMakeScalar('rsep',prm.rsep,sclvec)
  s=DataMapMakeScalar('xcf',prm.xcf,sclvec)
  s=DataMapMakeScalar('tfreq',prm.tfreq,sclvec)
  s=DataMapMakeScalar('mxpwr',prm.mxpwr,sclvec)
  s=DataMapMakeScalar('lvmax',prm.lvmax,sclvec)
  s=DataMapMakeScalar('fitacf.revision.major',fit.revision.major,sclvec)
  s=DataMapMakeScalar('fitacf.revision.minor',fit.revision.minor,sclvec)
  s=DataMapMakeScalar('combf',prm.combf,sclvec)
  s=DataMapMakeScalar('noise.sky',fit.noise.sky,sclvec)
  s=DataMapMakeScalar('noise.lag0',fit.noise.lag0,sclvec)
  s=DataMapMakeScalar('noise.vel',fit.noise.vel,sclvec)

  s=DataMapMakeArray('ptab',prm.pulse[0:prm.mppul-1],arrvec,/new)

  s=DataMapMakeArray('ltab',prm.lag[0:prm.mplgs,*],arrvec)

  s=DataMapMakeArray('pwr0',fit.pwr0[0:prm.nrang-1],arrvec)

  q=fit.qflg[0:prm.nrang-1]+fit.x_qflg[0:prm.nrang-1]

  slist=fix(where(q gt 0,count))

  if (count ne 0) then begin

    s=DataMapMakeArray('slist',slist,arrvec)
    
    s=DataMapMakeArray('nlag',fit.nlag[slist],arrvec)

    s=DataMapMakeArray('qflg',fit.qflg[slist],arrvec)
    s=DataMapMakeArray('gflg',fit.gflg[slist],arrvec)
    s=DataMapMakeArray('p_l',fit.p_l[slist],arrvec)
    s=DataMapMakeArray('p_l_e',fit.p_l_e[slist],arrvec)
    s=DataMapMakeArray('p_s',fit.p_s[slist],arrvec)
    s=DataMapMakeArray('p_s_e',fit.p_s_e[slist],arrvec)
    s=DataMapMakeArray('v',fit.v[slist],arrvec)
    s=DataMapMakeArray('v_e',fit.v_e[slist],arrvec)
    s=DataMapMakeArray('w_l',fit.w_l[slist],arrvec)
    s=DataMapMakeArray('w_l_e',fit.w_l_e[slist],arrvec)
    s=DataMapMakeArray('w_s',fit.w_s[slist],arrvec)
    s=DataMapMakeArray('w_s_e',fit.w_s_e[slist],arrvec)
    s=DataMapMakeArray('sd_l',fit.sd_l[slist],arrvec)
    s=DataMapMakeArray('sd_s',fit.sd_s[slist],arrvec)
    s=DataMapMakeArray('sd_phi',fit.sd_phi[slist],arrvec)

    if (prm.xcf eq 1) then begin
      s=DataMapMakeArray('x_qflg',fit.x_qflg[slist],arrvec)
      s=DataMapMakeArray('x_gflg',fit.x_gflg[slist],arrvec)
      s=DataMapMakeArray('x_p_l',fit.x_p_l[slist],arrvec)
      s=DataMapMakeArray('x_p_l_e',fit.x_p_l_e[slist],arrvec)
      s=DataMapMakeArray('x_p_s',fit.x_p_s[slist],arrvec)
      s=DataMapMakeArray('x_p_s_e',fit.x_p_s_e[slist],arrvec)
      s=DataMapMakeArray('x_v',fit.x_v[slist],arrvec)
      s=DataMapMakeArray('x_v_e',fit.x_v_e[slist],arrvec)
      s=DataMapMakeArray('x_w_l',fit.x_w_l[slist],arrvec)
      s=DataMapMakeArray('x_w_l_e',fit.x_w_l_e[slist],arrvec)
      s=DataMapMakeArray('x_w_s',fit.x_w_s[slist],arrvec)
      s=DataMapMakeArray('x_w_s_e',fit.x_w_s_e[slist],arrvec)
      s=DataMapMakeArray('phi0',fit.phi0[slist],arrvec)
      s=DataMapMakeArray('phi0_e',fit.phi0_e[slist],arrvec)
      s=DataMapMakeArray('elv',fit.elv[slist],arrvec)
      s=DataMapMakeArray('elv_low',fit.elv_low[slist],arrvec)
      s=DataMapMakeArray('elv_high',fit.elv_high[slist],arrvec)
      s=DataMapMakeArray('x_sd_l',fit.x_sd_l[slist],arrvec)
      s=DataMapMakeArray('x_sd_s',fit.x_sd_s[slist],arrvec)
      s=DataMapMakeArray('x_sd_phi',fit.x_sd_phi[slist],arrvec)
    endif
endif

  s=DataMapWrite(unit,sclvec,arrvec)

  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitMakeFitInx
;
; PURPOSE:
;       Create a structure to store fitACF index.
;
;
; CALLING SEQUENCE:
;       FitMakeFitInx,inx
;
;       This procedure creates a structure to store the fitacf index.
;
;
;-----------------------------------------------------------------
;

pro FitMakeFitIndex,inx,time,offset
  inx={FitInx, time: time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitLoadInx
;
; PURPOSE:
;       Read an index to a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitLoadInx(unit,inx)
;
;       This function reads an index to a fitacf file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitLoadInx,unit,inx

ON_IOERROR,iofail
    
  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    FitMakeFitIndex,tinx,time,offset
    if (n_elements(inx) eq 0) then inx=tinx $
    else inx=[inx,tinx] 
  endwhile
 
  return, 0

iofail:
  return,0 


end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitNativeGetTime
;
; PURPOSE:
;       Extract the time from a fitacf record.
;
;
; CALLING SEQUENCE:
;       status = FitNativeGetTime(sclvec)
;
;       This function extracts the time infornation from the scalar
;       vector array generated by reading a DataMap file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitNativeGetTime,sclvec
 
  sclname=['time.yr','time.mo','time.dy','time.hr', $
           'time.mt','time.sc','time.us']

  scltype=[2,2,2,2,2,2,3]

  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  if (n_elements(sclvec) ne 0) then begin
    for n=0,n_elements(sclname)-1 do $
      sclid[n]=DataMapFindScalar(sclname[n],scltype[n],sclvec)
  endif

  q=where(sclid eq -1,count)
  if (count ne 0) then begin
    print,'File is in the wrong format!'
    return, -1
  endif

  yr=*(sclvec[sclid[0]].ptr)
  mo=*(sclvec[sclid[1]].ptr)
  dy=*(sclvec[sclid[2]].ptr)
  hr=*(sclvec[sclid[3]].ptr)
  mt=*(sclvec[sclid[4]].ptr)
  sc=*(sclvec[sclid[5]].ptr)
  us=*(sclvec[sclid[6]].ptr)

  return, TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+us/1.0e6)
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitNativeSeek
;
; PURPOSE:
;       Find a record in a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a fitacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function FitNativeSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (n_elements(inx) ne 0) then begin
   
    rec=0L
    prec=-1L
    num=n_elements(inx)
    stime=inx[0].time
    etime=inx[num-1].time
   
    srec=0L
    erec=num; 
    if (tval lt stime) then begin
      atme=stime
      point_lun,unit,inx[srec].offset
      return, 0
    endif else if (tval gt etime) then begin
      atme=stime
      point_lun,unit,inx[erec-1].offset
      return, -1
    endif 

   repeat begin
      prec=rec;
      rec=srec+fix(((tval-stime)*(erec-srec))/(etime-stime))
      if (inx[rec].time eq tval) then break
      if (inx[rec].time lt tval) then begin
         srec=rec
         stime=inx[rec].time
      endif else begin
         erec=rec
         etime=inx[rec].time
      endelse
    endrep until (prec eq rec)
    atme=inx[rec].time
    point_lun,unit,inx[rec].offset
    return,0
  endif else begin
     s=DataMapRead(unit,sclvec,arrvec)
     fptr=0L
     if (n_elements(sclvec) ne 0) then begin
       tfile=FitNativeGetTime(sclvec)
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tfile gt tval) then point_lun,unit,0 $
       else begin
          stat=fstat(unit)
          fptr=stat.cur_ptr
      endelse
     endif else point_lun,unit,0
     atme=tfile
     repeat begin
       stat=fstat(unit)
       s=DataMapRead(unit,sclvec,arrvec)
       if (s eq -1) then break
       tfile=FitNativeGetTime(sclvec)
      
       st=DataMapFreeScalar(sclvec)
       st=DataMapFreeArray(arrvec)
       if (tval gt tfile) then begin
         fptr=stat.cur_ptr
         atme=tfile
       endif
     endrep until (tval le tfile)
     if (tval gt tfile) then return,-1
     point_lun,unit,fptr
    endelse

 
  return,0 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitNativePoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = FitNativePoint(unit,ptr)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitNativePoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitExternalRead
;
; PURPOSE:
;       Read a record from a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitExternalRead(unit,prm,fit)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and fit are 
;       populated accordingly.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function FitExternalRead,unit,prm,fit,lib=lib
  RadarMakeRadarPrm, prm
  FitMakeFitData, fit

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  s=CALL_EXTERNAL(lib,'FitIDLRead', $
                  unit,prm,fit)

  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitExternalWrite
;
; PURPOSE:
;       Write a record from a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitExternalWrite(unit,prm,fit)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. The structure prm and fit are 
;       populated accordingly.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function FitExternalWrite,unit,prm,fit,lib=lib
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  s=CALL_EXTERNAL(lib,'FitIDLWrite', $
                  unit,prm,fit)

  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitExternalSeek
;
; PURPOSE:
;       Find a record in a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitExternalSeek(unit,yr,mo,dy,hr,mt,sc,atme,inx)
;
;       This function searchs for a record in a fitacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;


function FitExternalSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  atme=0.0D
  yrval=fix(yr)
  moval=fix(mo)
  dyval=fix(dy)
  hrval=fix(hr)
  mtval=fix(mt)
  scval=double(sc)
   
  if (n_elements(inx) ne 0) then begin
    inum=n_elements(inx)
    tval=double(inx.time[*])
    oval=long(inx.offset[*])
    s=CALL_EXTERNAL(lib,'FitIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme,inum, $
                  tval,oval) 
  endif else begin 
    s=CALL_EXTERNAL(lib,'FitIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme) 
  endelse

  return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitExternalPoint
;
; PURPOSE:
;       Sets the file pointer position in a fit file.
;
;
; CALLING SEQUENCE:
;       status = FitExternalPoint(unit,ptr,lib=library)
;
;       This function sets the file pointer in a fit file.
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitExternalPoint,unit,off,lib=lib

 if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')   
 offl=long(off)
 s=CALL_EXTERNAL(lib,'FitIDLPoint', $
                unit,offl) 
 return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitRead
;
; PURPOSE:
;       Read a record from a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitRead(unit,prm,fit,lib=library,/native,/external)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and fit are 
;       populated accordingly.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This determines whether to use the native of call external 
;       reader.
;
;-----------------------------------------------------------------
;


function FitRead,unit,prm,fit,lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=FitExternalRead(unit,prm,fit,lib=lib) $
  else s=FitNativeRead(unit,prm,fit)
  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitWrite
;
; PURPOSE:
;       Write a record to a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitRead(unit,prm,fit,lib=library,/native,/external)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. The structure prm and fit are 
;       populated accordingly.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This determines whether to use the native of call external 
;       reader.
;
;-----------------------------------------------------------------
;


function FitWrite,unit,prm,fit,lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=FitExternalWrite(unit,prm,fit,lib=lib) $
  else s=FitNativeWrite(unit,prm,fit)
  return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitSeek
;
; PURPOSE:
;       Find a record in a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = FitSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=library,
;                        /native,/external)
;
;       This function searchs for a record in a fitacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;
;       This determines whether to use the native of call external 
;       reader.
;
;-----------------------------------------------------------------
;


function FitSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib, $
                         native=native,external=external


  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=FitExternalSeek(unit,yr,mo,dy,hr,mt,sc,inx, $
                                        lib=lib,atme=atme) $
  else s=FitNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme)
  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitOpen
;
; PURPOSE:
;       Open a fitacf file.
;
;
; CALLING SEQUENCE:
;       unit = FitOpen(lib=library,/native,/external,/read,/write,/update)
;
;       This function opens a fitacf file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;       This function is provided as a convenience and opens
;       the fitacf file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function FitOpen,fname,atme=atme,lib=lib, $
                 native=native,external=external, $
                 read=read,write=write,update=update

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then begin
    if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN, /SWAP_IF_BIG_ENDIAN
  endif else begin
    if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/STDIO
    if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN,/STDIO
    if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN,/STDIO
  endelse

  return, unit
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitPoint
;
; PURPOSE:
;       Sets the file pointer position in a fit file.
;
;
; CALLING SEQUENCE:
;       status = FitPoint(fitfp,ptr,lib=library)
;
;       This function sets the file pointer in a fit file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function FitPoint,unit,off,lib=lib

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_FITIDL')   
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then return, FitNativePoint(unit,off) $
  else return, FitExternalPoint(unit,off,lib=lib)

  

end
