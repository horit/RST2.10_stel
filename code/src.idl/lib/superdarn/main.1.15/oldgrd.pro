; oldgrd.pro
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
; PublicFunctions
; ---------------
;
; OldGridNativeRead        
; OldGridNativeWrite        
; OldGridMakeGridInx         
; OldGridLoadInx          
; OldGridGetTime          
; OldGridNativeSeek     
; OldGridNativePoint   
; OldGridExternalRead      
; OldGridExternalWrite      
; OldGridExternalSeek 
; OldGridExternalPoint     
; OldGridRead               
; OldGridWrite             
; OldGridSeek            
; OldGridOpen              
; OldGridPoint
;
; ---------------------------------------------------------------
; $Log: oldgrd.pro,v $
; Revision 1.9  2004/11/04 20:49:29  barnes
; Fixed bug in the search algorithm.
;
; Revision 1.8  2004/11/04 20:18:43  barnes
; Re-wrote the read routine to actually decode the block headers.
;
; Revision 1.7  2004/11/04 15:34:10  barnes
; Fixed bug in writting regular style files.
;
; Revision 1.6  2004/11/04 15:25:19  barnes
; Fixed possible problems with format overflows.
;
; Revision 1.5  2004/11/03 23:50:11  barnes
; Fixed possible problem with formatting the output.
;
; Revision 1.4  2004/11/03 23:42:34  barnes
; Fixed bug in writing out old style grid files.
;
; Revision 1.3  2004/11/03 23:40:02  barnes
; Fixed return status for OldGrdNativeWrite
;
; Revision 1.2  2004/11/03 21:31:23  barnes
; Fixed bug in the external read routine that caused a crash when reading
; empty records.
;
; Revision 1.1  2004/11/02 22:30:34  barnes
; Initial revision
;
;
; ---------------------------------------------------------------
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridNativeRead
;
; PURPOSE:
;       Read a record from a grid file.
;       
;
; CALLING SEQUENCE:
;       status = GridNativeRead(unit,prm,stvec,gvec)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. 
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridNativeRead,unit,prm,stvec,gvec

  block=strarr(3,18)
  block[0,0:17]=['st_id','chn','nvec','freq0','major_rev','minor_rev', $
                 'prog_id','noise_mean','noise_sd','gsct','v_min','v_max', $
                 'p_min','p_max','w_min','w_max','ve_min','ve_max']

  block[1,0:11]=['gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd','pwr','pwr_sd','wdt','wdt_sd']
  block[2,0:7]=['gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd']


  ON_IOERROR,iofail

  GridMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec

  syr=0
  smo=0
  sdy=0
  shr=0
  smt=0
  ssc=0
 
  eyr=0
  emo=0
  edy=0
  ehr=0
  emt=0
  esc=0

  point_lun,-unit,sptr
 
  readf,unit,syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc
  prm.stme.yr=syr
  prm.stme.mo=smo
  prm.stme.dy=sdy
  prm.stme.hr=shr
  prm.stme.mt=smt
  prm.stme.sc=ssc
  prm.etme.yr=eyr
  prm.etme.mo=emo
  prm.etme.dy=edy
  prm.etme.hr=ehr
  prm.etme.mt=emt
  prm.etme.sc=esc

  nblk=0
  nprm=0
  npnt=0

 readf,unit,nblk
  for blk=0,nblk-1 do begin
    readf,unit,npnt,nprm
    names=''
    units=''
    types=''
    readf,unit,names 
    readf,unit,units
    readf,unit,types
    str=strsplit(names,' ',/EXTRACT)
    x=strjoin(str)
    y=strjoin(transpose(block[*,*]))
    q=where(strcmp(x,y) ne 0,cnt)
    if cnt eq 0 then begin
       tmp=''
       for n=0,npnt-1 do readf,tmp
       continue
    endif
    case q[0] of

    0: BEGIN
        prm.stnum=npnt
        stvec=replicate(stvec,prm.stnum)
        for n=0,npnt-1 do begin
          readf,unit,stid,chn,nvec,freq,major_rev,minor_rev,prog_id, $
            noise_mean,noise_sd,gsct, $
            v_min,v_max,p_min,p_max,w_min,w_max,ve_min,ve_max
          stvec[n].st_id=stid
          stvec[n].chn=chn
          stvec[n].npnt=nvec
          stvec[n].freq=freq
          stvec[n].major_revision=major_rev
          stvec[n].minor_revision=minor_rev
          stvec[n].prog_id=prog_id
          stvec[n].noise.mean=noise_mean
          stvec[n].noise.sd=noise_sd
          stvec[n].gsct=gsct
          stvec[n].vel.min=v_min
          stvec[n].vel.max=v_max
          stvec[n].pwr.min=p_min
          stvec[n].pwr.max=p_max
          stvec[n].wdt.min=w_min
          stvec[n].wdt.max=w_max
          stvec[n].verr.min=ve_min
          stvec[n].verr.max=ve_max
        endfor

      END
      1: BEGIN
        prm.vcnum=npnt
        prm.xtd=1
        gvec=replicate(gvec,prm.vcnum)
        for n=0,npnt-1 do begin
          readf,unit,gmlon,gmlat,kvect,stid,chn,index,vlos,vlos_sd, $
               pwr,pwr_sd,wdt,wdt_sd
          gvec[n].mlon=gmlon
          gvec[n].mlat=gmlat
          gvec[n].azm=kvect
          gvec[n].st_id=stid
          gvec[n].chn=chn
          gvec[n].index=index
          gvec[n].vel.median=vlos
          gvec[n].vel.sd=vlos_sd
          gvec[n].pwr.median=pwr
          gvec[n].pwr.sd=pwr_sd
          gvec[n].wdt.median=wdt
          gvec[n].wdt.sd=wdt_sd
        endfor

      END
      2: BEGIN
        prm.vcnum=npnt
        prm.xtd=0
        gvec=replicate(gvec,prm.vcnum)
        for n=0,npnt-1 do begin
          readf,unit,gmlon,gmlat,kvect,stid,chn,index,vlos,vlos_sd
          gvec[n].mlon=gmlon
          gvec[n].mlat=gmlat
          gvec[n].azm=kvect
          gvec[n].st_id=stid
          gvec[n].chn=chn
          gvec[n].index=index
          gvec[n].vel.median=vlos
          gvec[n].vel.sd=vlos_sd
        endfor
      END
  endcase
  endfor

  point_lun,-unit,eptr

  return,eptr-sptr  

