; oldraw.pro
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
;
; Public Functions:
; -----------------
;
; OldRawOpen
; OldRawClose
; OldRawRead
; OldRawWrite
; OldRawWriteHeader
; OldRawRawSeek
; OldRawPoint
;
; Internal Functions:
; -------------------
;
; OldRawMakeOldRawFP
; OldRawMakeOldRawRecHdr
; OldRawMakeOldRawRadarParm
; OldRawNativeOpen
; OldRawTopBit
; OldRawCmpr
; OldRawDcmpr
; OldRawNativeRead
; OldRawNativeWrite
; OldRawNativeWriteHeader
; OldRawNativeSeek
; OldRawNativePoint
; OldRawExternalRead
; OldRawExternalWrite
; OldRawExternalWriteHeader
; OldRawExternalSeek
; OldRawExternalOpen
; OldRawExternalPoint
;
; ---------------------------------------------------------------
; $Log: oldraw.pro,v $
; Revision 1.10  2006/08/17 14:42:26  code
; Added origin code and dummy beam azimut.
;
; Revision 1.9  2006/07/12 21:17:28  code
; Added origin flag.
;
; Revision 1.8  2004/09/16 21:10:54  barnes
; Fixed bug detected by Colin Waters - Some old dat files do not have
; a space between the string "version" and the version number.
;
; Revision 1.7  2004/08/27 22:18:57  barnes
; Fixed bug in the Point family of functions.
;
; Revision 1.6  2004/08/27 19:35:29  barnes
; Added functions for setting the file pointer.
;
; Revision 1.5  2004/08/27 19:15:53  barnes
; Added a function for setting the file pointer
;
; Revision 1.4  2004/08/23 20:40:41  barnes
; Fixed bug in opening older files that do not include the threshold directive in the header.
;
; Revision 1.3  2004/08/16 14:02:40  barnes
; Changed time types.
;
; Revision 1.2  2004/08/11 12:00:56  barnes
; Trapped io error when opening files.
;
; Revision 1.1  2004/08/08 00:11:59  barnes
; Initial revision
;
;
; ---------------------------------------------------------------
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawMakeOldRawFP
;
; PURPOSE:
;       Create a structure to store raw file pointer.
;       
;
; CALLING SEQUENCE:
;       OldRawMakeOldRawFP,raw
;
;       This procedure creates a structure to store the raw file pointer,
;       the structure is returns in rawfp.
;
;
;-----------------------------------------------------------------
;


pro OldRawMakeOldRawFP,rawfp

  rawfp={OldRawFP, rawunit: 0L, $ 
                inxunit: 0L, $
                ctime: 0.0D, $
                stime: 0.0D, $
                frec: 0L, $
                rlen: 0L, $
                ptr: 0L, $
                thr: 0L, $
                major_rev: 0L, $
                minor_rev: 0L, $
                rawread: 0L $
      }
   
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawMakeOldRawRecHdr
;
; PURPOSE:
;       Create a structure to store the record header.
;       
;
; CALLING SEQUENCE:
;       OldRawMakeOldRawRecHdr,rawrechdr
;
;       This procedure creates a structure to store the record
;       header.
;
;
;-----------------------------------------------------------------
;
   

pro OldRawMakeOldRawRecHdr,rawrechdr
    rawrechdr={OldRawRecHdr, size: 0, number: 0L, stamp:bytarr(8) }
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawMakeOldRawRadarParm
;
; PURPOSE:
;       Create a structure to store the radar parameter block as
;       stored in the raw files.
;.
;       
;
; CALLING SEQUENCE:
;       OldRawMakeOldRawRadarParm,radarparm
;
;       This procedure creates a structure to store the radar
;       parameter block.
;
;
;-----------------------------------------------------------------
;
   
