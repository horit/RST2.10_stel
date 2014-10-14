; aacgmlib.pro
; ============
; Author: K.Baker, S.Shepherd & R.J.Barnes
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
; cnv_geo_gsm
; cnv_gsm_geo
; cnv_gse_gsm
; cnv_gsm_gse
; cnv_sm_gsm
; cnv_gsm_sm
;
; ---------------------------------------------------------------
; $Log: geoplib.pro,v $
; Revision 1.1  2004/08/19 15:21:23  barnes
; Initial revision
;
; ---------------------------------------------------------------
; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_geo_gsm
;
; PURPOSE:
;
;         Converts the GEO to GSM coordinate or GSM to GEO.
;
; CALLING SEQUENCE:
;
;	  cnv_geo_gsm,syear,smonth,sday,shour,smin,ssec,bgeo,bgsm,s
;
; OR: 
;
;         cnv_geo_gsm,syear,smonth,sday,shour,smin,ssec,bgsm,bgeo,s,/rev
;
;         Converts from GEO to GSM or GSM to GEO for the specified
;         time. The arguments bgsm and bgeo are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_geo_gsm,syear,smonth,sday,shour,smin,ssec,bgeo,bgsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgeo = double(bgeo)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGeoGsm(syear,smonth,sday,shour,smin,ssec,bgeo,bgsm) $
   else  $
    s=GEOPGsmGeo(syear,smonth,sday,shour,smin,ssec,bgsm,bgeo) 

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gsm_geo
;
; PURPOSE:
;
;         Converts the GSM to GEO coordinate or GEO to GSM.
;
; CALLING SEQUENCE:
;
;	  cnv_gsm_geo,syear,smonth,sday,shour,smin,ssec,bgsm,bgeo,s
;
; OR: 
;
;         cnv_gsm_geo,syear,smonth,sday,shour,smin,ssec,bgeo,bgsm,s,/rev
;
;         Converts from GSM to GEO or GEO to GSM for the specified
;         time. The arguments bgsm and bgeo are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_gsm_geo,syear,smonth,sday,shour,smin,ssec,bgsm,geo,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgeo = double(bgeo)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGsmGeo(syear,smonth,sday,shour,smin,ssec,bgsm,bgeo) $
   else  $
    s=GEOPGeoGso(syear,smonth,sday,shour,smin,ssec,bgeo,bgsm) 

  return
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gse_gsm
;
; PURPOSE:
;
;         Converts the GSE to GSM coordinate or GSM to GSE.
;
; CALLING SEQUENCE:
;
;	  cnv_gse_gsm,syear,smonth,sday,shour,smin,ssec,bgse,bgsm,s
;
; OR: 
;
;         cnv_gse_gsm,syear,smonth,sday,shour,smin,ssec,bgsm,bgse,s,/rev
;
;         Converts from GSE to GSM or GSM to GSE for the specified
;         time. The arguments bgse and bgsm are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------





pro cnv_gse_gsm,syear,smonth,sday,shour,smin,ssec,bgse,bgsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgse = double(bgse)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGseGsm(syear,smonth,sday,shour,smin,ssec,bgse,bgsm) $
   else  $
    s=GEOPGsmGse(syear,smonth,sday,shour,smin,ssec,bgsm,bgse) 

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gsm_gse
;
; PURPOSE:
;
;         Converts the GSM to GSE coordinate or GSE to GSM.
;
; CALLING SEQUENCE:
;
;	  cnv_gsm_gse,syear,smonth,sday,shour,smin,ssec,bgsm,bgse,s
;
; OR: 
;
;         cnv_gsm_gse,syear,smonth,sday,shour,smin,ssec,bgse,bgsm,s,/rev
;
;         Converts from GSM to GSE or GSE to GSM for the specified
;         time. The arguments bgsm and bgse are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------





pro cnv_gsm_gse,syear,smonth,sday,shour,smin,ssec,bgsm,bgse,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bgse = double(bgse)
  bgsm = double(bgsm)


  if not keyword_set(rev) then $
    s=GEOPGsmGse(syear,smonth,sday,shour,smin,ssec,bgsm,bgse) $
   else  $
    s=GEOPGseGsm(syear,smonth,sday,shour,smin,ssec,bgse,bgsm) 

  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_gsm_sm
;
; PURPOSE:
;
;         Converts the GSM to SM coordinate or SM to GSM.
;
; CALLING SEQUENCE:
;
;	  cnv_gsm_sm,syear,smonth,sday,shour,smin,ssec,bgsm,bsm,s
;
; OR: 
;
;         cnv_gsm_sm,syear,smonth,sday,shour,smin,ssec,bsm,bgsm,s,/rev
;
;         Converts from GSM to SM or SM to GSM for the specified
;         time. The arguments bgsm and bsm are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_gsm_sm,syear,smonth,sday,shour,smin,ssec,bgsm,bsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bsm = double(bsm)
  bgsm = double(bgsm)

  if not keyword_set(rev) then $
    s=GEOPGsmSm(syear,smonth,sday,shour,smin,ssec,bgsm,bsm) $
   else  $
    s=GEOPSmGsm(syear,smonth,sday,shour,smin,ssec,bsm,bgsm) 
  return
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; NAME:
;       cnv_sm_gsm
;
; PURPOSE:
;
;         Converts the SM to GSM coordinate or GSM to SM.
;
; CALLING SEQUENCE:
;
;	  cnv_sm_gsm,syear,smonth,sday,shour,smin,ssec,bsm,bgsm,s
;
; OR: 
;
;         cnv_sm_gsm,syear,smonth,sday,shour,smin,ssec,bgsm,bsm,s,/rev
;
;         Converts from SM to GSM or GSM to SM for the specified
;         time. The arguments bsm and bgsm are three element, double arrays
;         containing the coordinates of the point.
;         If the keyword /rev is specified the inverse transform is
;         performed.
;
;      
;         s  is 0 if everything is ok and a non-zero error code otherwise
;-------------------------------------------------------------------------



pro cnv_sm_gsm,syear,smonth,sday,shour,smin,ssec,bsm,bgsm,s,rev=rev

  syear = fix(syear)
  smonth = fix(smonth)
  sday = fix(sday)
  shour = fix(shour)
  smin = fix(smin)
  ssec = fix(ssec)
  bsm = double(bsm)
  bgsm = double(bgsm)

  if not keyword_set(rev) then $
    s=GEOPSmGsm(syear,smonth,sday,shour,smin,ssec,bsm,bgsm) $
   else  $
    s=GEOPGsmSm(syear,smonth,sday,shour,smin,ssec,bgsm,bsm) 

  return
end
