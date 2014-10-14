; radar.pro
; =======
; Author: R.J.Barnes & D.Andre
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
;  RadarMakeSite
;  RadarMakeRadar
;  RadarLoad
;  RadarLoadHardware
;  RadarEpochGetSite
;  RadarYMDHMSGetSite
;  RadarGetRadar
;  RadarNativePos
;  RadarNativePosGS
;  RadarExternalPos
;  RadarExternalPosGS
;  RadarExternalGeochrt
;  RadarPos
;  RadarPosGS
;  RadarGeochrt
;
; Private Functions:
; ------------------
;
; RadarSlantRange
; RadarGeoTGC
; RadarFldPnt
; RadarGeoCnvrt
; RadarFldPnth
; RadarFldPnthGS

; ---------------------------------------------------------------
; $Log: radar.pro,v $
; Revision 1.9  2007/01/22 15:22:00  code
; Modified  RadarExternalGeochrt to get the number of beams and ranges from
; the hardware information.
;
; Revision 1.8  2006/07/18 18:19:21  code
; Fixed bug in determining the hardware (again).
;
; Revision 1.7  2006/07/07 20:15:06  code
; Fixed bugs in getting the correct site info.
;
; Revision 1.6  2006/07/07 20:12:01  code
; Fixed bug in getting the site location.
;
; Revision 1.5  2006/04/13 15:55:39  barnes
; Fixed bug in reading radar hardware.
;
; Revision 1.4  2006/03/16 20:26:19  barnes
; Fixed bug in structure definition.
;
; Revision 1.3  2006/03/15 16:31:27  barnes
; Modification to deal with radars with extra beams.
;
; Revision 1.2  2005/04/12 21:17:48  barnes
; Added Dieter Andre's RadarGeochrt routine.
; Note: As yet we do not have an IDL implementation of the IGRF
; so a native version of this routine is not yet possible.
;
; Revision 1.1  2004/08/09 19:36:16  barnes
; Initial revision
;
; ---------------------------------------------------------------
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeSite
;
; PURPOSE:
;       Create a structure to store the hardware tables
;
;
; CALLING SEQUENCE:
;       RadarMaksSite,site
;
;       This procedure creates a structure to store the hardware table,
;       the structure is returns in site.
;


pro RadarMakeSite,site

  site={RadarSite, $
         tval: 0.0D, $
         geolat: 0.0D, $
         geolon: 0.0D, $
         alt: 0.0D, $
         boresite: 0.0D, $
         bmsep:0.0D, $
         vdir: 0.0D, $
         atten: 0.0D, $
         tdiff: 0.0D, $
         phidiff: 0.0D, $
         interfer: dblarr(3), $
         recrise: 0.0D, $
         maxatten: 0L, $
         maxrange: 0L, $
         maxbeam: 0L $
     }
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarMakeRadar
;
; PURPOSE:
;       Create a structure to store the radar identification table
;
;
; CALLING SEQUENCE:
;       RadarMaksRadar,radar
;
;       This procedure creates a structure to store the radar identification,
;       table, the structure is returns in radar.
;




pro RadarMakeRadar,radar
    
 RadarMakeSite,site
 radar={Radar, $
       id: 0L, $
       status: 0L, $
       cnum: 0L, $
       code: strarr(8), $
       name: '', $
       operator: '', $
       hdwfname: '', $
       snum: 0L, $
       site: replicate(site,32) $
}    

end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarLoad
;
; PURPOSE:
;       Loads the radar identification table.
;       
;
; CALLING SEQUENCE:
;       radar = RadarLoad(unit)
;
;       This function reads the radar identification table from
;       the open file with logical unit number, unit
;
;
;       The returned value is the radar identification table or
;       zero if an error occurred.
;
;-----------------------------------------------------------------
;
 
function RadarLoad,unit