pro OldRawMakeOldRawRadarParm,radarparm
    radarparm={OldRawRadarParm, $
                rev: {rev_no, major: byte(0), minor: byte(0)}, $
                nparm: 0, $
                st_id: 0, $
                year: 0, $
                month: 0, $
                day: 0, $
                hour: 0, $
                minut: 0, $
                sec: 0, $
                txpow: 0, $
                nave: 0, $
                atten: 0, $
                lagfr: 0, $
                smsep: 0, $
                ercod: 0, $
                agc_stat: 0, $
                lopwr_stat: 0, $
                nbaud: 0, $
                noise: 0L, $
                noise_mean: 0L, $
                channel: 0, $
                rxrise: 0, $
                intt: 0, $
                txpl: 0, $
                mpinc: 0, $
                mppul: 0, $
                mplgs: 0, $
                nrang: 0, $
                frang: 0, $
                rsep: 0, $
                bmnum: 0, $
                xcf: 0, $
                tfreq: 0, $
                scan: 0, $
                mxpwr: 0L, $
                lvmax: 0L, $
                usr_resL1: 0L, $
                usr_resL2: 0L, $
                cp: 0, $
                usr_resS1: 0, $
                usr_resS2: 0, $
                usr_resS3: 0 $
              }


end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawNativeOpen
;
; PURPOSE:
;       Open a raw file.
;
; CALLING SEQUENCE:
;       rawfp = OldRawNativeOpen(fname)
;
;       This function opens the raw file specified by fname.
;
;       Returns a valid file pointer structure on success, or -1
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function OldRawNativeOpen,fname

ON_IOERROR,iofail


   OldRawMakeOldRawFP,rawfp
   OldRawMakeOldRawRecHdr,rechdr 
   OldRawMakeOldRawRadarParm,rparm


   num_byte=0
   recnum=0L

   openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
  
   rawfp.rawunit=unit
   rawfp.inxunit=-1
   rawfp.stime=-1.0D
   rawfp.ctime=-1.0D
      
   readu,unit,num_byte,recnum

   rawfp.frec=num_byte
   rawfp.rlen=num_byte
   rawfp.ptr=num_byte
 
   if (recnum eq 0) then begin ; decode header 
      buf=bytarr(num_byte-6)
      readu,unit,buf
      txt=string(buf)
      vp=strpos(txt,'version')
      ve=strpos(txt,'.',vp+7)
      vstr=strmid(txt,vp+7,ve-vp-7)
      reads,vstr,vnum
      rawfp.major_rev=long(vnum)
      vp=ve+1
      ve=strpos(txt,' ',vp)
      vstr=strmid(txt,vp,ve-vp)
      reads,vstr,vnum
      rawfp.minor_rev=long(vnum)
      tp=strpos(txt,'threshold')
      if (tp ne -1) then begin
         te=strpos(txt,' ',tp+10)
        tstr=strmid(txt,tp+10,te-tp-10)
        reads,tstr,thr
        rawfp.thr=thr
       endif else begin
          rawfp.thr=3
       endelse
   endif else begin
      point_lun,unit,0
      rawfp.rlen=0
   endelse

  readu, unit,rechdr
  readu, unit,rparm

  rawfp.stime=TimeYMDHMSToEpoch(rparm.year,rparm.month,rparm.day, $
                                rparm.hour,rparm.minut,rparm.sec)  

  rawfp.ctime=rawfp.stime
  point_lun,unit,rawfp.frec


   return, rawfp

iofail:
  return,0
  
end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawTopBit
;
; PURPOSE:
;       Find the top bit set in a 32-bit word.
;       
;
; CALLING SEQUENCE:
;       OldRawTopBit(word)
;
;       This function finds the top bit set in a 32-bit word.
;
;
;       The returned value is the decoded value, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function OldRawTopBit,word

  if (word eq 0) then return, 0
  bytearr=byte(word,0,4)
  q=where(bytearr ne 0,count)
  bit=32-8*q[count-1]
  tmp=bytearr[q[count-1]]
  
  if ((tmp and 'f0'X) ne 0) then begin
    bit-=4
    tmp=tmp/16
  endif

  if ((tmp and '0c'X) ne 0) then begin
    bit-=2
    tmp=tmp/4
  endif
  if ((tmp and '02'X) ne 0) then bit-=1
  return, bit
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawCmpr
;
; PURPOSE:
;       Compress a value in a raw file.
;       
;
; CALLING SEQUENCE:
;       OldRawCmpr(inp,out)
;
;       This function compresses a value in a raw file.
;
;
;-----------------------------------------------------------------
;

