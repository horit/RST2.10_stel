; geop.pro
; ========
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
; GEOPExternalGeoGsm
; GEOPExternalGsmGeo
; GEOPExternalGseGsm
; GEOPExternalGsmGse
; GEOPExternalGsmSm
; GEOPExternalSmGsm
; GEOPGeoGsm
; GEOPGsmGeo
; GEOPGseGsm
; GEOPGsmGse
; GEOPGsmSm
; GEOPSmGsm
;
; ---------------------------------------------------------------
; $Log: geop.pro,v $
; Revision 1.1  2004/08/18 02:46:23  barnes
; Initial revision
;
; ---------------------------------------------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPExternalGeoGsm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;       
;
; CALLING SEQUENCE:
;       B=GEOPExternalGeoGsm(yr,mo,dy,hr,mt,sc,bgeo,bgsm,lib=lib)
;
;     
;-----------------------------------------------------------------
;

function GEOPExternalGeoGsm,year,month,day,hour,min,sec,bgeo,bgsm,lib=lib

   yr=fix(year)
   mo=fix(month)
   dy=fix(day)
   hr=fix(hour)
   mt=fix(min)
   sc=fix(sec)

   binp=double(bgeo)
   bout=double(bgsm)

   mode=0L
 
   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
   s=CALL_EXTERNAL(lib,'GEOPIDLGeoGsm',yr,mo,dy,hr,mt,sc,binp,bout) 
   bgsm[*]=bout[*]
   return, s
end


   


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPExternalGsmGeo
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;       
;
; CALLING SEQUENCE:
;       B=GEOPExternalGsmGeo(yr,mo,dy,hr,mt,sc,bgsm,bgeo,lib=lib)
;
;     
;-----------------------------------------------------------------
;

function GEOPExternalGsmGeo,year,month,day,hour,min,sec,bgsm,bgeo,lib=lib

   yr=fix(year)
   mo=fix(month)
   dy=fix(day)
   hr=fix(hour)
   mt=fix(min)
   sc=fix(sec)

   binp=double(bgsm)
   bout=double(bgeo)

   mode=0L
 
   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
   s=CALL_EXTERNAL(lib,'GEOPIDLGsmGeo',yr,mo,dy,hr,mt,sc,binp,bout) 

   bgeo[*]=bout[*]
   return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPExternalGseGsm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;       
;
; CALLING SEQUENCE:
;       B=GEOPExternalGseGsm(yr,mo,dy,hr,mt,sc,bgse,bgsm,lib=lib)
;
;     
;-----------------------------------------------------------------
;

function GEOPExternalGseGsm,year,month,day,hour,min,sec,bgse,bgsm,lib=lib

   yr=fix(year)
   mo=fix(month)
   dy=fix(day)
   hr=fix(hour)
   mt=fix(min)
   sc=fix(sec)

   binp=double(bgse)
   bout=double(bgsm)

   mode=0L
 
   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
   s=CALL_EXTERNAL(lib,'GEOPIDLGseGsm',yr,mo,dy,hr,mt,sc,binp,bout) 
   
   bgsm[*]=bout[*]
   return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPExternalGsmGse
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;       
;
; CALLING SEQUENCE:
;       B=GEOPExternalGsmGse(yr,mo,dy,hr,mt,sc,bgsm,bgse,lib=lib)
;
;     
;-----------------------------------------------------------------
;

function GEOPExternalGsmGse,year,month,day,hour,min,sec,bgsm,bgse,lib=lib

   yr=fix(year)
   mo=fix(month)
   dy=fix(day)
   hr=fix(hour)
   mt=fix(min)
   sc=fix(sec)

   binp=double(bgsm)
   bout=double(bgse)

   mode=0L
 
   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
   s=CALL_EXTERNAL(lib,'GEOPIDLGsmGse',yr,mo,dy,hr,mt,sc,binp,bout) 

   bgse[*]=bout[*]
   return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPExternalGsmSm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;       
;
; CALLING SEQUENCE:
;       B=GEOPExternalGsmSm(yr,mo,dy,hr,mt,sc,bgsm,bsm,lib=lib)
;
;     
;-----------------------------------------------------------------
;

