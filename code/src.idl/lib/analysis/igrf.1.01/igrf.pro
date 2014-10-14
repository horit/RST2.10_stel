; igrf.pro
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
; IGRFModelExternalCall
; IGRFModelExternalMagCmp
; IGRFModelCall
; IGRFModelMagCmp
;
; ---------------------------------------------------------------
; $Log: igrf.pro,v $
; Revision 1.1  2004/08/18 02:30:37  barnes
; Initial revision
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IGRFModelExternalCall
;
; PURPOSE:
;       Call the IGRF model
;       
;
; CALLING SEQUENCE:
;       B=IGRFModelExternalCall(year,glat,glon,height)
;
;     
;-----------------------------------------------------------------
;


function IGRFModelExternalCall,year,glat,glon,height,lib=lib

  bx=0.0D
  by=0.0D
  bz=0.0D

  date=double(year)
  flat=double(glat)
  flon=double(glon)
  elev=double(height)

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_IGRFIDL')
  s=CALL_EXTERNAL(lib,'IGRFIDLCall',date,flat,flon,elev,bx,by,bz)  
  B = double([bx,by,bz])

  return,B
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IGRFModelExternalMagCmp
;
; PURPOSE:
;       Call the IGRF model
;       
;
; CALLING SEQUENCE:
;       B=IGRFExternalMagCmp(year,rho,glat,glon)
;
;     
;-----------------------------------------------------------------
;


function IGRFModelExternalMagCmp,year,rho,glat,glon,lib=lib

  bx=0.0D
  by=0.0D
  bz=0.0D
  bt=0.0D

  date=double(year)
  frho=double(rho)
  flat=double(glat)
  flon=double(glon)
 
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_IGRFIDL')
  s=CALL_EXTERNAL(lib,'IGRFIDLMagCmp',date,frho,flat,flon,bx,by,bz,bt)  
  B = double([bx,by,bz,bt])

  return,B
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IGRFModelCall
;
; PURPOSE:
;       Call the IGRF model
;       
;
; CALLING SEQUENCE:
;       B=IGRFModelCall(year,glat,glon,height)
;
;     
;-----------------------------------------------------------------
;


function IGRFModelCall,year,glat,glon,height,$
                       lib=lib,native=native,external=external
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_IGRFIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1  $ ; not implemented yet 
  else return, IGRFModelExternalCall(year,glat,glon,height,lib=lib) 
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       IGRFModelMagCmp
;
; PURPOSE:
;       Call the IGRF model
;       
;
; CALLING SEQUENCE:
;       B=IGRFModelMagCmp(year,rho,glat,glon)
;
;     
;-----------------------------------------------------------------
;

function IGRFModelMagCmp,year,rho,glat,glon, $
                         lib=lib,native=native,external=external
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_IGRFIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1

  if (mode eq 1) then return,-1 $ ; not implemented yet 
  else return, IGRFModelExternalMagCmp(year,rho,glat,glon,lib=lib) 
end
