; genlib.pro
; ==========
; Author: K.Baker & R.J.Barnes
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
; cnv_mdhms_sec
; cnvtime
; cnv_sec_mdhms
; cnv_sec_mdhms_b
; split_path
;
; ---------------------------------------------------------------
; $Log: genlib.pro,v $
; Revision 1.1  2004/08/19 15:21:23  barnes
; Initial revision
;
; ---------------------------------------------------------------
; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       cnv_mdhms_sec
;
; PURPOSE:
;
;       convert time from the form year, month, day, hour, minute, sec
;       to seconds of the year
;
; CALLING SEQUENCE:
;
;	t = cnv_mdhms_sec(yr, month, day, hour, minute, sec)
;	   if the arguments are arrays, they must be the same size
;
;	   OR
;
;	t = cnv_mdhms_sec(time_array)
;	   where time_array is a 2-d intarr (6,n)
;	   the first dimension gives the year, month, day, hour, minute, sec
;
;---------------------------------------------------------------------
;

function cnv_mdhms_sec,yr,mo,day,hr,mn,sc

  if n_params() eq 1 then begin
    sz = size(yr)
    if (sz(0) eq 1) then begin
      y = yr(0)
      m = yr(1)
      d = yr(2)
      h = yr(3)
      n = yr(4)
      s = yr(5)
    endif else begin
      if (sz(1) eq 6) then begin
        y=intarr(sz(2))
	m=y & d=y & h=y & n=y & s=y
	y = yr(0,*)
	m = yr(1,*)
	d = yr(2,*)
	h = yr(3,*)
	n = yr(4,*)
	s = yr(5,*)
      endif else begin 
        if sz(2) eq 6 then begin
	  y=intarr(sz(1))
	  m=y & d=y & h=y & n=y & s=y
	  y= yr(*,0)
	  m= yr(*,1)
	  d= yr(*,2)
	  h= yr(*,3)
	  n= yr(*,4)
	  s= yr(*,5)
        endif else begin
          print,"input array must be 6 x n  or n x 6"
          help,yr
          return,-1
	endelse
      endelse
    endelse
  endif else begin
    y = yr
    m = mo
    d = day
    h = hr
    n = mn
    s = sc
  endelse

  t=TimeYMDHMSToYrsec(y,m,d,h,n,s)
  return,t

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	cnvtime
;
; PURPOSE:
;
; 	This provides an alternate entry point to CNV_MDHMS_SEC
;
;----------------------------------------------------------------
;

function cnvtime,yr,mo,dy,hr,mn,sc
  return,cnv_mdhms_sec(yr,mo,dy,hr,mn,sc)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	cnv_sec_mdhms
;
; PURPOSE:
;
;	Convert the time in seconds of the year to the form month, day
;	hour, minutes, and seconds
;
; CALLING SEQUENCE:
;
;	status = cnv_sec_mdhms(yr, mo, day, hour, minute, sec, yr_secs)
;
;	All the arguments must be given. yr must be assigned a value (this
;	determines whether you are in  aleap year or not).  Mo, day, hour,
;	minute and sec must be declared to be normal integers and yr_secs
;	must be given a value and must be a long integer.
;
;	The status will be 0 for success and -1 for failure
;
;-----------------------------------------------------------------
;

function cnv_sec_mdhms,yr,mo,dy,hr,mn,sec,seconds
  if n_elements(seconds) eq 1 then begin
    s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mn,sec,seconds)
    return,0
  endif else begin

    for i = 0,n_elements(seconds)-1 do begin
      s=TimeYrsecToYMDHMS(yr,mov,dyv,hrv,mnv,scv,seconds[i])
      mo[i]=mov
      dy[i]=dyv
      hr[i]=hrv
      mn[i]=mnv
      sec[i]=scv
    endfor
  endelse
  return,0
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+ 
; NAME:
;	cnv_sec_mdhms_b
;
; PURPOSE:
;
; 	This routine provide upward compatibility for older software
;	that used this function.  Newer software should simply use
;	CNV_SEC_MDHMS, which can now handle arrays of times
;
;-----------------------------------------------------------
;


function cnv_sec_mdhms_b,tar,count,yr
  mo = intarr(count)
  dy = mo
  hr = mo
  mn = mo
  sc = mo
  status=cnv_sec_mdhms(yr,mo,dy,hr,mn,sc,tar)
  mdhms=intarr(5,count)
  mdhms(0,*)=mo
  mdhms(1,*)=dy
  mdhms(2,*)=hr
  mdhms(3,*)=mn
  mdhms(4,*)=sc
  return,mdhms
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	split_path
;
; PURPOSE:
;
;	Split a fully specified filename into its directory path, name
;       and extension.
;
; CALLING SEQUENCE:
;
;	splitpath,fname,path,name,ext
;
;	The full pathname is given by fname, the directory path, name
;       and extension are returned in the strings path,name, and ext.
;
;-----------------------------------------------------------------
;

pro splitpath,fname,path,name,ext 

  sep=byte(PATH_SEP())
  
  bytarr=byte(fname)
  n=n_elements(bytarr)-1

  for c=n,0,-1 do begin
    if (bytarr[c] eq sep) then break  
  endfor

  if (c ne -1) then begin 
     path=strmid(fname,0,c+1) 
     name=strmid(fname,c+1)
  endif else begin
     path=''
     name=fname
  endelse

  dot=strpos(name,'.')

  if (dot ne -1) then begin
    ext=strmid(name,dot+1)
    name=strmid(name,0,dot)
  endif else ext=''
end
 
