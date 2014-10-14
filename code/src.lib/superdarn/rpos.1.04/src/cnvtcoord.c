/* cnvtcoord.c
   ===========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Software Toolkit - SuperDARN Toolkit" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "rmath.h"
#include "radar.h"
#include "rpos.h"
#include "aacgm.h"

/*
 $Log: cnvtcoord.c,v $
 Revision 1.3  2006/03/14 20:07:46  barnes
 Added the ability to vary the number of beams.

 Revision 1.2  2004/05/04 22:47:16  barnes
 Changed math header name.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

 Revision 1.2  2003/03/09 17:14:47  barnes
 Renamed AACGM header.

 Revision 1.1  2003/03/07 19:08:30  barnes
 Initial revision

 */


double slant_range(int frang,int rsep,
		   double rxris,double range_edge,
		   int range_gate) {
   int lagfr,smsep;
   lagfr=frang*20/3;
   smsep=rsep*20/3;
   return (lagfr-rxris+(range_gate-1)*smsep+range_edge)*0.15;
}


void geodtgc(int iopt,double *gdlat,double *gdlon,
             double *grho,double *glat,
			 double *glon,double *del) {

   double a=6378.16;
   double f=1.0/298.25;
   double b,e2;

   b=a*(1.0-f);
   e2=(a*a)/(b*b)-1;
   if (iopt>0) {
     *glat=atand( (b*b)/(a*a)*tand(*gdlat));
     *glon=*gdlon;
     if (*glon > 180) *glon=*glon-360;
   } else {
     *gdlat=atand( (a*a)/(b*b)*tand(*glat));
     *gdlon=*glon;
   }
   *grho=a/sqrt(1.0+e2*sind(*glat)*sind(*glat));
   *del=*gdlat-*glat;
}

void fldpnt(double rrho,double rlat,double rlon,double ral,
			double rel,double r,double *frho,double *flat,
                        double *flon) {

   double rx,ry,rz,sx,sy,sz,tx,ty,tz;
   double sinteta;
  
   /* convert from global spherical to global cartesian*/

   sinteta=sind(90.0-rlat);
   rx=rrho*sinteta*cosd(rlon);
   ry=rrho*sinteta*sind(rlon);
   rz=rrho*cosd(90.0-rlat);

   sx=-r*cosd(rel)*cosd(ral);
   sy=r*cosd(rel)*sind(ral);
   sz=r*sind(rel);

   tx  =  cosd(90.0-rlat)*sx + sind(90.0-rlat)*sz;
   ty  =  sy;
   tz  = -sind(90.0-rlat)*sx + cosd(90.0-rlat)*sz;
   sx  =  cosd(rlon)*tx - sind(rlon)*ty;
   sy  =  sind(rlon)*tx + cosd(rlon)*ty;
   sz  =  tz;

   tx=rx+sx;
   ty=ry+sy;
   tz=rz+sz;

   /* convert from cartesian back to global spherical*/
   *frho=sqrt((tx*tx)+(ty*ty)+(tz*tz));
   *flat=90.0-acosd(tz/(*frho));
   if ((tx==0) && (ty==0)) *flon=0;
   else *flon=atan2d(ty,tx);
}

void geocnvrt(double gdlat,double gdlon,
			  double xal,double xel,double *ral,double *rel) {

  double kxg,kyg,kzg,kxr,kyr,kzr;
  double rrad,rlat,rlon,del;

  kxg=cosd(xel)*sind(xal);
  kyg=cosd(xel)*cosd(xal);
  kzg=sind(xel);
  geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
  kxr=kxg;
  kyr=kyg*cosd(del)+kzg*sind(del);
  kzr=-kyg*sind(del)+kzg*cosd(del);

  *ral=atan2d(kxr,kyr);
  *rel=atand(kzr/sqrt((kxr*kxr)+(kyr*kyr)));
}

