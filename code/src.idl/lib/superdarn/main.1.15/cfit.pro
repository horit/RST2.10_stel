; cfit.pro
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
; Public Functions:
; -----------------
;
; CFitRead            
; CFitWrite          
; CFitSeek            
;
; ---------------------------------------------------------------
; $Log: cfit.pro,v $
; Revision 1.4  2006/08/17 14:22:59  code
; Added support for beam azimuth.
;
; Revision 1.3  2006/02/06 20:03:27  barnes
; Changed micro-seconds to long integer.
;
; Revision 1.2  2004/08/10 00:35:08  barnes
; Changed the atme in CFitSeek to a keyword parameter.
;
; Revision 1.1  2004/08/07 22:50:10  barnes
; Initial revision
;
; ---------------------------------------------------------------
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CFitRead
;
; PURPOSE:
;       Read a record from a cfit file.
;       
;
; CALLING SEQUENCE:
;       status = CFitRead(unit,prm,fit)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and fit are
;       populated accordingly.
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;



function CFitRead,unit,prm,fit

  ON_IOERROR, iofail

  time=0.0D
  stid=0
  scan=0
  cp=0
  bmnum=0
  bmazm=0.0
  channel=0
  inttsc=0
  inttus=0L
  frang=0
  rsep=0
  rxrise=0
  tfreq=0
  noise=0L
  atten=0
  nave=0 
  nrang=0
 
  RadarMakeRadarPrm,prm
  FitMakeFitData,fit

  readu, unit,time,stid,scan,cp,bmnum,bmazm,channel,inttsc,inttus,frang,rsep,$
         rxrise,tfreq,noise,atten,nave,nrang
 
  
  status=TimeEpochToYMDHMS(yr,mo,dy,hr,mt,sc,time)
 
  s=8+4+2*14+1

  prm.time.yr=yr
  prm.time.mo=mo
  prm.time.dy=dy
  prm.time.hr=hr
  prm.time.mt=mt
  prm.time.sc=floor(sc)
  prm.time.us=1.0e6*(sc-floor(sc)) 
  prm.stid=stid
  prm.scan=scan
  prm.cp=cp
  prm.bmnum=bmnum
  prm.bmazm=bmazm
  prm.channel=channel
  prm.intt.sc=inttsc
  prm.intt.us=inttus
  prm.frang=frang
  prm.rsep=rsep
  prm.rxrise=rxrise
  prm.tfreq=tfreq
  prm.noise.search=noise
  prm.atten=atten
  prm.nave=nave
  prm.nrang=nrang
  prm.combf='cfit'
 
  fit.qflg[*]=0
  fit.gflg[*]=0
  fit.pwr0[*]=0
  fit.p_l[*]=0
  fit.p_l_e[*]=0
  fit.w_l[*]=0
  fit.w_l_e[*]=0
  fit.v[*]=0
  fit.v_e[*]=0


  num=0B

  readu,unit,num

  if (num eq 0) then return, s+1

 
  rng=bytarr(num)
  readu,unit,rng
 
  s+=num+1
 
  gsct=0B
  p_0=0.0
  p_0_e=0.0
  v=0.0
  p_l=0.0 
  w_l=0.0
  v_e=0.0
  p_l_e=0.0
  w_l_e=0.0

  
  for i=0,num-1 do begin
     readu,unit,gsct,p_0,p_0_e,v,p_l,w_l,v_e, p_l_e,w_l_e
     
     fit.qflg[rng[i]]=1
     fit.gflg[rng[i]]=gsct
     fit.pwr0[rng[i]]=p_0
     fit.v[rng[i]]=v
     fit.p_l[rng[i]]=p_l
     fit.w_l[rng[i]]=w_l
     fit.v_e[rng[i]]=v_e
     fit.p_l_e[rng[i]]=p_l_e
     fit.w_l_e[rng[i]]=w_l_e
     s+=1+8*4
  endfor
  
  return, s

  iofail:
    return,-1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CFitWrite