function OldRawCmpr,inp,out
  tmp=0L
  for n=0,n_elements(inp)-1 do begin
     if (inp[n] lt 0) then begin
       tmp=long(-inp[n])
       sign=1;
     endif else begin
       tmp=long(inp[n])
       sign=0
     endelse
     bit=OldRawTopBit(swap_endian(tmp,/SWAP_IF_BIG_ENDIAN))
     if (bit eq 0) then tmp=sign*(2^15) $
     else if (bit gt 16) then  tmp=((tmp/2) and '7ff0'X) or (sign*(2^15)) $
     else tmp=( (tmp/(2^(17-bit))) and '7ff0'X) or $ 
             ((17-bit) and '0f'X) or (sign*(2^15))
    
     tmp=swap_endian(fix(tmp),/SWAP_IF_BIG_ENDIAN)
     out[2*n:2*n+1]=byte(tmp,0,2)
   endfor   
   return, 0
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawDcmpr
;
; PURPOSE:
;       Decompress a value in a raw file.
;       
;
; CALLING SEQUENCE:
;       value = OldRawDcmpr(bytarr)
;
;       This function decodes an compressed value in a raw file.
;
;
;       The returned value is the decoded value, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function OldRawDcmpr,word
  count=word[0] and '0f'X  
  value=long((256*(word[1] and '7f'X)) or (word[0] and 'f0'X))
  if (count eq 0) then value=value*2L $
  else begin 
     value=value or '8000'X
     value=value*(2L^count)
  endelse
  if ((word[1] and '80'X) ne 0) then value=-value
  return, value
end
  


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawNativeRead
;
; PURPOSE:
;       Read a record from a raw file.
;       
;
; CALLING SEQUENCE:
;       status = OldRawNativeRead(rawfp,prm,raw)
;
;       This function reads a single record from the open file with
;       file pointer rawfp.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawNativeRead,rawfp,prm,raw

