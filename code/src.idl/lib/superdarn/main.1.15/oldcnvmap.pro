; oldcnvmap.pro
; ============
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
; OldCnvMapNativeRead        
; OldCnvMapNativeWrite        
; OldCnvMapMakeCnvMapInx         
; OldCnvMapLoadInx          
; OldCnvMapGetTime          
; OldCnvMapNativeSeek     
; OldCnvMapNativePoint   
; OldCnvMapExternalRead      
; OldCnvMapExternalWrite      
; OldCnvMapExternalSeek 
; OldCnvMapExternalPoint     
; OldCnvMapRead               
; OldCnvMapWrite             
; OldCnvMapSeek            
; OldCnvMapOpen              
; OldCnvMapPoint
;
; ---------------------------------------------------------------
; $Log: oldcnvmap.pro,v $
; Revision 1.2  2004/11/04 20:49:29  barnes
; Fixed bug in the search algorithm.
;
; Revision 1.1  2004/11/04 20:18:43  barnes
; Initial revision
;
; ---------------------------------------------------------------
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapNativeRead
;
; PURPOSE:
;       Read a record from a grdmap file.
;       
;
; CALLING SEQUENCE:
;       status = OldCnvMapNativeRead(unit,prm,stvec,gvec,mvec,coef,bvec)
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

