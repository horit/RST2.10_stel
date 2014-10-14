; aacgmlib.pro
; ============
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
; cnvcoord
; mlt
;
; ---------------------------------------------------------------
; $Log: aacgmlib.pro,v $
; Revision 1.1  2004/08/19 15:21:23  barnes
; Initial revision
;
; ---------------------------------------------------------------
; 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       cnvcoord
;
; PURPOSE:
;
;       Create a structure to store fitACF data.
;       
;
; CALLING SEQUENCE:
;
;	  pos = cnvcoord(inpos,[inlong],[height],[/GEO], [MODEL=year])
;
;	     the routine can be called either with a 3-element floating
;	     point array giving the input latitude, longitude and height
;	     or it can be called with 3 separate floating point values
;	     giving the same inputs.  The default conversion is from
;	     geographic to PACE geomagnetic coordinates.  If the keyword
;	     GEO is set (/GEO) then the conversion is from magnetic to
;	     geographic.  The optional keyword MODEL can be used
;            to select a particular magnetic coordinates model.
;            It should be set to the year of the model (for example,
;            MODEL=1990).  If the model is not specified, the most
;            recent model is used by default.  Once a model has
;            been specified it will continue to be used on subsequent
;            calls, even if the keyword is not included in the call.
;
;	     The input array can also be given in the form 
;		inpos(3,d1,d2,. . .)
;	     The output array will be in the same form.
;----------------------------------------------------------------------------



function cnvcoord, in1, in2, in3, geo = geo, model = model

  common aacgm_common, modyr

;  if (keyword_set(model)) then myear = model else myear = 1995
  if (keyword_set(model)) then myear = model else myear = 2000
  myear = fix(myear)
  if (n_params() GE 3) then inp = float([in1,in2,in3]) $
    else inp = float(in1)
  if (n_elements(inp) MOD 3 NE 0) then begin
    print,'input position must be fltarr(3) [lat,long,height]'
    return,[0,0,0]
  end

  if (n_elements(modyr) eq 0) then modyr=0
  
  if (modyr ne myear) then begin
    modyr=myear
    prefix=getenv('AACGM_DAT_PREFIX')
    fname=prefix+string(format='(I4.4)',modyr)+'.asc'
    openr, unit, fname, /GET_LUN,/STDIO
    s=AACGMLoadCoef(unit)
    free_lun,unit
    
  endif

  
  err=0
  s0 = size(inp)
  tmp = reform(inp,3,n_elements(inp)/3)
  outpos=tmp
  s1 = size(tmp)
  for j=0,s1(2)-1 do begin
    invec = fltarr(3)
    invec(0:2) = tmp(0:2,j)
    outvec = invec
    ret_val=AACGMConvert(invec[0],invec[1],invec[2], $
                         olat,olon,rad,geo=geo)
    outvec[0]=olat
    outvec[1]=olon
    outvec[2]=rad
    
    if (ret_val NE 0) then begin 
      print,"cnvcoord error = ",ret_val,err
      return,0
    endif
    outpos(*,j)=outvec
  endfor
  outpos=reform(outpos,s0(1:n_elements(s0)-3))
  return,outpos
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;	mlt
;
; PURPOSE:
;
;	convert UT time to MLT
;
; CALLING SEQUENCE:
;
;	  mt = mlt(year, ut_seconds, mag_long)
;
;		inputs:  year, time in seconds from Jan. 1 at 00;00:00 UT
;		         magnetic longitude of the observation point
;	
;	        the time in seconds can be found with routine "cnvtime"
;		the magnetic longitude of the obs. point can be found
;		  by using routine "cnvcoord"
;
;-----------------------------------------------------------------------------

function mlt, year, t, mlong
  year = fix(year)
  t = long(t)
  mlong = float(mlong)
  if (n_elements(t) EQ 1) then mt=0.0 else mt=fltarr(n_elements(t))
  if (n_elements(t) EQ 1) then $
    mt=AACGMConvertMLT(year,t,mlong) $
  else $
    for i=0,n_elements(t)-1 do mt(i)=AACGMConvertMLT(year,t(i),mlong)
    return, mt
end