iofail:
  return,-1 
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridNativeWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;       
;
; CALLING SEQUENCE:
;       status = OlcGridNativeWrite(unit,prm,stvec,gvec)
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


function OldGridNativeWrite,unit,prm,stvec,gvec

ON_IOERROR,iofail

  point_lun,-unit,sptr

  printf,unit,prm.stme.yr,prm.stme.mo,prm.stme.dy,prm.stme.hr,prm.stme.mt,$
              fix(prm.stme.sc),prm.etme.yr,prm.etme.mo,prm.etme.dy,$
              prm.etme.hr,prm.etme.mt,fix(prm.etme.sc),$
              format='(1I5,5I3.2,1I5,5I3.2)'
  printf,unit,2,format='(1I0)'
  printf,unit,prm.stnum,18,format='(1I0," ",1I0)'
  printf,unit,'st_id','chn','nvec','freq0','major_rev','minor_rev', $
              'prog_id','noise_mean','noise_sd','gsct','v_min','v_max', $
              'p_min','p_max','w_min','w_max','ve_min','ve_max', $
              format='(18A15)'

  printf,unit,'count','count','count','KHz','count','count', $
              'count','count','count','count','m/sec','m/sec', $
              'dB','dB','m/sec','m/sec','m/sec','m/sec', $
               format='(18A15)'

  printf,unit,'int','int','int','float','int','int', $
              'int','float','float','int','float','float', $
              'float','float','float','float','float','float', $
               format='(18A15)'

  for n=0,prm.stnum-1 do begin
    printf,unit,stvec[n].st_id,stvec[n].chn,stvec[n].npnt,stvec[n].freq, $
      stvec[n].major_revision,stvec[n].minor_revision, $
      stvec[n].prog_id,float(stvec[n].noise.mean),float(stvec[n].noise.sd), $
      stvec[n].gsct,float(stvec[n].vel.min),float(stvec[n].vel.max), $
      float(stvec[n].pwr.min),float(stvec[n].pwr.max),$
      float(stvec[n].wdt.min), $
      float(stvec[n].wdt.max),float(stvec[n].verr.min), $
      float(stvec[n].verr.max), $
      format='(3I15,1G15,3I15,2G15,1I15,8G15)' 

  endfor

  if prm.xtd eq 1 then begin
    printf,unit,prm.vcnum,12,format='(1I0," ",1I0)'
    printf,unit,'gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd','pwr','pwr_sd','wdt','wdt_sd', $
              format='(12A15)'
    printf,unit,'degrees','degrees','degrees','count','count','count', $
              'm/sec','m/sec','dB','dB','m/sec','m/sec', $
              format='(12A15)'
    printf,unit,'float','float','float','int','int','int', $
              'float','float','float','float','float','float', $
              format='(12A15)'

    for n=0,prm.vcnum-1 do begin
      printf,unit,float(gvec[n].mlon),float(gvec[n].mlat),float(gvec[n].azm), $
        gvec[n].st_id,gvec[n].chn, $
        gvec[n].index,float(gvec[n].vel.median),float(gvec[n].vel.sd), $
        float(gvec[n].pwr.median),float(gvec[n].pwr.sd),$
        float(gvec[n].wdt.median), $
        float(gvec[n].wdt.sd),format='(3G15,3I15,6G15)'
    endfor

  endif else begin
    printf,unit,prm.vcnum,12,format='(1I0," ",1I0)'
    printf,unit,'gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd', $
              format='(8A15)'
    printf,unit,'degrees','degrees','degrees','count','count','count', $
              'm/sec','m/sec', $
              format='(8A15)'
    printf,unit,'float','float','float','int','int','int', $
              'float','float', $
              format='(8A15)'
    for n=0,prm.vcnum-1 do begin
      printf,unit,gvec[n].mlon,gvec[n].mlat,gvec[n].azm, $
        gvec[n].st_id,gvec[n].chn, $
        gvec[n].index,gvec[n].vel.median,gvec[n].vel.sd, $
        format='(3G15,3I15,2G15)'
     endfor

 endelse


  point_lun,-unit,eptr

  return,eptr-sptr  