ON_IOERROR, iofail

   RadarMakeRadar, rad
   radar=replicate(rad,1)
   c=0

   id=0L
   status=0L
   txt=''

   repeat begin
      
      readf,unit,id,status,txt
     
      rad.id=id
      rad.status=status

      ; decode the text strings
      s=strpos(txt,'"')
      e=strpos(txt,'"',s+1)
      rad.name=strmid(txt,s+1,e-s-1)
      s=strpos(txt,'"',e+1)
      e=strpos(txt,'"',s+1)
      rad.operator=strmid(txt,s+1,e-s-1)

      s=strpos(txt,'"',e+1)
      e=strpos(txt,'"',s+1)
      rad.hdwfname=strmid(txt,s+1,e-s-1)
      n=0
      while (s ne -1) do begin
         s=strpos(txt,'"',e+1)
         if (s eq -1) then break
         e=strpos(txt,'"',s+1)
         rad.code[n]=strmid(txt,s+1,e-s-1)
         n=n+1
      endwhile   

      if (c ne 0) then radar=[radar,rad] $
      else radar[0]=rad
      c=c+1
   endrep until eof(unit)

iofail:
   return, radar
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarLoadHardware
;
; PURPOSE:
;       Loads the radar hardware tables
;       
;
; CALLING SEQUENCE:
;       s = RadarLoadHardware(radar)
;
;       This function reads in the radar hardware tables and
;       populates the structure radar. The structure should
;       be created using the LoadRadar function.
;
;       The returned value is zero
;
;-----------------------------------------------------------------
;


function RadarLoadHardware,radar,path=path
   txt=''
   for n=0,N_ELEMENTS(radar)-1 do begin
      if (KEYWORD_SET(path)) then fname=path+radar[n].hdwfname $
      else fname=radar[n].hdwfname
      
      ON_IOERROR, trap
      openr,unit,fname,/GET_LUN
      x=0

      while (~eof(unit)) do begin
        readf,unit,txt
        s=strpos(txt,'#')
        if (s ne -1) then continue
        if (strlen(txt) eq 0) then continue
        tmp=dblarr(19)
        reads,txt,tmp
        if (tmp[0] eq radar[n].id) then begin
          if (tmp[1] eq 2999) then radar[n].site[x].tval=-1 $
          else begin
            yr=tmp[1]
            s=TimeYrsecToYMDHMS(yr,mo,dy,hr,mt,sc,tmp[2])
            radar[n].site[x].tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
            
          endelse

          radar[n].site[x].geolat=tmp[3]
          radar[n].site[x].geolon=tmp[4]
          radar[n].site[x].alt=tmp[5]
          radar[n].site[x].boresite=tmp[6]
          radar[n].site[x].bmsep=tmp[7]
          radar[n].site[x].vdir=tmp[8]
          radar[n].site[x].atten=tmp[9]
          radar[n].site[x].tdiff=tmp[10]
          radar[n].site[x].phidiff=tmp[11]
          radar[n].site[x].interfer[0]=tmp[12]
          radar[n].site[x].interfer[1]=tmp[13] 
          radar[n].site[x].interfer[2]=tmp[14]
          radar[n].site[x].recrise=tmp[15]
          radar[n].site[x].maxatten=tmp[16]
          radar[n].site[x].maxrange=tmp[17]
          radar[n].site[x].maxbeam=tmp[18]
          x=x+1
        endif 
      endwhile    
      radar[n].snum=x
      trap:
        free_lun,unit
   endfor


iofail:
   return, 0
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarEpochGetSite
;
; PURPOSE:
;       Get the hardware information for a radar 
;       
;
; CALLING SEQUENCE:
;       site = RadarEpochGetSite(radar,tval)
;
;       This function finds the hardware information for a radar.
;
;       The returned structure is the hardware table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarEpochGetSite,radar,tval

  for s=0,radar.snum do begin
    if  (radar.site[s].tval eq -1) then break
    if  (radar.site[s].tval ge tval) then break
  endfor

  if (s eq radar.snum) then return, 0

  return, radar.site[s]



end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarYMDHMSGetSite
;
; PURPOSE:
;       Get the hardware information for a radar 
;       
;
; CALLING SEQUENCE:
;       site = RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc)
;
;       This function finds the hardware information for a radar.
;
;       The returned structure is the hardware table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarYMDHMSGetSite,radar,yr,mo,dy,hr,mt,sc

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc)
  return, RadarEpochGetSite(radar,tval)

end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGetRadar
;
; PURPOSE:
;       Get the hardware information for a radar 
;       
;
; CALLING SEQUENCE:
;       radar = RadarGetRadar(radar,id)
;
;       This function finds the information for a radar.
;
;       The returned structure is the information table or zero
;       if an error occurred.
;
;-----------------------------------------------------------------
;


