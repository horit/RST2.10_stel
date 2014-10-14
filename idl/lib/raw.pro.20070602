; raw.pro
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
; Public Functions
; ----------------
;
; RawMakeRawData        
; RawNativeRead         
; RawNativeWrite
; RawMakeRawInx          
; RawLoadInx              
; RawGetTime    
; RawNativeSeek
; RawNativePoint       
; RawExternalRead      
; RawExternalWrite    
; RawExternalSeek
; RawExternalPoint  
; RawRead   
; RawWrite          
; RawSeek
; RawPoint
;
; ---------------------------------------------------------------
; $Log: raw.pro,v $
; Revision 1.11  2007/02/05 22:22:53  code
; Fixed the lag table.
;
; Revision 1.10  2006/09/14 21:51:10  code
; Removed debug code.
;
; Revision 1.9  2006/09/14 19:30:54  code
; Fixed bug in decoding time.
;
; Revision 1.8  2006/07/12 21:17:28  code
; Added origin flag.
;
; Revision 1.7  2006/07/12 21:10:19  code
; Added the origin flag.
;
; Revision 1.6  2006/05/10 21:37:01  barnes
; Changed the size of some of the defaults.
;
; Revision 1.5  2006/02/06 20:03:27  barnes
; Changed micro-seconds to long integer.
;
; Revision 1.4  2004/08/27 22:18:57  barnes
; Fixed bug in the Point family of functions.
;
; Revision 1.3  2004/08/27 19:35:01  barnes
; Added functions for setting the file pointer.
;
; Revision 1.2  2004/08/09 19:05:22  barnes
; Minor bug fixes to select the right code to compile.
;
; Revision 1.1  2004/08/03 22:59:40  barnes
; Initial revision
;
; ---------------------------------------------------------------
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawMakeRawData
;
; PURPOSE:
;       Create a structure to store rawACF data.
;       
;
; CALLING SEQUENCE:
;       RawMakeRawData,raw
;
;       This procedure creates a structure to store the rawACF data,
;       the structure is returns in rawdata.
;
;
;-----------------------------------------------------------------
;


pro RawMakeRawData,raw

  LAG_SIZE=96
  MAX_RANGE=300

  raw={RawData, $
         revision: {rlstr, major: 0L, minor: 0L}, $ 
         thr: 0.0, $
         pwr0: fltarr(MAX_RANGE), $
         acfd: fltarr(MAX_RANGE,LAG_SIZE,2), $
         xcfd: fltarr(MAX_RANGE,LAG_SIZE,2) $
      }

end
   
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawNativeRead
;
; PURPOSE:
;       Read a record from a rawacf file.
;       
;
; CALLING SEQUENCE:
;       status = RawNativeRead(unit,prm,raw)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The scalar and array vectors are
;       returned in sclvec and arrvec respectively.
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawNativeRead,unit,prm,raw

  s=DataMapRead(unit,sclvec,arrvec)
  if (s eq -1) then return,s
  
  RadarMakeRadarPrm,prm
  RawMakeRawData,raw
  

  sclname=['radar.revision.major','radar.revision.minor', $
           'origin.code','origin.time','origin.command','cp','stid', $
           'time.yr','time.mo','time.dy','time.hr','time.mt','time.sc', $
           'time.us','txpow','nave','atten','lagfr','smsep','ercod', $
           'stat.agc','stat.lopwr','noise.search','noise.mean','channel', $
           'bmnum','bmazm','scan','offset','rxrise','intt.sc','intt.us', $
           'txpl', 'mpinc','mppul','mplgs','nrang','frang','rsep','xcf', $
           'tfreq', 'mxpwr','lvmax','rawacf.revision.major', $
           'rawacf.revision.minor','combf','thr']

  scltype=[1,1,1,9,9,2,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,2,4,4,2,2,4,2,2,2,2,3, $
           2,2,2,2,2,2,2,2,2,3,3,3,3,9,4]
  
  sclid=intarr(n_elements(sclname))
  sclid[*]=-1

  arrname=['ptab','ltab','slist','pwr0','acfd','xcfd']
  arrtype=[2,2,2,4,4,4]

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

  raw.revision.major=*(sclvec[sclid[43]].ptr)
  raw.revision.minor=*(sclvec[sclid[44]].ptr)
  raw.thr=*(sclvec[sclid[46]].ptr)

  if (prm.nrang gt 0) then raw.pwr0[0:prm.nrang-1]=*(arrvec[arrid[3]].ptr)

  if (arrid[2] eq -1) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return,s
  endif

  if ~(ptr_valid(arrvec[arrid[2]].ptr)) then begin
     st=DataMapFreeScalar(sclvec)
     st=DataMapFreeArray(arrvec)
     return,s
  endif

  slist=*(arrvec[arrid[2]].ptr)

  if (n_elements(slist) eq 0) then return,s 
  
  raw.acfd[slist,0:prm.mplgs-1,*]= (*(arrvec[arrid[4]].ptr))[*,*,*]
 
  if (prm.xcf ne 0) and (arrid[5] ne -1) then $
    raw.xcfd[slist,0:prm.mplgs-1,*]= (*(arrvec[arrid[5]].ptr))[*,*,*]

  st=DataMapFreeScalar(sclvec)
  st=DataMapFreeArray(arrvec)

  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawNativeWrite
;
; PURPOSE:
;       Write a record from a rawacf file.
;       
;
; CALLING SEQUENCE:
;       status = RawNativeWrite(unit,prm,raw)
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

function RawNativeWrite,unit,prm,raw

  
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
  s=DataMapMakeScalar('rawacf.revision.major',raw.revision.major,sclvec)
  s=DataMapMakeScalar('rawacf.revision.minor',raw.revision.minor,sclvec)
  s=DataMapMakeScalar('combf',prm.combf,sclvec)
  s=DataMapMakeScalar('thr',raw.thr,sclvec)

  s=DataMapMakeArray('ptab',prm.pulse[0:prm.mppul-1],arrvec,/new)

  s=DataMapMakeArray('ltab',prm.lag[0:prm.mplgs,*],arrvec)

  tx=floor((raw.thr*prm.noise.search)/2.0)

  slist=fix(where(raw.pwr0[0:prm.nrang-1] ge tx,count))

  if (count ne 0) then s=DataMapMakeArray('slist',slist,arrvec)
  
  s=DataMapMakeArray('pwr0',raw.pwr0[0:prm.nrang-1],arrvec)
  
  if (count ne 0) then begin 
    s=DataMapMakeArray('acfd',raw.acfd[slist,0:prm.mplgs-1,*],arrvec)
 
    if (prm.xcf eq 1) then $
      s=DataMapMakeArray('xcfd',raw.xcfd[slist,0:prm.mplgs-1,*],arrvec)
  endif
  s=DataMapWrite(unit,sclvec,arrvec)
  s=DataMapFreeScalar(sclvec)
  s=DataMapFreeArray(arrvec)
  return,s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawMakeRawInx
;
; PURPOSE:
;       Create a structure to store rawACF index.
;
;
; CALLING SEQUENCE:
;       RawMakeRawInx,inx
;
;       This procedure creates a structure to store the rawacf index.
;
;
;-----------------------------------------------------------------
;

pro RawMakeRawIndex,inx,time,offset
  inx={RawInx, time: time, offset: offset}
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawLoadIndex
;
; PURPOSE:
;       Reads an index to a rawacf file..
;
;
; CALLING SEQUENCE:
;       status = RawLoadIndex(unit,inx)
;
;       This function reads an index to a rawacf file.
;
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawLoadInx,unit,inx

ON_IOERROR,iofail

  time=0.0D
  offset=0L
  s=0
  c=0L
  while (s eq 0) do begin
    readu,unit,time,offset
    RawMakeRawIndex,tinx,time,offset
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
;       RawGetTime
;
; PURPOSE:
;       Extract the time from a fitacf record.
;
;
; CALLING SEQUENCE:
;       tval = RawGetTime(sclvec)
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