function OldCnvMapNativeRead,unit,prm,stvec,gvec,mvec,coef,bvec
 
  block=strarr(13,18)

  block[0,0:2]=['source','major_rev','minor_rev']
  block[1,0:2]=['Bx','By','Bz']
  block[2,0:4]=['doping_level','imf_flag','imf_delay','mod_dir','mod_mag']
  block[3,0:6]=['lon_shft','lat_shft','hemisphere','order','latmin', $
                'error_wt','model_wt']
  block[4,0:3]=['N','N+1','N+2','N+3']
  block[5,0:2]=['chi_sqr','chi_sqr_dat','rms_err']
  block[6,0:1]=['pot','pot_err']
  block[7,0:2]=['mlt_start','mlt_end','mlt_avg']
  block[8,0:3]=['gmlong','gmlat','kvect','vlos']
  block[9,0:1]=['bnd_lon','bnd_lat']

  block[10,0:17]=['st_id','chn','nvec','freq0','major_rev','minor_rev', $
                 'prog_id','noise_mean','noise_sd','gsct','v_min','v_max', $
                 'p_min','p_max','w_min','w_max','ve_min','ve_max']

  block[11,0:11]=['gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd','pwr','pwr_sd','wdt','wdt_sd']
  block[12,0:7]=['gmlong','gmlat','kvect','st_id','chn','grid_index', $
              'vlos','vlos_sd']


  ON_IOERROR,iofail

  CnvMapMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec
  GridMakeGVec,mvec
  coef=0.0D
  CnvMapMakeBnd,bvec 

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

  stid=0
  chn=0
  nvec=0
  freq=0.0
  major_rev=0
  minor_rev=0
  prog_id=0
  noise_mean=0.0
  noise_sd=0.0
  gsct=0
  v_min=0.0
  v_max=0.0
  p_min=0.0
  p_max=0.0
  w_min=0.0
  w_max=0.0
  ve_min=0.0
  ve_max=0.0

  gmlon=0.0
  gmlat=0.0 
  kvect=0.0
  index=0L
  vlos=0.0
  vlos_sd=0.0
  pwr=0.0
  pwr_sd=0.0
  wdt=0.0
  wdt_sd=0.0
 
  Bx=0.0
  By=0.0
  Bz=0.0

  lon_shft=0.0
  lat_shft=0.0
  hemisphere=0
  order=0
  latmin=0.0
  error_wt=0
  model_wt=0
 
  N0=0.0
  N1=0.0
  N2=0.0
  N3=0.0
 
  chi_sqr=0.0
  chi_sqr_dat=0.0
  rms_err=0.0
  
  pot=0.0
  pot_err=0.0
  mlt_start=0.0
  mlt_end=0.0
  mlt_avg=0.0
  bndlat=0.0
  bndlon=0.0

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
         tmp=''
         readf,unit,tmp
         str=strsplit(tmp,' ',/EXTRACT)
         if strcmp(str[0],'Null') eq 0 then prm.source=str[0]
         prm.major_rev=str[1]
         prm.minor_rev=str[2]
      END
      1: BEGIN
         readf,unit,Bx,By,Bz
         prm.Bx=Bx
         prm.By=By
         prm.Bz=Bz
      END
      2: BEGIN
         readf,unit,tmp
         str=strsplit(tmp,' ',/EXTRACT)
         prm.doping_level=fix(str[0])
         prm.imf_flag=fix(str[1])
         prm.imf_delay=fix(str[2])
         if strcmp(str[3],'Null') eq 0 then prm.imf_model[0]=str[3]
         if strcmp(str[4],'Null') eq 0 then prm.imf_model[1]=str[4]
      END
      3: BEGIN
         readf,unit,lon_shft,lat_shft,hemisphere,order,latmin,error_wt,model_wt
         prm.lon_shft=lon_shft
         prm.lat_shft=lat_shft
         prm.hemisphere=hemisphere
         prm.fit_order=order
         prm.latmin=latmin
         prm.error_wt=error_wt
         prm.model_wt=model_wt
      END
      4: BEGIN
        prm.coefnum=npnt
        coef=dblarr(prm.coefnum,4)
        for n=0,npnt-1 do begin
          readf,unit,N0,N1,N2,N3
          coef[n,0]=N0
          coef[n,1]=N1
          coef[n,2]=N2
          coef[n,3]=N3
        endfor
      END
      5: BEGIN
        readf,unit,chi_sqr,chi_sqr_dat,rms_err
        prm.chi_sqr=chi_sqr
        prm.chi_sqr_dat=chi_sqr_dat        
        prm.rms_err=rms_err
      END
      6: BEGIN
        readf,unit,pot,pot_err
        prm.pot_drop=pot
        prm.pot_drop_err=pot_err
        readf,unit,pot,pot_err
        prm.pot_max=pot
        prm.pot_max_err=pot_err
        readf,unit,pot,pot_err
        prm.pot_min=pot
        prm.pot_min_err=pot_err
      END
      7: BEGIN
         readf,unit,mlt_start,mlt_end,mlt_av
         prm.mlt.st=mlt_start
         prm.mlt.ed=mlt_end
         prm.mlt.av=mlt_av
      END
      8: BEGIN
        prm.modnum=npnt
        mvec=replicate(mvec,prm.modnum)
        for n=0,npnt-1 do begin
          readf,unit,gmlon,gmlat,kvect,vlos
          mvec[n].mlon=gmlon
          mvec[n].mlat=gmlat
          mvec[n].azm=kvect
          mvec[n].vel.median=vlos
        endfor   
      END
      9: BEGIN
        prm.bndnum=npnt
        bvec=replicate(bvec,prm.bndnum)
        for n=0,npnt-1 do begin
          readf,unit,gmlon,gmlat
          bvec[n].lon=gmlon
          bvec[n].lat=gmlat
        endfor   
      END
      10: BEGIN
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
      11: BEGIN
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
      12: BEGIN
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
;       OldCnvMapNativeWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;       
;
; CALLING SEQUENCE:
;       status = OldCnvMapNativeWrite(unit,prm,stvec,gvec)
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