;
; PURPOSE:
;       Writes a record to a cfit file.
;       
;
; CALLING SEQUENCE:
;       status = CFitWrite(unit,prm,fit,minpwr=minpower)
;
;       This function writes a single record from the open file with
;       logical unit number, unit.
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function CFitWrite,unit,prm,fit,minpwr=minpwr

  ON_IOERROR, iofail

  s=8+4+2*14+1

  if (KEYWORD_SET(minpwr) eq 0) then minpwr=3.0
 
  time=TimeYMDHMSToEpoch(prm.time.yr,prm.time.mo,prm.time.dy, $
                           prm.time.hr,prm.time.mt, $
                           prm.time.sc)

  time=time+prm.time.us/1e6
  stid=fix(prm.stid)
  scan=fix(prm.scan)
  cp=fix(prm.cp)
  bmnum=fix(prm.bmnum)
  bmazm=float(prm.bmazm)
  channel=fix(prm.channel)
  inttsc=fix(prm.intt.sc)
  inttus=long(prm.intt.us)
  frang=fix(prm.frang)
  rsep=fix(prm.rsep)
  rxrise=fix(prm.rxrise)
  tfreq=fix(prm.tfreq)
  noise=long(floor(prm.noise.search))
  atten=fix(prm.atten)
  nave=fix(prm.nave)
  nrang=fix(prm.nrang)
  

  writeu, unit,time,stid,scan,cp,bmnum,bmazm,channel,inttsc,inttus,frang,rsep,$
          rxrise,tfreq,noise,atten,nave,nrang
 
  rng=where( (fit.qflg[0:prm.nrang-1] eq 1) and  $
             (fit.pwr0[0:prm.nrang-1] gt minpwr),num)
  
  num=byte(num)

  writeu,unit,num
 
  if (num eq 0) then return, s+1

  rng=byte(rng)

  writeu,unit,rng
 
  s+=num+1
 
  p_0_e=0.0
  
  for i=0,num-1 do begin
   
     gsct=byte(fit.gflg[rng[i]])
     p_0=float(fit.pwr0[rng[i]])
     v=float(fit.v[rng[i]])
     p_l=float(fit.p_l[rng[i]])
     w_l=float(fit.w_l[rng[i]])
     v_e=float(fit.v_e[rng[i]])
     p_l_e=float(fit.p_l_e[rng[i]])
     w_l_e=float(fit.w_l_e[rng[i]])

     writeu,unit,gsct,p_0,p_0_e,v,p_l,w_l,v_e, p_l_e,w_l_e
  
     s+=1+8*4
  endfor
  
  return, s

  iofail:
    return,-1

end
 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CFitSeek
;
; PURPOSE:
;       Find a record in a cfit file.
;
;
; CALLING SEQUENCE:
;       status = CFitSeek(unit,yr,mo,dy,hr,mt,sc,atme=atme)
;
;       This function searchs for a record in a cfit file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function CFitSeek,unit,yr,mo,dy,hr,mt,sc,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
 
  s=CFitRead(unit,prm,fit)
  if (s eq -1) then begin
     point_lun,unit,0
     s=CFitRead(unit,prm,fit)
     if (s eq -1) then return,-1
  endif

  tfile=TimeYMDHMSToEpoch(prm.time.yr,prm.time.mo,prm.time.dy, $
                          prm.time.hr,prm.time.mt, $
                          prm.time.sc+prm.time.us/1.0e6)

  if (tfile gt tval) then point_lun,unit,0 $
  else begin
    stat=fstat(unit)
    fptr=stat.cur_ptr
  endelse

  atme=tfile
  repeat begin
    stat=fstat(unit)
    s=CFitRead(unit,prm,fit)
    if (s eq -1) then break
    tfile=TimeYMDHMSToEpoch(prm.time.yr,prm.time.mo,prm.time.dy, $
                            prm.time.hr,prm.time.mt,$
                            prm.time.sc+prm.time.us/1.0e6)
 
    if (tval gt tfile) then begin
      fptr=stat.cur_ptr
      atme=tfile
    endif
  endrep until (tval le tfile)
  if (tval gt tfile) then return,-1
  point_lun,unit,fptr
  return,0
end