iofail:
  return,-1 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridMakeGridInx
;
; PURPOSE:
;       Create a structure to store grdmap index.
;
;
; CALLING SEQUENCE:
;       OldGridMakeGridInx,inx
;
;       This procedure creates a structure to store the grid index.
;
;
;-----------------------------------------------------------------
;

pro OldGridMakeGridIndex,inx,st_time,ed_time,offset
  inx={OldGridInx, st_time: st_time, ed_time: ed_time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridLoadInx
;
; PURPOSE:
;       Read an index to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridLoadInx(unit,inx)
;
;       This function reads an index to a grid file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridLoadInx,unit,inx

ON_IOERROR,iofail
   
  syr=0
  smo=0
  sdy=0
  shr=0
  smt=0
  ssc=0
 
  eyr=0
  emo=0
  edy=0
  ehr=0
  emt=0
  esc=0
  offset=0L
  s=0
  c=0L

  st_time=0.0D
  ed_time=0.0D

  while (s eq 0) do begin
    readf,unit,syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc,offset
    st_time=TimeYMDHMSToEpoch(syr,smo,sdy,shr,smt,ssc)
    ed_time=TimeYMDHMSToEpoch(eyr,emo,edy,ehr,emt,esc)
    OldGridMakeGridIndex,tinx,st_time,ed_time,offset
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
;       GridNativeGetTime
;
; PURPOSE:
;       Read a record from a grid file.
;       
;
; CALLING SEQUENCE:
;       status = GridNativeGetTime(unit,prm,st_time,ed_time)
;
;       This function reads a single record from the open file with
;       logical unit number, unit. 
;
;
;       The returned value is size in bytes of the record on success, 
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridNativeGetTime,unit,st_time,ed_time

  ON_IOERROR,iofail

  syr=0
  smo=0
  sdy=0
  shr=0
  smt=0
  ssc=0
 
  eyr=0
  emo=0
  edy=0
  ehr=0
  emt=0
  esc=0

  point_lun,-unit,sptr
 
  readf,unit,syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc
  st_time=TimeYMDHMSToEpoch(syr,smo,sdy,shr,smt,ssc)
  ed_time=TimeYMDHMSToEpoch(eyr,emo,edy,ehr,emt,esc)

  nblk=0
  nprm=0
  npnt=0

  readf,unit,nblk
  tmp=''
  for n=0,nblk-1 do begin 
    readf,unit,npnt,nprm
    for x=0,npnt+2 do begin
      readf,unit,tmp  
    endfor
  endfor
 
  point_lun,-unit,eptr
  return,eptr-sptr  

iofail:
  return,-1 
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GridNativeSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = GridNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a grdmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function OldGridNativeSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  if (n_elements(inx) ne 0) then begin
   
    rec=0L
    prec=-1L
    num=n_elements(inx)
    stime=inx[0].st_time
    etime=inx[num-1].ed_time
   
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
      if (inx[rec].st_time eq tval) then break
      if (inx[rec].st_time lt tval) then begin
         srec=rec
         stime=inx[rec].st_time
      endif else begin
         erec=rec
         etime=inx[rec].ed_time
      endelse
    endrep until (prec eq rec)
    atme=inx[rec].st_time
    point_lun,unit,inx[rec].offset
    return,0
  endif else begin
     st_time=0.0D
     ed_time=0.0D
     s=OldGridNativeGetTime(unit,st_time,ed_time)
     if (s ne -1) then begin
       if (st_time gt tval) then point_lun,unit,0 $
       else begin
          stat=fstat(unit)
          fptr=stat.cur_ptr
      endelse
     endif else point_lun,unit,0
     atme=st_time
     repeat begin
       stat=fstat(unit)
       s=OldGridNativeGetTime(unit,st_time,ed_time)
       if (s eq -1) then break
       if (tval ge st_time) then begin
         fptr=stat.cur_ptr
         atme=st_time
       endif
     endrep until (tval lt st_time)
     if (tval gt st_time) then return,-1
     point_lun,unit,fptr
    endelse
  return,0 
end






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridNativePoint
;
; PURPOSE:
;       Sets the file pointer position in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridNativePoint(unit,ptr)
;
;       This function sets the file pointer in a oldgrdmap file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridNativePoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridExternalRead
;
; PURPOSE:
;       Read a record from a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = OldGridExternalRead(unit,prm,stvec,gvec)
;
;       This function reads a single record from the open file with
;       logical unit number, unit.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function OldGridExternalRead,unit,prm,stvec,gvec,lib=lib

  stnum=0L
  vcnum=0L
  ptr=''

  GridMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  s=CALL_EXTERNAL(lib,'OldGridIDLRead', $
                  unit,stnum,vcnum,ptr)

  if s eq -1 then return, -1
 
  prm.stnum=stnum
  prm.vcnum=vcnum
  stvec=replicate(stvec,stnum)
  if (vcnum ne 0) then gvec=replicate(gvec,vcnum)

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  st=CALL_EXTERNAL(lib,'OldGridIDLDecode', $
                  ptr,prm,stvec,gvec)


  ptr=''
  return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridExternalWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridExternalWrite(unit,prm,stvec,gvec)
;
;       This function writes a single record to the open file with
;       logical unit number, unit.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;

function OldGridExternalWrite,unit,prm,stvec,gvec,lib=lib
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  s=CALL_EXTERNAL(lib,'OldGridIDLWrite', $
                  unit,prm,stvec,gvec)

  return, s
end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridExternalSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridExternalSeek(unit,yr,mo,dy,hr,mt,sc,atme,inx)
;
;       This function searchs for a record in a grid file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;


function OldGridExternalSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  atme=0.0D
  yrval=fix(yr)
  moval=fix(mo)
  dyval=fix(dy)
  hrval=fix(hr)
  mtval=fix(mt)
  scval=double(sc)
   
  if (n_elements(inx) ne 0) then begin
    inum=n_elements(inx)
    stval=double(inx.st_time[*])
    etval=double(inx.ed_time[*])
    oval=long(inx.offset[*])
    s=CALL_EXTERNAL(lib,'OldGridIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme,inum, $
                  stval,etval,oval) 
  endif else begin 
    s=CALL_EXTERNAL(lib,'OldGridIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme) 
  endelse

  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridExternalPoint
;
; PURPOSE:
;       Sets the file pointer position in a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldGridExternalPoint(unit,ptr,lib=library)
;
;       This function sets the file pointer in a grdmap file.
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridExternalPoint,unit,off,lib=lib

 if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')   
 offl=long(off)
 s=CALL_EXTERNAL(lib,'OldGridIDLPoint', $
                unit,offl) 
 return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridRead
;
; PURPOSE:
;       Read a record from a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridRead(unit,prm,stvec,gvec,lib=library,
;                            /native,/external)
;
;       This function reads a single record from the open file with
;       logical unit number.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This determines whether to use the native of call external 
;       reader.
;
;-----------------------------------------------------------------
;


function OldGridRead,unit,prm,stvec,gvec,lib=lib,$
                     native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=OldGridExternalRead(unit,prm,stvec,gvec,lib=lib) $
  else s=OldGridNativeRead(unit,prm,stvec,gvec)
  return, s
end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridWrite(unit,prm,stvec,gvec,lib=library,
;                              /native,/external)
;
;       This function writes a single record to the open file with
;       logical unit number, unit. 
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;       This determines whether to use the native of call external 
;       reader.
;
;-----------------------------------------------------------------
;


function OldGridWrite,unit,prm,stvec,gvec,lib=lib,native=native,$
                      external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=OldGridExternalWrite(unit,prm,stvec,gvec,lib=lib) $
  else s=OldGridNativeWrite(unit,prm,stvec,gvec)
  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=library,
;                        /native,/external)
;
;       This function searchs for a record in a grdmap file.
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


function OldGridSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib, $
                         native=native,external=external


  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=OldGridExternalSeek(unit,yr,mo,dy,hr,mt,sc,inx, $
                                        lib=lib,atme=atme) $
  else s=OldGridNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme)
  return, s
end






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldGridOpen
;
; PURPOSE:
;       Open a fitacf file.
;
;
; CALLING SEQUENCE:
;       unit = OldGridOpen(lib=library,/native,/external,/read,/write,/update)
;
;       This function opens a grid file.
;
;       The returned value is the logical unit number on success,
;       or  -1 for failure
;
;
;       This function is provided as a convenience and opens
;       the grdmap file using the appropriate flags to to open.
;
;-----------------------------------------------------------------
;

function OldGridOpen,fname,atme=atme,lib=lib, $
                 native=native,external=external, $
                 read=read,write=write,update=update

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')
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
;       OldGridPoint
;
; PURPOSE:
;       Sets the file pointer position in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldGridPoint(fitfp,ptr,lib=library)
;
;       This function sets the file pointer in a grid file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldGridPoint,unit,off,lib=lib
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDGRDIDL')   
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then return, OldGridNativePoint(unit,off) $
  else return, OldGridExternalPoint(unit,off,lib=lib)
end




