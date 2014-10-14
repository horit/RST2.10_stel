; igrflib.pro
; ===========
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
; magcmp
; igrfcall
;
; ---------------------------------------------------------------
; $Log: igrflib.pro,v $
; Revision 1.1  2004/08/19 15:50:28  barnes
; Initial revision
;
; Revision 1.1  2004/08/19 15:21:23  barnes
; Initial revision
;
; ---------------------------------------------------------------
; 



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;           magcmp
;
; PURPOSE:  
;
;           use IGRF magnetic field models to return magnetic field
;	    components.
;
; CALLING SEQUENCE:
;
;	B = magcmp(lat,lon,elev,date)
;	     where date is given in decimal years
;	     lat, lon are the geographic latitude and longitude in degrees
;	     elev is the elevation.
;
;-----------------------------------------------------------------------
;

function magcmp,lat,lon,elev,date,ifrst
  bvec=IGRFModelMagCmp(date,elev,lat,lon)
  return,bvec
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME: 
;
;           igrfcall
;
; PURPOSE:  
;
;           use IGRF magnetic field models to return magnetic field
;	    components.
;
; CALLING SEQUENCE:
;
;	B = igrfcall(date,glat,glong,alt)
;	     where date is given in decimal years
;	     glat, glong are the geographic latitude and longitude in degrees
;	     alt is the altitude in km.
;
;-----------------------------------------------------------------------
;

function igrfcall,date,glat,glong,alt
  if (n_params() NE 4) then begin
    print,"Invalid number of parameters in call to IGRFCALL"
    print,"USE: B=igrfcall(date,glat,glong,altitude)"
    return,[0,0,0]
  endif
	
  temp_date = date

  if (temp_date LT 1900) then temp_date = temp_date + 1900   
  igrf_year = double(temp_date)
  flat = double(glat)
  flong = double(glong)
  height = double(alt)
 
  b=IGRFModelCall(igrf_year,flat,flong,height)

  return,B
end