void fldpnth(double gdlat,double gdlon,double psi,double bore,
			 double fh,double r,double *frho,double *flat,
	                 double *flon) {

  double rrad,rlat,rlon,del;
  double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
  double dum,dum1,dum2,dum3;
  double frad;  
 
  if (fh<=150) xh=fh;
  else {
    if (r<=600) xh=115;
    else if ((r>600) && (r<800)) xh=(r-600)/200*(fh-115)+115;
    else xh=fh;
  }

  if (r<150) xh=(r/150.0)*115.0;
  geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
  rrho=rrad;
  frad=rrad;
 

  do {
    *frho=frad+xh;
  
    rel=asind( ((*frho**frho) - (rrad*rrad) - (r*r)) / (2*rrad*r));
    xel=rel;
    if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
      else tan_azi=sqrt( (sind(psi)*sind(psi))/
                ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
    if (psi>0) azi=atand(tan_azi)*1.0;
      else azi=atand(tan_azi)*-1.0;
    xal=azi+bore;
    geocnvrt(gdlat,gdlon,xal,xel,&ral,&dum);

    fldpnt(rrho,rlat,rlon,ral,rel,r,frho,flat,flon);
    geodtgc(-1,&dum1,&dum2,&frad,flat,flon,&dum3);
    fhx=*frho-frad; 
  } while(fabs(fhx-xh) > 0.5);
} 


void fldpnth_gs(double gdlat,double gdlon,double psi,double bore,
			 double fh,double r,double *frho,double *flat,
	                 double *flon) {

  double rrad,rlat,rlon,del;
  double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
  double dum,dum1,dum2,dum3;
  double frad;  
 
  if (fh<=150) xh=fh;
  else {
    if (r<=300) xh=115;
    else if ((r>300) && (r<500)) xh=(r-300)/200*(fh-115)+115;
    else xh=fh;
  }

  if (r<150) xh=(r/150.0)*115.0;
  geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
  rrho=rrad;
  frad=rrad;
 

  do {
    *frho=frad+xh;
  
    rel=asind( ((*frho**frho) - (rrad*rrad) - (r*r)) / (2*rrad*r));
    xel=rel;
    if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
      else tan_azi=sqrt( (sind(psi)*sind(psi))/
                ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
    if (psi>0) azi=atand(tan_azi)*1.0;
      else azi=atand(tan_azi)*-1.0;
    xal=azi+bore;
    geocnvrt(gdlat,gdlon,xal,xel,&ral,&dum);

    fldpnt(rrho,rlat,rlon,ral,rel,r,frho,flat,flon);
    geodtgc(-1,&dum1,&dum2,&frad,flat,flon,&dum3);
    fhx=*frho-frad; 
  } while(fabs(fhx-xh) > 0.5);
} 


void RPosGeo(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int frang,int rsep,
                int rxrise,double height,
                double *rho,double *lat,double *lng) {

  double rx;
  double psi,d;
  double re=6356.779;
  double offset;
  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->bmsep*0.5;
    range_edge=-0.5*rsep*20/3;
  }
  
  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;
  offset=pos->maxbeam/2.0-0.5;
  psi=pos->bmsep*(bcrd-offset)+bm_edge;
  d=slant_range(frang,rsep,rx,range_edge,rcrd+1);
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth(pos->geolat,pos->geolon,psi,pos->boresite,
		  height,d,rho,lat,lng); 
}


void RPosMag(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int frang,int rsep,int rxrise,double height,
                double *rho,double *lat,double *lng) {

  double rx;
  double radius;
  double psi,d;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;
  double offset=0;
 
  if (center==0) {
    bm_edge=-pos->bmsep*0.5;
    range_edge=-0.5*rsep*20/3;
  }
  
  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;

  offset=pos->maxbeam/2.0-0.5;
  psi=pos->bmsep*(bcrd-offset)+bm_edge;
  d=slant_range(frang,rsep,rx,range_edge,rcrd+1);
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth(pos->geolat,pos->geolon,psi,pos->boresite,
		  height,d,rho,lat,lng); 
 
  AACGMConvert(*lat,*lng,(double) height,lat,lng,&radius,0);
 
 

}


void RPosCubic(int center,int bcrd,int rcrd,
		   struct RadarSite *pos,
		   int frang,int rsep,int rxrise,double height,
           double *x,double *y,double *z) {
   
    /* returns cartesian cubic co-ordinates */
  double rx;
  double psi,d;
  double rho,lat,lng;
  double re=6356.779;
  double offset=0;
  double bm_edge=0;
  double range_edge=0;

  if (center==0) {
    bm_edge=-pos->bmsep*0.5;
    range_edge=-0.5*rsep*20/3;
  }

  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;

  offset=pos->maxbeam/2.0-0.5;
  psi=pos->bmsep*(bcrd-offset)+bm_edge;
  
  d=slant_range(frang,rsep,rx,range_edge,rcrd+1);
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
  fldpnth(pos->geolat,pos->geolon,psi,pos->boresite,
	      height,d,&rho,&lat,&lng);  

  /* convert to x,y,z (normalized to the unit sphere) */

  lng=90-lng;
  *x=rho*cos(lng*PI/180.0)*cos(lat*PI/180.0)/re;
  *y=rho*sin(lat*PI/180.0)/re;
  *z=rho*sin(lng*PI/180.0)*cos(lat*PI/180.0)/re;

} 



void RPosGeoGS(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int frang,int rsep,int rxrise,double height,
                double *rho,double *lat,double *lng) {
 
  double rx;
  double psi,d;
  double re=6356.779;
  double offset=0;
  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->bmsep*0.5;
    range_edge=-0.5*rsep*20/3;
  }
  
  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;

  offset=pos->maxbeam/2.0-0.5;
  psi=pos->bmsep*(bcrd-offset)+bm_edge;
 
  d=slant_range(frang,rsep,rx,range_edge,rcrd+1)/2;
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth_gs(pos->geolat,pos->geolon,psi,pos->boresite,
		  height,d,rho,lat,lng); 
}



