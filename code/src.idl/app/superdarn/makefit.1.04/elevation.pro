; elevation.pro
; =============
; Author: R.J.Barnes - Based on C code by K.Baker
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
; FitACFElevation 


; ---------------------------------------------------------------
; $Log: elevation.pro,v $
; Revision 1.2  2006/04/13 16:13:29  barnes
; Modification to deal with radars with extra beams.
;
; Revision 1.1  2005/07/01 00:57:20  barnes
; Initial revision
;
; Revision 1.1  2005/07/01 00:31:17  barnes
; Initial revision
;
; ---------------------------------------------------------------



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       FitACFElevation
;
; PURPOSE:
;       Unknown.
;       
;
; CALLING SEQUENCE:
;        angle=FitACFElevation(prm,range,phi0)
;
;
;
;-----------------------------------------------------------------
;



function FitACFElevation,prm,range,phi0

  C=2.997d8

  if (not finite(phi0)) then return, -!VALUES.D_INFINITY

  antenna_separation=sqrt(prm.interfer[1]*prm.interfer[1] + $ 
			  prm.interfer[0]*prm.interfer[0] + $
	                  prm.interfer[2]*prm.interfer[2])

  elev_corr= prm.phidiff*asin(prm.interfer[2]/antenna_separation)

  if (prm.interfer[1] gt 0.0) then phi_sign=1.0d $ ; in front of main antenna 
  else begin

; interferometer behind main antenna */

    phi_sign= -1.0d
    elev_corr= -elev_corr
  endelse
  offset=0.0d
  offset=(prm.maxbeam/2.0d)-0.5d

  phi= prm.bmsep*(prm.bmnum - offset)* !DPI/ 180.0d;
  c_phi= cos( phi);
  k= 2 * !DPI * prm.tfreq * 1000.0d/C;

; the phase difference phi0 is between -pi and +pi and gets positive,  
; if the signal from the interferometer antenna arrives earlier at the 
; receiver than the signal from the main antenna. 
; If the cable to the interferometer is shorter than the one to 
; the main antenna, than the signal from the interferometer     
; antenna arrives earlier. tdiff < 0  --> dchi_cable > 0        

  dchi_cable= - 2* !DPI * prm.tfreq * 1000.0d * prm.tdiff * 1.0d-6

; If the interferometer antenna is in front of the main antenna 
; then lower elevation angles correspond to earlier arrival     
; and greater phase difference.     
; If the interferometer antenna is behind of the main antenna   
; then lower elevation angles correspond to later arrival       
; and smaller phase difference     

  chi_max= phi_sign* k* antenna_separation* c_phi + dchi_cable

; change phi0 by multiples of twopi, until it is in the range   
; (chi_max - twopi) to chi_max (interferometer in front)       
; or chi_max to (chi_max + twopi) (interferometer in the back)  

  phi_temp= phi0 + 2*!DPI* floor( (chi_max - phi0)/ (2*!DPI))

  if (phi_sign lt 0.0) then phi_temp= phi_temp + (2*!DPI)

; subtract the cable effect 

  psi= phi_temp - dchi_cable
  theta= psi/ (k* antenna_separation)
  theta= (c_phi* c_phi - theta* theta)
 
; set elevation angle to 0 for out of range values

  if ( (theta lt 0.0) or (abs( theta) gt 1.0) ) then theta= -elev_corr $
  else theta= asin( sqrt( theta))

  return,180.0d* (theta + elev_corr)/ !DPI
end