function OldCnvMapNativeWrite,unit,prm,stvec,gvec,mvec,coef,bvec

 
 ON_IOERROR,iofail

  point_lun,-unit,sptr

  printf,unit,prm.stme.yr,prm.stme.mo,prm.stme.dy,prm.stme.hr,prm.stme.mt,$
              fix(prm.stme.sc),prm.etme.yr,prm.etme.mo,prm.etme.dy,$
              prm.etme.hr,prm.etme.mt,fix(prm.etme.sc),$
              format='(1I5,5I3.2,1I5,5I3.2)'
  printf,unit,12,format='(1I0)'

  printf,unit,1,3,format='(1I0," ",1I0)'
  printf,unit,'source','major_rev','minor_rev',format='(3A15)'
  printf,unit,'name','count','count',format='(3A15)'
  printf,unit,'text','int','int',format='(3A15)'

  source=prm.source
  if strlen(source) eq 0 then source='Null'
  printf,unit,source,prm.major_rev,prm.minor_rev,format='(1A15,2I15)'

  printf,unit,1,3,format='(1I0," ",1I0)'
  printf,unit,'Bx','By','Bz',format='(3A15)'
  printf,unit,'nT','nT','nT',format='(3A15)'
  printf,unit,'float','float','float',format='(3A15)'
  printf,unit,float(prm.Bx),float(prm.By),float(prm.Bz),format='(3G15)'
 

  mod_mag=prm.imf_model[1]
  mod_dir=prm.imf_model[0]
  if strlen(mod_mag) eq 0 then mod_mag='Null'
  if strlen(mod_dir) eq 0 then mod_dir='Null'


  printf,unit,1,5,format='(1I0," ",1I0)'
  printf,unit,'doping_level','imf_flag','imf_delay','mod_dir','mod_mag', $
              format='(5A15)'
  printf,unit,'count','count','minute','name','name',format='(5A15)'
  printf,unit,'int','int','int','text','text',format='(5A15)'
  printf,unit,prm.doping_level,prm.imf_flag,prm.imf_delay, $
         mod_dir,mod_mag,format='(3I15,2A15)'

  printf,unit,1,7,format='(1I0," ",1I0)'
  printf,unit,'lon_shft','lat_shft','hemisphere','order','latmin', $
              'error_wt','model_wt',format='(7A15)'
  printf,unit,'degrees','degrees','count','count','degrees', $
              'count','count',format='(7A15)'
  printf,unit,'float','float','int','int','float','int','int',format='(7A15)'
  printf,unit,float(prm.lon_shft),float(prm.lat_shft),prm.hemisphere, $
         prm.fit_order,float(prm.latmin),prm.error_wt,prm.model_wt, $
         format='(2G15,2I15,1G15,2I15)'


  printf,unit,prm.coefnum,4,format='(1I0," ",1I0)'
  printf,unit,'N','N+1','N+2','N+3',format='(4A15)'
  printf,unit,'none','none','none','none',format='(4A15)'
  printf,unit,'float','float','float','float',format='(4A15)'

  for n=0,prm.coefnum-1 do begin
    printf,unit,float(coef[n,0]),float(coef[n,1]),float(coef[n,2]), $
          float(coef[n,3]),format='(4G15)'
  endfor


  printf,unit,1,3,format='(1I0," ",1I0)'
  printf,unit,'chi_sqr','chi_sqr_dat','rms_err',format='(3A15)'
  printf,unit,'kv','kv','kv',format='(3A15)'
  printf,unit,'float','float','float',format='(3A15)'
  printf,unit,float(prm.chi_sqr),float(prm.chi_sqr_dat), $
              float(prm.rms_err),format='(3G15)'


  printf,unit,3,2,format='(1I0," ",1I0)'
  printf,unit,'pot','pot_err',format='(2A15)'
  printf,unit,'kv','kv',format='(2A15)'
  printf,unit,'float','float',format='(2A15)'
  printf,unit,float(prm.pot_drop),float(prm.pot_drop_err), format='(2G15)'
  printf,unit,float(prm.pot_max),float(prm.pot_max_err), format='(2G15)'
  printf,unit,float(prm.pot_min),float(prm.pot_min_err), format='(2G15)'

  printf,unit,1,3,format='(1I0," ",1I0)'
  printf,unit,'mlt_start','mlt_end','mlt_avg',format='(3A15)'
  printf,unit,'dec_hour','dec_hour','dec_hour',format='(3A15)'
  printf,unit,'float','float','float',format='(3A15)'
  printf,unit,float(prm.mlt.st),float(prm.mlt.ed), $
              float(prm.mlt.av),format='(3G15)'

  printf,unit,prm.modnum,4,format='(1I0," ",1I0)'
  printf,unit,'gmlong','gmlat','kvect','vlos', $
              format='(4A15)'
  printf,unit,'degrees','degrees','degrees','m/sec', $
              format='(4A15)'
  printf,unit,'float','float','float','float', $
              format='(8A15)'
  for n=0,prm.modnum-1 do begin
    printf,unit,mvec[n].mlon,mvec[n].mlat,mvec[n].azm, $
        mvec[n].vel.median,format='(4G15)'
  endfor
  
  printf,unit,prm.bndnum,2,format='(1I0," ",1I0)'
  printf,unit,'bnd_lon','bnd_lat',format='(2A15)'
  printf,unit,'degrees','degrees',format='(2A15)'
  printf,unit,'float','float',format='(2A15)'

  for n=0,prm.bndnum-1 do begin
    printf,unit,bvec[n].lon,bvec[n].lat,format='(2G15)'
  endfor

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
;       OldCnvMapMakeCnvMapInx
;
; PURPOSE:
;       Create a structure to store grdmap index.
;
;
; CALLING SEQUENCE:
;       OldCnvMapMakeCnvMapInx,inx
;
;       This procedure creates a structure to store the grdmap index.
;
;
;-----------------------------------------------------------------
;