function RawGetTime,sclvec

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
;       RawNativeSeek
;
; PURPOSE:
;       Find a record in a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a rawacf file.
;
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawNativeSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

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
       tfile=RawGetTime(sclvec)
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
       tfile=RawGetTime(sclvec)      
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
;       RawNativePoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = RawNativePoint(unit,ptr)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawNativePoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawExternalRead
;
; PURPOSE:
;       Read a record from a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawExternalRead(unit,prm,raw)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and raw are 
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

function RawExternalRead,unit,prm,raw,lib=lib
  RadarMakeRadarPrm, prm
  RawMakeRawData, raw

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
  s=CALL_EXTERNAL(lib,'RawIDLRead', $
                  unit,prm,raw)

  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawExternalWrite
;
; PURPOSE:
;       Write a record from a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawExternalWrite(unit,prm,raw)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. The structure prm and raw are 
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

function RawExternalWrite,unit,prm,raw,lib=lib
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
  s=CALL_EXTERNAL(lib,'RawIDLWrite', $
                  unit,prm,raw)

  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawExternalSeek
;
; PURPOSE:
;       Find a record in a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawExternalSeek(unit,yr,mo,dy,hr,mt,sc,atme,inx)
;
;       This function searchs for a record in a rawacf file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;


function RawExternalSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
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
    s=CALL_EXTERNAL(lib,'RawIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme,inum, $
                  tval,oval) 
  endif else begin 
    s=CALL_EXTERNAL(lib,'RawIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme) 
  endelse

  return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawExternalPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = RawExternalPoint(unit,ptr,lib=library)
;
;       This function sets the file pointer in a raw file.
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawExternalPoint,unit,off,lib=lib

 if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')   
 offl=long(off)
 s=CALL_EXTERNAL(lib,'RawIDLPoint', $
                unit,offl) 
 return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawRead
;
; PURPOSE:
;       Read a record from a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawRead(unit,prm,raw,lib=library,/native,/external)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. The structure prm and raw are 
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


function RawRead,unit,prm,raw,lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2  
  if (mode eq 2) then s=RawExternalRead(unit,prm,raw,lib=lib) $
  else s=RawNativeRead(unit,prm,raw)
  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawWrite
;
; PURPOSE:
;       Write a record to a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawRead(unit,prm,raw,lib=library,/native,/external)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. The structure prm and raw are 
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


function RawWrite,unit,prm,raw,lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2
  
  if (mode eq 2) then s=RawExternalWrite(unit,prm,raw,lib=lib) $
  else s=RawNativeWrite(unit,prm,raw)
  return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawSeek
;
; PURPOSE:
;       Find a record in a rawacf file.
;
;
; CALLING SEQUENCE:
;       status = RawSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=library,
;                        /native,/external)
;
;       This function searchs for a record in a rawacf file.
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


function RawSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib, $
                         native=native,external=external


  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2
  
  if (mode eq 2) then s=RawExternalSeek(unit,yr,mo,dy,hr,mt,sc,inx, $
                                        lib=lib,atme=atme) $
  else s=RawNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme)
  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RawOpen
;
; PURPOSE:
;       Open a rawacf file.
;
;
; CALLING SEQUENCE:
;       unit = RawOpen(lib=library,/native,/external,/read,/write,/update)
;
;       This function opens a rawacf file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;       This function is provided as a convenience and opens
;       the rawacf file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function RawOpen,fname,atme=atme,lib=lib, $
                 native=native,external=external, $
                 read=read,write=write,update=update

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then begin
    if KEYWORD_SET(read) then openr,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(write) then openw,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
    if KEYWORD_SET(update) then openu,unit,fname,/GET_LUN,/SWAP_IF_BIG_ENDIAN
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
;       RawPoint
;
; PURPOSE:
;       Sets the file pointer position in a raw file.
;
;
; CALLING SEQUENCE:
;       status = RawPoint(rawfp,ptr,lib=library)
;
;       This function sets the file pointer in a raw file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function RawPoint,unit,off,lib=lib

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RAWIDL')   
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then return, RawNativePoint(unit,off) $
  else return, RawExternalPoint(unit,off,lib=lib)

  

end