function RadarGetRadar,radar,id
  q=where(radar[*].id eq id,count) 
  if (count ne 0) then return, radar[q[0]]
  return, 0
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarSlantRange
;
; PURPOSE:
;       Calculate the slant range
;       
;
; CALLING SEQUENCE:
;       srng = RadarSlantRange(frang,rsep,rxrise,range_edge,range_gate)
;
;       The returned value is the slant range
;
;-----------------------------------------------------------------
;



function RadarSlantRange,frang,rsep,rxrise,range_edge,range_gate
   lagfr=frang*20/3
   smsep=rsep*20/3
   return, (lagfr-rxrise+(range_gate-1)*smsep+range_edge)*0.15D
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGeoTGC
;
; PURPOSE:
;       Internal function for RPos
;       
;
; CALLING SEQUENCE:
;       RadarGeoTGC,iopt,gdlat,gdlon,grho,glat,glon,del
;
;-----------------------------------------------------------------
;


pro RadarGeoTGC,iopt,gdlat,gdlon,grho,glat,glon,del

   a=6378.16D
   f=1.0D/298.25D
  

   b=a*(1.0-f)
   e2=(a*a)/(b*b)-1
   if (iopt gt 0) then begin
     glat=atan( (b*b)/(a*a)*tan(!PI*gdlat/180.0))*180.0/!PI
     glon=gdlon
     if (glon gt 180) then glon=glon-360
   endif  else begin
     gdlat=atan( (a*a)/(b*b)*tan(!PI*glat/180.0))*180.0/!PI
     gdlon=glon
   endelse
   grho=a/sqrt(1.0+e2*sin(!PI*glat/180.0)*sin(!PI*glat/180.0));
   del=gdlat-glat;
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGeoFldPnt
;
; PURPOSE:
;       Internal function for RPos
;       
;
; CALLING SEQUENCE:
;       RadarGeoTGC,rrho,rlat,rlon,ral,rel,r,frho,flat,flon
;
;-----------------------------------------------------------------
;


pro RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon 

   sinteta=sin(!PI*(90.0-rlat)/180.0)
   rx=rrho*sinteta*cos(!PI*rlon/180.0)
   ry=rrho*sinteta*sin(!PI*rlon/180.0)
   rz=rrho*cos(!PI*(90.0-rlat)/180.0)

   sx=-r*cos(!PI*rel/180.0)*cos(!PI*ral/180.0)
   sy=r*cos(!PI*rel/180.0)*sin(!PI*ral/180.0)
   sz=r*sin(!PI*rel/180.0)

   tx  =  cos(!PI*(90.0-rlat)/180.0)*sx + sin(!PI*(90.0-rlat)/180.0)*sz
   ty  =  sy
   tz  = -sin(!PI*(90.0-rlat)/180.0)*sx + cos(!PI*(90.0-rlat)/180.0)*sz
   sx  =  cos(!PI*rlon/180.0)*tx - sin(!PI*rlon/180.0)*ty
   sy  =  sin(!PI*rlon/180.0)*tx + cos(!PI*rlon/180.0)*ty
   sz  =  tz

   tx=rx+sx
   ty=ry+sy
   tz=rz+sz

   frho=sqrt((tx*tx)+(ty*ty)+(tz*tz))
   flat=90.0-acos(tz/(frho))*180.0/!PI
   if ((tx eq 0) and (ty eq 0)) then flon=0 $
   else flon=atan(ty,tx)*180.0/!PI
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGeoCnvrt
;
; PURPOSE:
;       Internal function for RPos
;       
;
; CALLING SEQUENCE:
;       RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,rel
;
;-----------------------------------------------------------------
;


pro RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,rel

  rrad=0.0D
  rlat=0.0D
  rlon=0.0D
  del=0.0D
  

  kxg=cos(!PI*xel/180.0)*sin(!PI*xal/180.0)
  kyg=cos(!PI*xel/180.0)*cos(!PI*xal/180.0)
  kzg=sin(!PI*xel/180.0)
 
  RadarGeoTGC,1,gdlat,gdlon,rrad,rlat,rlon,del

  kxr=kxg
  kyr=kyg*cos(!PI*del/180.0)+kzg*sin(!PI*del/180.0)
  kzr=-kyg*sin(!PI*del/180.0)+kzg*cos(!PI*del/180.0)

  ral=atan(kxr,kyr)*180.0/!PI
  rel=atan(kzr/sqrt((kxr*kxr)+(kyr*kyr)))*180.0/!PI
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarFldPnth
;
; PURPOSE:
;       Internal function for RPos
;       
;
; CALLING SEQUENCE:
;       RadarFldPnth,gdlat,gdlon,psi,bor,fh,r,frho,flat,flon
;
;-----------------------------------------------------------------
;