pro OldCnvMapMakeCnvMapIndex,inx,time,offset
   inx={OldCnvMapInx, st_time: st_time, ed_time: ed_time, offset: offset}
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapLoadInx
;
; PURPOSE:
;       Read an index to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapLoadInx(unit,inx)
;
;       This function reads an index to a grdmap file.
;
;       The returned value is size in bytes of the record on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldCnvMapLoadInx,unit,inx

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
;       OldCnvMapNativeGetTime
;
; PURPOSE:
;       Extract the time from a cnvmap record.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapNativeGetTime(unit,prm,st_time,ed_time)
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

function OldCnvMapNativeGetTime,unit,st_time,ed_time
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
;       OldCnvMapNativeSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atime)
;
;       This function searchs for a record in a grdmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;


function OldCnvMapNativeSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme

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
     s=OldCnvMapNativeGetTime(unit,st_time,ed_time)
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
       s=OldCnvMapNativeGetTime(unit,st_time,ed_time)
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
;       OldCnvMapNativePoint
;
; PURPOSE:
;       Sets the file pointer position in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapNativePoint(unit,ptr)
;
;       This function sets the file pointer in a grdmap file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldCnvMapNativePoint,unit,off

ON_IOERROR,iofail

  point_lun,unit,off
  return,off

iofail:
  return, -1

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapExternalRead
;
; PURPOSE:
;       Read a record from a fitacf file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapExternalRead(unit,prm,stvec,gvec,mvec,coef,bvec)
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

function OldCnvMapExternalRead,unit,prm,stvec,gvec,mvec,coef,bvec,lib=lib

  stnum=0L
  vcnum=0L
  modnum=0L
  coefnum=0L
  bndnum=0L
  ptr=''

  CnvMapMakePrm,prm 
  GridMakeStVec,stvec
  GridMakeGVec,gvec
  GridMakeGvec,mvec
  coef=0.0D
  CnvMapMakeBnd,bvec

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
  s=CALL_EXTERNAL(lib,'OldCnvMapIDLRead', $
                  unit,stnum,vcnum,modnum,coefnum,bndnum,ptr)

  if s eq -1 then return, -1
 
  prm.stnum=stnum
  prm.vcnum=vcnum
  prm.modnum=modnum
  prm.coefnum=coefnum
  prm.bndnum=bndnum
  stvec=replicate(stvec,stnum)
  if (vcnum ne 0) then gvec=replicate(gvec,vcnum)
  if (modnum ne 0) then mvec=replicate(mvec,modnum)
  if (coefnum ne 0 ) then coef=dblarr(coefnum,4)
  if (bndnum ne 0) then bvec=replicate(bvec,bndnum)

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
  st=CALL_EXTERNAL(lib,'OldCnvMapIDLDecode', $
                  ptr,prm,stvec,gvec,mvec,coef,bvec)

  ptr=''
  return, s
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapExternalWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapExternalWrite(unit,prm,stvec,gvec,mvec,coef,bvec)
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