ON_IOERROR,iofail  

  ORIG_COMBF_SIZE=80

  RadarMakeRadarPrm,prm
  RawMakeRawData,raw

  OldRawMakeOldRawRecHdr,rechdr 
  OldRawMakeOldRawRadarParm,rparm

  readu,rawfp.rawunit,rechdr
 
  num_byte=rechdr.size
  rawfp.rlen=rechdr.size
  rawfp.ptr+=rechdr.size
 
  num_byte-=14

  readu,rawfp.rawunit,rparm
    
  num_byte-=N_TAGS(rparm,/LENGTH)

  pulse=intarr(rparm.mppul)

  readu,rawfp.rawunit,pulse

  num_byte-=2*rparm.mppul

  lag=intarr(rparm.mplgs,2)

  readu,rawfp.rawunit,lag

  num_byte-=4*rparm.mplgs

  combf=bytarr(ORIG_COMBF_SIZE)
 
  readu,rawfp.rawunit,combf

  num_byte-=ORIG_COMBF_SIZE

  pwr0=bytarr(2*rparm.nrang)
  
  readu,rawfp.rawunit,pwr0
  
  num_byte-=2*rparm.nrang


  ; populate the output data structure

  prm.revision.major=rparm.rev.major
  prm.revision.minor=rparm.rev.minor
  prm.origin.code=-1
  prm.origin.time=''
  prm.origin.command=''
  prm.cp=rparm.cp
  prm.stid=rparm.st_id
  prm.time.yr=rparm.year
  prm.time.mo=rparm.month
  prm.time.dy=rparm.day
  prm.time.hr=rparm.hour
  prm.time.mt=rparm.minut
  prm.time.sc=rparm.sec
  prm.time.us=0
  prm.txpow=rparm.txpow
  prm.nave=rparm.nave
  prm.atten=rparm.atten
  prm.lagfr=rparm.lagfr
  prm.smsep=rparm.smsep
  prm.ercod=rparm.ercod
  prm.lagfr=rparm.lagfr
  prm.stat.agc=rparm.agc_stat
  prm.stat.lopwr=rparm.lopwr_stat
  prm.noise.search=rparm.noise
  prm.noise.mean=rparm.noise_mean
  prm.channel=rparm.channel
  prm.bmnum=rparm.bmnum
  prm.bmazm=-999
  prm.scan=rparm.scan
  prm.rxrise=rparm.rxrise
  prm.intt.sc=rparm.intt
  prm.intt.us=0
  prm.txpl=rparm.txpl
  prm.mpinc=rparm.mpinc
  prm.mppul=rparm.mppul
  prm.mplgs=rparm.mplgs
  prm.nrang=rparm.nrang
  prm.frang=rparm.frang
  prm.rsep=rparm.rsep
  prm.xcf=rparm.xcf
  prm.tfreq=rparm.tfreq
  prm.offset=rparm.usr_resL1
  prm.mxpwr=rparm.mxpwr
  prm.lvmax=rparm.lvmax
  prm.pulse[0:prm.mppul-1]=pulse[0:prm.mppul-1]
  prm.lag[0:prm.mplgs-1,0]=lag[*,0]
  prm.lag[0:prm.mplgs-1,1]=lag[*,1]
  prm.combf=string(combf)

  raw.thr=rawfp.thr
  raw.revision.major=rawfp.major_rev
  raw.revision.minor=rawfp.minor_rev

  for n=0,rparm.nrang-1 do begin
    raw.pwr0[n]=OldRawDcmpr(pwr0[2*n:2*n+1])
  endfor

  xcf_data=0
  prev_range=-1
  range=0
  raw.acfd[*,*,*]=0
  raw.xcfd[*,*,*]=0

  buf=bytarr(rparm.mplgs*2*2)
  while (num_byte gt 0) do begin
    readu,rawfp.rawunit,range
    range=range-1
    num_byte-=2
    if ((range le prev_range) and (rparm.xcf eq 1)) then xcf_data = 1
   
    readu,rawfp.rawunit,buf
    for n=0,rparm.mplgs-1 do begin
       if (xcf_data ne 0) then begin
         raw.xcfd[range,n,0]=OldRawDcmpr(buf[4*n:4*n+1])
         raw.xcfd[range,n,1]=OldRawDcmpr(buf[4*n+2:4*n+3]) 
       endif else begin
         raw.acfd[range,n,0]=OldRawDcmpr(buf[4*n:4*n+1])
         raw.acfd[range,n,1]=OldRawDcmpr(buf[4*n+2:4*n+3]) 
       endelse
       num_byte-=4
       prev_range=range
   endfor
   
endwhile

  rawfp.ctime=TimeYMDHMSToEpoch(rparm.year,rparm.month,rparm.day, $
                                rparm.hour,rparm.minut,rparm.sec)


  return,0

iofail:
  return,-1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawNativeWrite
;
; PURPOSE:
;       Write a record from a raw file.
;       
;
; CALLING SEQUENCE:
;       status = OldRawNativeWrite(unit,prm,raw,recnum)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;
;       The returned value is zero on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawNativeWrite,unit,prm,raw,recnum