void RPosMagGS(int center,int bcrd,int rcrd,
                struct RadarSite *pos,
                int frang,int rsep,int rxrise,double height,
                double *rho,double *lat,double *lng) {
  double rx;
  double radius;
  double psi,d;
  double re=6356.779;
  double offset=0;
  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->bmsep*0.5;
    range_edge=-0.5*rsep*20/3;
  }
  
  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;

  offset=pos->maxbeam/2.0-0.5;
  psi=pos->bmsep*(bcrd-offset)+bm_edge;

  
  d=slant_range(frang,rsep,rx,range_edge,rcrd+1)/2;
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth_gs(pos->geolat,pos->geolon,psi,pos->boresite,
		  height,d,rho,lat,lng); 
 
  AACGMConvert(*lat,*lng,(double) height,lat,lng,&radius,0);
 
 

}


void RPosCubicGS(int center,int bcrd,int rcrd,
		   struct RadarSite *pos,
		   int frang,int rsep,int rxrise,double height,
           double *x,double *y,double *z) {
   
    /* returns cartesian cubic co-ordinates */
  double rx;
  double psi,d; 
  double rho,lat,lng;
  double re=6356.779;
  double offset=0;
  double bm_edge=0;
  double range_edge=0;

  if (center==0) {
    bm_edge=-pos->bmsep*0.5;
    range_edge=-0.5*rsep*20/3;
  }

  if (rxrise==0) rx=pos->recrise;
  else rx=rxrise;

  offset=pos->maxbeam/2.0-0.5;
  psi=pos->bmsep*(bcrd-offset)+bm_edge;

  d=slant_range(frang,rsep,rx,range_edge,rcrd+1)/2;
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
  fldpnth_gs(pos->geolat,pos->geolon,psi,pos->boresite,
	      height,d,&rho,&lat,&lng);  

  /* convert to x,y,z (normalized to the unit sphere) */

  lng=90-lng;
  *x=rho*cos(lng*PI/180.0)*cos(lat*PI/180.0)/re;
  *y=rho*sin(lat*PI/180.0)/re;
  *z=rho*sin(lng*PI/180.0)*cos(lat*PI/180.0)/re;

} 

 