pro RadarFldPnth,gdlat,gdlon,psi,bore,fh,r,frho,flat,flon
 
  rrad=0.0D
  rlat=0.0D
  rlon=0.0D
  del=0.0D 
  dum1=0.0D
  dum2=0.0D
  dum3=0.0D

  if (fh le 150) then xh=fh $
  else begin
    if (r le 600) then xh=115 $
    else if ((r gt 600) and (r lt 800)) then $
      xh=(r-600.0D)/200.0D*(fh-115.0D)+115.0D $
    else xh=fh
  endelse

  if (r lt 150) then xh=(r/150.0D)*115.0D;
  RadarGeoTGC,1,gdlat,gdlon,rrad,rlat,rlon,del

  rrho=rrad
  frad=rrad
 
  repeat begin 
    frho=frad+xh
 
    rel=asin(((frho*frho)-(rrad*rrad)-(r*r))/(2*rrad*r))*180.0/!PI
    xel=rel

    if (((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $
         (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))) lt 0) then tan_azi=1e32 $
      else tan_azi=sqrt( (sin(!PI*psi/180.0)*sin(!PI*psi/180.0))/ $
                ((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $ 
                (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))))
    if (psi gt 0) then azi=atan(tan_azi)*180.0/!PI $
    else azi=-atan(tan_azi)*180.0/!PI

    xal=azi+bore
  
    RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,dum
    
    RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon
    RadarGeoTGC,-1,dum1,dum2,frad,flat,flon,dum3
  
    fhx=frho-frad    
  endrep until (abs(fhx-xh) le 0.5) 
end
 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarFldPnthGS
;
; PURPOSE:
;       Internal function for RPos
;       
;
; CALLING SEQUENCE:
;       RadarFldPnthGS,gdlat,gdlon,psi,bor,fh,r,frho,flat,flon
;
;-----------------------------------------------------------------
;