ON_IOERROR,iofail

  ORIG_COMBF_SIZE=80
  ORIG_MAX_RANGE=75

  OldRawMakeOldRawRecHdr,rechdr 
  OldRawMakeOldRawRadarParm,rparm

  rparm.rev.major=prm.revision.major
  rparm.rev.minor= prm.revision.minor
  rparm.nparm=48
  rparm.cp=prm.cp
  rparm.st_id=prm.stid
  rparm.year=prm.time.yr
  rparm.month= prm.time.mo
  rparm.day= prm.time.dy
  rparm.hour=prm.time.hr
  rparm.minut= prm.time.mt
  rparm.sec=prm.time.sc
  rparm.txpow=prm.txpow
  rparm.nave=prm.nave
  rparm.atten=prm.atten
  rparm.lagfr=prm.lagfr
  rparm.smsep=prm.smsep
  rparm.ercod=prm.ercod
  rparm.lagfr=prm.lagfr
  rparm.agc_stat=prm.stat.agc
  rparm.lopwr_stat=prm.stat.lopwr
  rparm.noise=prm.noise.search
  rparm.noise_mean=prm.noise.mean
  rparm.channel=prm.channel
  rparm.bmnum=prm.bmnum
  rparm.scan=prm.scan
  rparm.rxrise=prm.rxrise
  rparm.intt=prm.intt.sc
  rparm.txpl=prm.txpl
  rparm.mpinc=prm.mpinc
  rparm.mppul=prm.mppul
  rparm.mplgs=prm.mplgs
  rparm.nrang=prm.nrang
  rparm.frang=prm.frang
  rparm.rsep=prm.rsep
  rparm.xcf=prm.xcf
  rparm.tfreq=prm.tfreq
  rparm.usr_resL1= prm.offset
  rparm.mxpwr=prm.mxpwr
  rparm.lvmax=prm.lvmax

  if (rparm.nrang gt ORIG_MAX_RANGE) then rparm.nrang=ORIG_MAX_RANGE

  num_byte=14+N_TAGS(rparm,/LENGTH)+2*rparm.mppul+4*rparm.mplgs+ $
           ORIG_COMBF_SIZE+2*rparm.nrang

  threshold=floor((raw.thr*rparm.noise)/2);
  slist=where(raw.pwr0[0:rparm.nrang-1] ge threshold,count)
  
  num_byte+=count*(rparm.mplgs*2*2+2)
  if (rparm.xcf ne 0) then num_byte+=count*(rparm.mplgs*2*2+2)

  rechdr.size=num_byte
  rechdr.number=recnum
  rechdr.stamp[*]=byte('rawwrite')

  writeu,unit,rechdr
  writeu,unit,rparm

  pulse=fix(prm.pulse[0:rparm.mppul-1])
  lag=fix(prm.lag[0:rparm.mplgs-1,*])
  combf=bytarr(ORIG_COMBF_SIZE)
  l=strlen(prm.combf)
  if (l gt ORIG_COMBF_SIZE) then l=ORIG_COMBF_SIZE
  tmp=byte(prm.combf)
  combf[0:l-1]=tmp[0:l-1]
 
  writeu,unit,pulse
  writeu,unit,lag
  writeu,unit,combf

  pwr0=bytarr(rparm.nrang*2)

  s=OldRawCmpr(raw.pwr0[0:rparm.nrang-1],pwr0)
  writeu,unit,pwr0

  rbuf=bytarr(rparm.mplgs*2)
  ibuf=bytarr(rparm.mplgs*2)
  buf=bytarr(rparm.mplgs*2*2)
  if (count ne 0) then begin
    for n=0,count-1 do begin
      range=fix(slist[n]+1)
      s=OldRawCmpr(raw.acfd[slist[n],0:rparm.mplgs-1,0],$
                   rbuf)
      s=OldRawCmpr(raw.acfd[slist[n],0:rparm.mplgs-1,1],$
                   ibuf)
      for x=0,rparm.mplgs-1 do begin
        buf[4*x]=rbuf[2*x]
        buf[4*x+1]=rbuf[2*x+1]
        buf[4*x+2]=ibuf[2*x]
        buf[4*x+3]=ibuf[2*x+1]
       endfor
       writeu,unit,range,buf
    endfor
  endif


  if (rparm.xcf eq 1) and (count ne 0) then begin
    for n=0,count-1 do begin
      s=OldRawCmpr(raw.xcfd[slist[n],0:rparm.mplgs-1,0],$
                   rbuf)
      s=OldRawCmpr(raw.xcfd[slist[n],0:rparm.mplgs-1,1],$
                   ibuf)
      range=fix(slist[n]+1)
      for x=0,rparm.mplgs-1 do begin
        buf[4*x]=rbuf[2*x]
        buf[4*x+1]=rbuf[2*x+1]
        buf[4*x+2]=ibuf[2*x]
        buf[4*x+3]=ibuf[2*x+1]
      endfor
      writeu,unit,range,buf
    endfor
  endif
  return,s


iofail:
  return,-1



end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawNativeWriteHeader
;
; PURPOSE:
;       Write the header to a raw file.
;       
;
; CALLING SEQUENCE:
;       status = OldRawNativeWriteHeader(unit,prm,raw)
;
;       This function writes the header record to the open file
;       with logical unit number unit.
;
;
;       The returned value is zero on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawNativeWriteHeader,unit,prm,raw,recnum