function OldCnvMapExternalWrite,unit,prm,stvec,gvec,mvec,coef,bvec,lib=lib
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
  s=CALL_EXTERNAL(lib,'OldCnvMapIDLWrite', $
                  unit,prm,stvec,gvec,mvec,coef,bvec)

  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapExternalSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapExternalSeek(unit,yr,mo,dy,hr,mt,sc,atme,inx)
;
;       This function searchs for a record in a cnvmap file.
;
;       The returned value is zero on success,
;       or  -1 for failure
;
;       This routine uses the call external function to access the 
;       C read routines.
;
;-----------------------------------------------------------------
;


function OldCnvMapExternalSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
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
    s=CALL_EXTERNAL(lib,'OldCnvMapIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme,inum, $
                  stval,etval,oval) 
  endif else begin 
    s=CALL_EXTERNAL(lib,'OldCnvMapIDLSeek', $
                  unit,yrval,moval,dyval,hrval,mtval,scval,atme) 
  endelse

  return, s

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapExternalPoint
;
; PURPOSE:
;       Sets the file pointer position in a fit file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapExternalPoint(unit,ptr,lib=library)
;
;       This function sets the file pointer in a grdmap file.
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldCnvMapExternalPoint,unit,off,lib=lib

 if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')   
 offl=long(off)
 s=CALL_EXTERNAL(lib,'OldCnvMapIDLPoint', $
                unit,offl) 
 return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapRead
;
; PURPOSE:
;       Read a record from a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapRead(unit,prm,stvec,gvec,mvec,coef,bvec,
;                              lib=library,/native,/external)
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


function OldCnvMapRead,unit,prm,stvec,gvec,mvec,coef,bvec, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=OldCnvMapExternalRead(unit,prm,stvec,gvec, $
                                           mvec,coef,bvec,lib=lib) $
  else s=OldCnvMapNativeRead(unit,prm,stvec,gvec,mvec,coef,bvec)
  return, s
end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapWrite
;
; PURPOSE:
;       Write a record to a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapWrite(unit,prm,stvec,gvec,mvec,coef,bvec,
;                            lib=library,/native,/external)
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


function OldCnvMapWrite,unit,prm,stvec,gvec,mvec,coef,bvec, $
                     lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=OldCnvMapExternalWrite(unit,prm,stvec,gvec,mvec, $
                                            coef,bvec,lib=lib) $
  else s=OldCnvMapNativeWrite(unit,prm,stvec,gvec,mvec,coef,bvec)
  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       CnvMapSeek
;
; PURPOSE:
;       Find a record in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,
;                              lib=library,/native,/external)
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


function OldCnvMapSeek,unit,yr,mo,dy,hr,mt,sc,inx,atme=atme,lib=lib, $
                         native=native,external=external


  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  
  if (mode eq 2) then s=OldCnvMapExternalSeek(unit,yr,mo,dy,hr,mt,sc,inx, $
                                        lib=lib,atme=atme) $
  else s=OldCnvMapNativeSeek(unit,yr,mo,dy,hr,mt,sc,inx,atme=atme)
  return, s
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       OldCnvMapOpen
;
; PURPOSE:
;       Open a fitacf file.
;
;
; CALLING SEQUENCE:
;       unit = OldCnvMapOpen(lib=library,
;                            /native,/external,/read,/write,/update)
;
;       This function opens a grdmap file.
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

function OldCnvMapOpen,fname,atme=atme,lib=lib, $
                 native=native,external=external, $
                 read=read,write=write,update=update

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')
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
;       OldCnvMapPoint
;
; PURPOSE:
;       Sets the file pointer position in a grdmap file.
;
;
; CALLING SEQUENCE:
;       status = OldCnvMapPoint(fitfp,ptr,lib=library)
;
;       This function sets the file pointer in a grdmap file.
;
;
;       The returned value is the offset on success,
;       or  -1 for failure
;
;-----------------------------------------------------------------
;

function OldCnvMapPoint,unit,off,lib=lib

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_OLDCNVMAPIDL')   
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1
  if (mode eq 1) then return, OldCnvMapNativePoint(unit,off) $
  else return, OldCnvMapExternalPoint(unit,off,lib=lib)

  

end