pro RadarFldPnthGS,gdlat,gdlon,psi,bore,fh,r,frho,flat,flon
 
  rrad=0.0D
  rlat=0.0D
  rlon=0.0D
  del=0.0D 
  dum1=0.0D
  dum2=0.0D
  dum3=0.0D

  if (fh le 150) then xh=fh $
  else begin
    if (r le 300) then xh=115 $
    else if ((r gt 300) and (r lt 500)) then $
      xh=(r-300.0D)/200.0D*(fh-115.0D)+115.0D $
    else xh=fh
  endelse

  if (r lt 150) then xh=(r/150.0D)*115.0D;
  RadarGeoTGC,1,gdlat,gdlon,rrad,rlat,rlon,del

  rrho=rrad
  frad=rrad
 

  repeat begin 
    frho=frad+xh

    rel=asin(((frho*frho)-(rrad*rrad)-(r*r))/(2*rrad*r))*180.0/!PI
  
    xel=rel

    if (((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $
         (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))) lt 0) then tan_azi=1e32 $
      else tan_azi=sqrt( (sin(!PI*psi/180.0)*sin(!PI*psi/180.0))/ $
                ((cos(!PI*psi/180.0)*cos(!PI*psi/180.0))- $ 
                (sin(!PI*xel/180.0)*sin(!PI*xel/180.0))))
    if (psi gt 0) then azi=atan(tan_azi)*1.0*180/!PI $
    else azi=-atan(tan_azi)*180/!PI

    xal=azi+bore

    RadarGeoCnvrt,gdlat,gdlon,xal,xel,ral,dum
    RadarFldPnt,rrho,rlat,rlon,ral,rel,r,frho,flat,flon
    RadarGeoTGC,-1,dum1,dum2,frad,flat,flon,dum3
    fhx=frho-frad 
   
  endrep until (abs(fhx-xh) le 0.5) 
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarNativPos
;
; PURPOSE:
;       Convert a range/beam coordinatee to geographic position
;       
;
; CALLING SEQUENCE:
;       s=RadarNativePos(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion. The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;

 
function RadarNativePos,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                        height,rho,lat,lng
                
  re=6356.779D

  bm_edge=0.0D;
  range_edge=0.0D
  
  if (center eq 0) then begin
    bm_edge=-site.bmsep*0.5;
    range_edge=-0.5*rsep*20.0/3.0
  endif
  
  if (rxrise eq 0) then rx=site.recrise $
  else rx=rxrise
  offset=site.maxbeam/2.0-0.5
  psi=site.bmsep*(bcrd-offset)+bm_edge
  d=RadarSlantRange(frang,rsep,rx,range_edge,rcrd+1)

  if (height lt 90) then $
    height=-re+sqrt((re*re)+2*d*re*sin(!PI*height/180.0)+(d*d));
  
  RadarFldPnth,site.geolat,site.geolon,psi,site.boresite,height,d,rho,lat,lng 
  return, 0
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarNativPosGS
;
; PURPOSE:
;       Convert a range/beam coordinates to geographic position for
;       ground scatter.
;       
;
; CALLING SEQUENCE:
;       RadarNativePosGS(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion for ground scatter
;       The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;


function RadarNativePosGS,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                        height,rho,lat,lng
                
  re=6356.779D

  bm_edge=0.0D;
  range_edge=0.0D
  
  if (center eq 0) then begin
    bm_edge=-site.bmsep*0.5;
    range_edge=-0.5*rsep*20.0/3.0
  endif
  
  if (rxrise eq 0) then rx=site.recrise $
  else rx=rxrise
  offset=site.maxbeam/2.0-0.5
  psi=site.bmsep*(bcrd-offset)+bm_edge

  d=RadarSlantRange(frang,rsep,rx,range_edge,rcrd+1)/2.0D

  if (height lt 90) then $
    height=-re+sqrt((re*re)+2*d*re*sin(!PI*height/180.0)+(d*d))
  
  RadarFldPnthGS,site.geolat,site.geolon,psi,site.boresite, $
                 height,d,rho,lat,lng 
  return, 0
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarExternalPos
;
; PURPOSE:
;       Convert a range/beam coordinates to geographic position
;       
;
; CALLING SEQUENCE:
;       s=RadarExternalPos(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion. The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;

 
function RadarExternalPos,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                        height,rho,lat,lng,lib=lib
                
  rho=0.0D
  lat=0.0D
  lng=0.0D
  centerl=long(center)
  bcrdl=long(bcrd)
  rcrdl=long(rcrd)
  frangl=long(frang)
  rsepl=long(rsep)
  rxrisel=long(rxrise)
  heightd=double(height)
  rxrise=long(rxrise)

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
  s=CALL_EXTERNAL(lib,'RPosIDL', $
                  centerl,bcrdl,rcrdl,site,frangl,rsepl, $
                  rxrisel,heightd,rho,lat,lng)
  return, 0
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarExternalPosGS
;
; PURPOSE:
;       Convert a range/beam coordinates to geographic position
;       
;
; CALLING SEQUENCE:
;       s=RadarExternalPosGS(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng,lib=library)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion. The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;

 
function RadarExternalPosGS,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                        height,rho,lat,lng,lib=lib
                
  rho=0.0D
  lat=0.0D
  lng=0.0D
  centerl=long(center)
  bcrdl=long(bcrd)
  rcrdl=long(rcrd)
  frangl=long(frang)
  rsepl=long(rsep)
  rxrisel=long(rxrise)
  heightd=double(height)
  rxrise=long(rxrise)

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
  s=CALL_EXTERNAL(lib,'RPosGSIDL', $
                  centerl,bcrdl,rcrdl,site,frangl,rsepl, $
                  rxrisel,heightd,rho,lat,lng)
  return, 0
end



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarExternalGeochrt
;
; PURPOSE:
;       For all range/beam coordinates calculate
;	geographic position [lat, lng, rho] and
;	pointing elevation and azimuth angles [elv, azc]
;       
;
; CALLING SEQUENCE:
;       s=RadarExternalGeochrt( site, year, frang,rsep,rxrise,
;                      height, rho, lat, lng, elv, azc)
;
;
;       The calculated values are returned in rho, lat, lng, azc and elv
;       The returned value is zero.
;-----------------------------------------------------------------
;

 
function RadarExternalGeochrt, site, yr, frang, rsep, rxrise, height, $
				rho, lat, lng, azc, elv, lib=lib
                
  yrl=long(yr)
  frangl=long(frang)
  rsepl=long(rsep)
  rxrisel=long(rxrise)
  heightf=float(height)
  rxrise=long(rxrise)
  rho= FLTARR( site.maxbeam, site.maxrange)
  lat= FLTARR( site.maxbeam, site.maxrange)
  lng= FLTARR( site.maxbeam, site.maxrange)
  elv= FLTARR( site.maxbeam, site.maxrange)
  azc= FLTARR( site.maxbeam, site.maxrange)

  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
  s=CALL_EXTERNAL( lib, 'RPosGeochrtIDL', site, yrl, frangl, rsepl, rxrisel, heightf, rho, lat, lng, elv, azc)
  return, 0
end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarPos
;
; PURPOSE:
;       Convert a range/beam coordinates to geographic position
;       
;
; CALLING SEQUENCE:
;       s=RadarPos(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng,lib=library,/native,/external)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion. The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;



function RadarPos,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                  height,rho,lat,lng,lib=lib,native=native,external=external
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1



  if (mode eq 1) then return, RadarNativePos(center,bcrd,rcrd,site,frang, $
                                            rsep,rxrise,height,rho,lat,lng) $
  else return, RadarExternalPos(center,bcrd,rcrd,site,frang, $
                                rsep,rxrise,height,rho,lat,lng,lib=lib) 

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarPosGS
;
; PURPOSE:
;       Convert a range/beam coordinates to geographic position
;       
;
; CALLING SEQUENCE:
;       s=RadarPos(center,bcrd,rcrd,site,frang,rsep,rxrise,
;                      height,rho,lat,lng,lib=library,/native,/external)
;
;       This function converts a range/beam coordinate to 
;       geographic positiion. The range (starting at zero) is 
;       given by rcrd, the beam by bcrd. The radar hardware
;       table is given by the structure site. The position
;       of the first range gate in kilometers is given by
;       frang and the range seperation in kilometers is given
;       by rsep. The receiver rise time is given by rxrise, if
;       this value is zero then the rise time is taken from
;       the parameter block. The height of the transformation is
;       given by height, if this value is less than 90 then it
;       is asuumed to be the elevation angle from the radar.
;       If center is not equal to zero, then the calculation is
;       assumed to be for the center of the cell, not the edge.       
;
;       The calculated values are returned in rho, lat and lng
;       The returned value is zero.
;-----------------------------------------------------------------
;



function RadarPosGS,center,bcrd,rcrd,site,frang,rsep,rxrise,$
                  height,rho,lat,lng,lib=lib,native=native,external=external
 
  mode=0
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
  if KEYWORD_SET(native) then mode=1
  if KEYWORD_SET(external) then mode=2
  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
  else if (mode eq 0) then mode=1



  if (mode eq 1) then return, RadarNativePosGS(center,bcrd,rcrd,site,frang, $
                                            rsep,rxrise,height,rho,lat,lng) $
  else return, RadarExternalPosGS(center,bcrd,rcrd,site,frang, $
                                rsep,rxrise,height,rho,lat,lng,lib=lib) 

end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;+
; NAME:
;       RadarGeochrt
;
; PURPOSE:
;       For all range/beam coordinates calculate
;	geographic position [lat, lng, rho] and
;	pointing elevation and azimuth angles [elv, azc]
;       
;
; CALLING SEQUENCE:
;       s=RadarGeochrt( site, year, frang,rsep,rxrise,
;                      height, rho, lat, lng, elv, azc)
;
;
;       The calculated values are returned in rho, lat, lng, azc and elv
;       The returned value is zero.
;-----------------------------------------------------------------
;



function RadarGeochrt, site, yr, frang, rsep, rxrise, height, $
			rho, lat, lng, azc, elv, $
                  lib=lib,native=native,external=external
 
;  mode=0
;  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
;  if KEYWORD_SET(native) then mode=1
;  if KEYWORD_SET(external) then mode=2
;  if (mode eq 0) and (file_test(lib) eq 1) then mode=2 $
;  else if (mode eq 0) then mode=1
  mode=2
  if (KEYWORD_SET(lib) eq 0) then lib=getenv('LIB_RPOSIDL')
  if KEYWORD_SET(native) then begin
    PRINT, "RadarNativeGeochrt NOT implememented."
    RETURN, 1
  endif
 return, RadarExternalGeochrt( site, yr, frang, rsep, rxrise, height, rho, lat, lng, elv, azc, lib=lib)
end