ON_IOERROR,iofail


  OldRawMakeOldRawRecHdr,rechdr 

   rechdr.number=0
   rechdr.stamp[*]=byte('rawwrite')
   vstr=string(format='(I1,".",I3.3)',raw.revision.major,raw.revision.minor)
   txt=' version '+vstr+' threshold '+string(format='(I1)',raw.thr)+' '
   txt+='IDL output'
   time=systime()
     
   rechdr.size=16+strlen(txt)+strlen(time)
   writeu, unit,rechdr
   writeu, unit, byte(txt), 10B, byte(time),10B
 
   return, 0

iofail:
  return,-1

end
   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawNativeSeek
;
; PURPOSE:
;       Find a record in a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawNativeSeek(rawfp,yr,mo,dy,hr,mt,sc,atme=atime)
;
;       This function searchs for a record in a raw file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawNativeSeek,rawfp,yr,mo,dy,hr,mt,sc,atme=atme

ON_IOERROR,iofail

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  if (tval<rawfp.ctime) then begin
     point_lun,rawfp.rawunit,rawfp.frec
     rawfp.ptr=rawfp.frec
     s=OldRawNativeRead(rawfp,prm,raw)
  endif

  repeat begin
   rjmp=rawfp.rlen
   atme=rawfp.ctime;
   s=OldRawNativeRead(rawfp,prm,raw)
   if (s eq -1) then break
  endrep until (tval lt rawfp.ctime)
  rawfp.ptr=rawfp.ptr-(rawfp.rlen+rjmp)
  rawfp.rlen=rjmp
  point_lun,rawfp.rawunit,rawfp.ptr
  return,0

iofail:
  return, -1

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawNativePoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawNativePoint(rawfp,ptr)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawNativePoint,rawfp,off

ON_IOERROR,iofail

  point_lun,rawfp.rawunit,off
  rawfp.ptr=off
  return,off

iofail:
  return, -1

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawExternalRead
;
; PURPOSE:
;       Read a record from a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawExternalRead(rawfp,prm,raw,lib=libary)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and raw are 
;       populated accordingly.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function OldRawExternalRead,rawfp,prm,raw,lib=lib
  RadarMakeRadarPrm, prm
  RawMakeRawData, raw

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')

  s=CALL_EXTERNAL(lib,'OldRawIDLRead', $
                  rawfp,prm,raw)
  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawExternalWrite
;
; PURPOSE:
;       Write a record to a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawExternalWrite(unit,prm,raw,recnum,lib=libary)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;       The returned value is size zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function OldRawExternalWrite,unit,prm,raw,recnum,lib=lib
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  s=CALL_EXTERNAL(lib,'OldRawIDLWrite', $
                  unit,prm,raw,recnum)


 
  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawExternalWriteHeader
;
; PURPOSE:
;       Write a header a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawExternalWriteHeader(unit,prm,raw,lib=libary)
;
;       This function writes the header to a raw file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function OldRawExternalWriteHeader,unit,prm,raw,lib=lib
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  s=CALL_EXTERNAL(lib,'OldRawIDLWriteHeader', $
                  unit,prm,raw)
  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawExternalSeek
;
; PURPOSE:
;       Find a record in a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawExternalSeek(rawfp,yr,mo,dy,hr,mt,sc,atme,lib=libary)
;
;       This function searchs for a record in a raw file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;


function OldRawExternalSeek,rawfp,yr,mo,dy,hr,mt,sc,atme=atme,lib=lib

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  atme=0.0D
  yrval=long(yr)
  moval=long(mo)
  dyval=long(dy)
  hrval=long(hr)
  mtval=long(mt)
  scval=double(sc)
   
  s=CALL_EXTERNAL(lib,'OldRawIDLSeek', $
                  rawfp,yrval,moval,dyval,hrval,mtval,scval,atme) 

  return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawExternalPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawExternalPoint(rawfp,ptr,lib=library)
;
;       This function sets the file pointer in a raw file.
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawExternalPoint,rawfp,off,lib=lib

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')   
  offl=long(off)
  s=CALL_EXTERNAL(lib,'OldRawIDLPoint', $
                  rawfp,offl) 

  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawExternalOpen
;
; PURPOSE:
;       Open a raw file.
;
; CALLING SEQUENCE:
;       rawfp = OldRawExternalOpen(fname,lib=library)
;
;       This function opens the raw file specified by fname.
;
;       Returns a valid file pointer structure on success, or -1
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function OldRawExternalOpen,fname,lib=lib


ON_IOERROR, iofail
   OldRawMakeOldRawFP,rawfp

   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')

   openr,unit,fname,BUFSIZE=0,/GET_LUN,/STDIO
   s=CALL_EXTERNAL(lib,'OldRawIDLOpen', $
                   unit,rawfp)

   return, rawfp

iofail:
  return, 0

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawClose
;
; PURPOSE:
;       Close a raw file.
;
; CALLING SEQUENCE:
;       s = OldRawClose(rawfp)
;
;       This function closes the raw file with the file pointer rawfp
;
;       Returns a zero on success, or -1
;       if an error occurred.
;-----------------------------------------------------------------
;


function OldRawClose,rawfp

ON_IOERROR,iofail

   free_lun, rawfp.rawunit
   return, 0

iofail:
  return, -1
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawOpen
;
; PURPOSE:
;       Open a raw file.
;
; CALLING SEQUENCE:
;       rawfp = OldRawOpen(fname,lib=library,/native,/external)
;
;       This function opens the raw file specified by fname.
;
;       Returns a valid file pointer structure on success, or -1
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function OldRawOpen,fname,lib=lib,native=native,external=external 

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1


  if (mode eq 1) then return, OldRawNativeOpen(fname) $
  else return, OldRawExternalOpen(fname,lib=lib)

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawRead
;
; PURPOSE:
;       Read a record from a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawRead(rawfp,prm,raw,lib=libary,/native,/external) 
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and raw are 
;       populated accordingly.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;


function OldRawRead,rawfp,prm,raw,lib=lib,native=native,external=external 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1


  if (mode eq 1) then return, OldRawNativeRead(rawfp,prm,raw) $
  else return, OldRawExternalRead(rawfp,prm,raw,lib=lib)
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawWrite
;
; PURPOSE:
;       Write a record to a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawWrite(unit,prm,raw,recnum,lib=libary,/native,/external)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;       The returned value is size zero on success,
;       or  -1 for failure
;
;
;-----------------------------------------------------------------
;

function OldRawWrite,unit,prm,raw,recnum,lib=lib,$
                 native=native,external=external 
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1


  if (mode eq 1) then return, OldRawNativeWrite(unit,prm,raw,recnum) $
  else return, OldRawExternalWrite(unit,prm,raw,recnum,lib=lib)


end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawWriteHeader
;
; PURPOSE:
;       Write the header record to a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawWriteHeader(unit,prm,rawlib=libary,/native,/external)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;       The returned value is size zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawWriteHeader,unit,prm,raw,lib=lib,$
                 native=native,external=external 
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1


  if (mode eq 1) then return, OldRawNativeWriteHeader(unit,prm,raw) $
  else return, OldRawExternalWriteHeader(unit,prm,raw,lib=lib)


end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawSeek
;
; PURPOSE:
;       Find a record in a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawSeek(rawfp,yr,mo,dy,hr,mt,sc,atme,lib=libary,
;                           /native,/external)                           
;
;       This function searchs for a record in a raw file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;


function OldRawSeek,rawfp,yr,mo,dy,hr,mt,sc,atme=atme,lib=lib, $
                 native=native,external=external 
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then return, OldRawNativeSeek(rawfp,yr,mo,dy,hr,mt,sc, $
                                               atme=atme) $
  else return, OldRawExternalSeek(rawfp,yr,mo,dy,hr,mt,sc,lib=lib,atme=atme)


end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldRawPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = OldRawPoint(rawfp,ptr,lib=library)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldRawPoint,rawfp,off,lib=lib

 mode=0
 if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDRAWIDL')   
 if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then return, OldRawNativePoint(rawfp,off) $
  else return, OldRawExternalPoint(rawfp,off,lib=lib)

  

end