function GEOPExternalGsmSm,year,month,day,hour,min,sec,bgsm,bsm,lib=lib

   yr=fix(year)
   mo=fix(month)
   dy=fix(day)
   hr=fix(hour)
   mt=fix(min)
   sc=fix(sec)

   binp=double(bgsm)
   bout=double(bsm)

   mode=0L
 
   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
   s=CALL_EXTERNAL(lib,'GEOPIDLGsmSm',yr,mo,dy,hr,mt,sc,binp,bout) 

   bsm[*]=bout[*]
   return, s
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPExternalSmGsm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;       
;
; CALLING SEQUENCE:
;       B=GEOPExternalSmGsm(yr,mo,dy,hr,mt,sc,bsm,bgsm,lib=lib)
;
;     
;-----------------------------------------------------------------
;

function GEOPExternalSmGsm,year,month,day,hour,min,sec,bsm,bgsm,lib=lib

   yr=fix(year)
   mo=fix(month)
   dy=fix(day)
   hr=fix(hour)
   mt=fix(min)
   sc=fix(sec)

   binp=double(bsm)
   bout=double(bgsm)

   mode=0L
 
   if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
   s=CALL_EXTERNAL(lib,'GEOPIDLSmGsm',yr,mo,dy,hr,mt,sc,binp,bout) 
   
   bgsm[*]=bout[*]
   return, s
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPGeoGsm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;
;
; CALLING SEQUENCE:
;       B=GEOPGeoGsm(yr,mo,dy,hr,mt,sc,bgeo,bgsm,lib=lib)
;
;
;-----------------------------------------------------------------
;

function GEOPGeoGsm,year,month,day,hour,min,sec, bgeo,bgsm, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet
  else return, GEOPExternalGeoGsm(year,month,day,hour,min,sec, $
                                  bgeo,bgsm,lib=lib)
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPGsmGeo
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;
;
; CALLING SEQUENCE:
;       B=GEOPGsmGeo(yr,mo,dy,hr,mt,sc,bgsm,bgeo,lib=lib)
;
;
;-----------------------------------------------------------------
;

function GEOPGsmGeo,year,month,day,hour,min,sec, bgsm,bgeo, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet
  else return, GEOPExternalGsmGeo(year,month,day,hour,min,sec, $
                                  bgsm,bgeo,lib=lib)
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPGseGsm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;
;
; CALLING SEQUENCE:
;       B=GEOPGseGsm(yr,mo,dy,hr,mt,sc,bgse,bgsm,lib=lib)
;
;
;-----------------------------------------------------------------
;
    
function GEOPGseGsm,year,month,day,hour,min,sec, bgse,bgsm, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet
  else return, GEOPExternalGseGsm(year,month,day,hour,min,sec, $
                                  bgse,bgsm,lib=lib)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPGsmGse
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;
;
; CALLING SEQUENCE:
;       B=GEOPGsmGse(yr,mo,dy,hr,mt,sc,bgsm,bgse,lib=lib)
;
;
;-----------------------------------------------------------------
;



function GEOPGsmGse,year,month,day,hour,min,sec, bgsm,bgse, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet
  else return, GEOPExternalGsmGse(year,month,day,hour,min,sec, $
                                  bgsm,bgse,lib=lib)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPGsmSm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;
;
; CALLING SEQUENCE:
;       B=GEOPGsmSm(yr,mo,dy,hr,mt,sc,bgsm,bsm,lib=lib)
;
;
;-----------------------------------------------------------------
;

function GEOPGsmSm,year,month,day,hour,min,sec, bgsm,bsm, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet
  else return, GEOPExternalGsmSm(year,month,day,hour,min,sec, $
                                  bgsm,bsm,lib=lib)
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       GEOPSmGsm
;
; PURPOSE:
;       Call the geopack coordinate transform routines
;
;
; CALLING SEQUENCE:
;       B=GEOPSmGsm(yr,mo,dy,hr,mt,sc,bsm,bgsm,lib=lib)
;
;
;-----------------------------------------------------------------
;

function GEOPSmGsm,year,month,day,hour,min,sec,bsm,bgsm, $
                         lib=lib,native=native,external=external

  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_GEOPIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet
  else return, GEOPExternalSmGsm(year,month,day,hour,min,sec, $
                                  bsm,bgsm,lib=lib)
end
