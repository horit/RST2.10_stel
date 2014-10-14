/* sgp4.c
   ======
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "rtime.h"
#include "sgp4.h"

/*
 $Log: sgp4.c,v $
 Revision 1.2  2004/04/26 21:25:27  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/04/26 18:22:12  barnes
 Initial revision

*/


#define  DE2RA 1.74532925E-2
#define  PI 3.1415926535898
#define  PIO2 1.5707963267949
#define  X3PIO2 4.71238898
#define  TWOPI 6.2831853071796
#define  E6A 1.0E-6
#define  TOTHRD 6.6666667E-1
#define  XJ2 1.0826158E-3
#define  XJ3 -2.53881E-6
#define  XJ4 1.65597E-6
#define  XKE 7.43669161E-2
#define  XKMPER 6.378135E3
#define  XMNPDA 1.44E3
#define  AE 1.0
#define  CK2 5.413079E-4
#define  CK4 6.209887E-7
#define  F 3.352779E-3
#define  GE 3.986008E5 
#define  S 1.012229
#define  QOMS2T 1.880279E-09
#define  SECDAY 8.6400E4
#define  OMEGA_E 1.0027379
#define  OMEGA_ER 6.3003879
#define  ZNS 1.19459E-5
#define  C1SS 2.9864797E-6
#define  ZES 1.675E-2
#define  ZNL 1.5835218E-4
#define  C1L 4.7968065E-7
#define  ZEL 5.490E-2
#define  ZCOSIS 9.1744867E-1
#define  ZSINIS 3.9785416E-1
#define  ZSINGS -9.8088458E-1
#define  ZCOSGS 1.945905E-1
#define  ZCOSHS 1.0
#define  ZSINHS 0.0
#define  Q22 1.7891679E-6
#define  Q31 2.1460748E-6
#define  Q33 2.2123015E-7
#define  G22 5.7686396
#define  G32 9.5240898E-1
#define  G44 1.8014998
#define  G52 1.0508330
#define  G54 4.4108898
#define  ROOT22 1.7891679E-6
#define  ROOT32 3.7393792E-7
#define  ROOT44 7.3636953E-9
#define  ROOT52 1.1428639E-7
#define  ROOT54 2.1765803E-9
#define  THDT 4.3752691E-3
#define  RHO 1.5696615E-1
#define  MFACTOR 7.292115E-5
#define  SR 6.96000E5
#define  AU 1.49597870E8  

double SGP4TLEToJulian(struct SGP4TLE *TLE) {
 

  int yr;
  double jepoch;
  double year,day;

  yr=(int) (TLE->epoch*1e-3);
  year=yr;
  day=((TLE->epoch*1e-3)-year)*1e3;

  if (yr<57) yr=yr+2000;
  else yr=yr+1900;

  jepoch=TimeYMDHMSToJulian(yr,1,1,0,0,0)+day-1.0;
  return jepoch;
}



int SGP4DecodeTLE(char *txt[2],struct SGP4TLE *TLE) {
  int c;
  char tmp[40];
  double temp;

  strncpy(tmp,txt[0]+2,5);
  tmp[5]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->catnr=atoi(tmp+c);  

  strncpy(TLE->idesg,txt[0]+9,8);
  TLE->idesg[8]=0;


  strncpy(tmp,txt[0]+18,14);
  tmp[14]=0;

  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->epoch=atof(tmp+c);  

  strncpy(tmp,txt[0]+33,10);
  tmp[10]=0;

  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->xndt2o=atof(tmp+c);  

  strncpy(tmp,txt[0]+44,1);
  tmp[1]='.';
  strncpy(tmp+2,txt[0]+45,5);
  tmp[7]='E';
  strncpy(tmp+8,txt[0]+50,2);
  tmp[10]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->xndd6o=atof(tmp+c);  


  strncpy(tmp,txt[0]+53,1);
  tmp[1]='.';
  strncpy(tmp+2,txt[0]+54,5);
  tmp[7]='E';
  strncpy(tmp+8,txt[0]+59,2);
  tmp[10]=0;
 
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->bstar=atof(tmp+c);  

  strncpy(tmp,txt[0]+64,4);
  tmp[4]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->elset=atoi(tmp+c);  

  strncpy(tmp,txt[1]+8,8);
  tmp[8]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->xincl=atof(tmp+c);  

  strncpy(tmp,txt[1]+17,8);
  tmp[8]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->xnodeo=atof(tmp+c);  

  strcpy(tmp,"0.");
  strncat(tmp,txt[1]+26,7);
  tmp[9]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->eo=atof(tmp+c);  

  strncpy(tmp,txt[1]+34,8);
  tmp[8]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->omegao=atof(tmp+c);  

  strncpy(tmp,txt[1]+43,8);
  tmp[8]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->xmo=atof(tmp+c);  

  strncpy(tmp,txt[1]+52,9);
  tmp[9]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->xno=atof(tmp+c);  

  strncpy(tmp,txt[1]+63,5);
  tmp[5]=0;
  for (c=0;(isspace(tmp[c])) && (tmp[c] !=0);c++); 
  TLE->revnum=atoi(tmp+c);  

  TLE->xnodeo=TLE->xnodeo*DE2RA;
  TLE->omegao=TLE->omegao*DE2RA;
  TLE->xmo=TLE->xmo*DE2RA;
  TLE->xincl=TLE->xincl*DE2RA;
 
  temp=TWOPI/XMNPDA/XMNPDA;
  TLE->xno=TLE->xno*temp*XMNPDA;
  TLE->xndt2o=TLE->xndt2o*temp;
  TLE->xndd6o=TLE->xndd6o*temp/XMNPDA;

  TLE->bstar=TLE->bstar/AE;

  return 0;
}



double actan(double sinx, double cosx) {
  if(cosx == 0) {
    if(sinx > 0) return PIO2;
    else return X3PIO2;
  } else {
    if(cosx > 0) {
      if(sinx > 0) return (atan(sinx/cosx) );
      else return ( TWOPI + atan(sinx/cosx) );
    } else return ( PI + atan(sinx/cosx) );
  }  
}

double fmod2p(double x) {
  int i;
  double ret_val;

  ret_val = x;
  i = (int) (ret_val/TWOPI);
  ret_val -= i*TWOPI;
  if (ret_val < 0) ret_val += TWOPI;
  return ret_val;
}


double thetag_jd(double jd) {
  double UT,TU,GMST;
  int i;

  UT=(jd+0.5)-floor(jd+0.5);
  jd=jd-UT;
  TU=(jd-2451545.0)/36525;
  GMST=24110.54841+TU*(8640184.812866+TU*(0.093104-TU*6.2E-6));
  GMST=GMST+SECDAY*OMEGA_E*UT;
  i=(int) (GMST/SECDAY);
  GMST=GMST-i*SECDAY;
  if (GMST<0) GMST+=SECDAY;
  return (TWOPI*GMST/SECDAY);
}

int SGP4Setup(struct SGP4 *SGP4,struct SGP4TLE *TLE) {

  /* Recover original mean motion (xnodp) and   */
  /* semimajor axis (aodp) from input elements. */

  double a1,eosq,del1,betao,ao,delo,s4,qoms24,perige,pinvsq,tsi;
  double theta2,betao2,etasq,eeta,psisq,coef,coef1,c2,c3;
  double temp,temp1,temp2,temp3,x1m5th,xhdot1; 
  double a3ovk2,theta4,c1sq;

     
  a1 =pow(XKE/TLE->xno,TOTHRD);
  SGP4->cosio=cos(TLE->xincl);
  theta2 = SGP4->cosio*SGP4->cosio;
  SGP4->x3thm1 = 3*theta2-1.0;
  eosq = TLE->eo*TLE->eo;
  betao2 = 1-eosq;
  betao = sqrt(betao2);
  del1 = 1.5*CK2*SGP4->x3thm1/(a1*a1*betao*betao2);
  ao = a1*(1-del1*(0.5*TOTHRD+del1*(1+134/81*del1)));
  delo = 1.5*CK2*SGP4->x3thm1/(ao*ao*betao*betao2);
  SGP4->xnodp = TLE->xno/(1+delo);
  SGP4->aodp = ao/(1-delo);
 
  /* For perigee less than 220 kilometers, the "simple" flag is set */
  /* and the equations are truncated to linear variation in sqrt a  */
  /* and quadratic variation in mean anomaly.  Also, the c3 term,   */
  /* the delta omega term, and the delta m term are dropped.        */

  if((SGP4->aodp*(1-TLE->eo)/AE) < (220/XKMPER+AE)) SGP4->simple=1;
  else SGP4->simple=0;
    
  /* For perigee below 156 km, the       */ 
  /* values of s and qoms2t are altered. */

  s4 = S;
  qoms24 = QOMS2T;
  perige = (SGP4->aodp*(1-TLE->eo)-AE)*XKMPER;
  if (perige < 156) {
    if (perige <= 98) s4 = 20;
    else s4 = perige-78;
    qoms24 = pow((120-s4)*AE/XKMPER,4);
    s4 = s4/XKMPER+AE;
  }
  pinvsq = 1/(SGP4->aodp*SGP4->aodp*betao2*betao2);
  tsi = 1/(SGP4->aodp-s4);
  SGP4->eta = SGP4->aodp*TLE->eo*tsi;
  etasq = SGP4->eta*SGP4->eta;
  eeta = TLE->eo*SGP4->eta;
  psisq = fabs(1-etasq);
  coef = qoms24*pow(tsi,4);
  coef1 = coef/pow(psisq,3.5);
  c2 = coef1*SGP4->xnodp*(SGP4->aodp*(1+1.5*etasq+eeta*(4+etasq))+
           0.75*CK2*tsi/psisq*SGP4->x3thm1*(8+3*etasq*(8+etasq)));
  SGP4->c1 = TLE->bstar*c2;
  SGP4->sinio = sin(TLE->xincl);
  a3ovk2 = -XJ3/CK2*pow(AE,3);
  c3 = coef*tsi*a3ovk2*SGP4->xnodp*AE*SGP4->sinio/TLE->eo;
  SGP4->x1mth2 = 1-theta2;
  SGP4->c4 = 2*SGP4->xnodp*coef1*SGP4->aodp*betao2*
             (SGP4->eta*(2+0.5*etasq)+
              TLE->eo*(0.5+2*etasq)-2*CK2*tsi/(SGP4->aodp*psisq)*
             (-3*SGP4->x3thm1*(1-2*eeta+etasq*(1.5-0.5*eeta))+0.75*
                 SGP4->x1mth2*(2*etasq-eeta*(1+etasq))*cos(2*TLE->omegao)));
  SGP4->c5 = 2*coef1*SGP4->aodp*betao2*(1+2.75*(etasq+eeta)+eeta*etasq);
  theta4 = theta2*theta2;
  temp1 = 3*CK2*pinvsq*SGP4->xnodp;
  temp2 = temp1*CK2*pinvsq;
  temp3 = 1.25*CK4*pinvsq*pinvsq*SGP4->xnodp;
  SGP4->xmdot = SGP4->xnodp+0.5*temp1*betao*SGP4->x3thm1+
	           0.0625*temp2*betao*(13-78*theta2+137*theta4);
  x1m5th = 1-5*theta2;
  SGP4->omgdot = -0.5*temp1*x1m5th+0.0625*temp2*(7-114*theta2+
                 395*theta4)+temp3*(3-36*theta2+49*theta4);
  xhdot1 = -temp1*SGP4->cosio;
  SGP4->xnodot = xhdot1+(0.5*temp2*(4-19*theta2)+
                 2*temp3*(3-7*theta2))*SGP4->cosio;
  SGP4->omgcof = TLE->bstar*c3*cos(TLE->omegao);
  SGP4->xmcof = -TOTHRD*coef*TLE->bstar*AE/eeta;
  SGP4->xnodcf = 3.5*betao2*xhdot1*SGP4->c1;
  SGP4->t2cof = 1.5*SGP4->c1;
  SGP4->xlcof = 0.125*a3ovk2*SGP4->sinio*(3+5*SGP4->cosio)/(1+SGP4->cosio);
  SGP4->aycof = 0.25*a3ovk2*SGP4->sinio;
  SGP4->delmo = pow(1+SGP4->eta*cos(TLE->xmo),3);
  SGP4->sinmo = sin(TLE->xmo);
  SGP4->x7thm1 = 7*theta2-1;
  if (!SGP4->simple) {
     c1sq = SGP4->c1*SGP4->c1;
     SGP4->d2 = 4*SGP4->aodp*tsi*c1sq;
     temp = SGP4->d2*tsi*SGP4->c1/3;
     SGP4->d3 = (17*SGP4->aodp+s4)*temp;
     SGP4->d4 = 0.5*temp*SGP4->aodp*tsi*(221*SGP4->aodp+31*s4)*SGP4->c1;
     SGP4->t3cof = SGP4->d2+2*c1sq;
     SGP4->t4cof = 0.25*(3*SGP4->d3+SGP4->c1*(12*SGP4->d2+10*c1sq));
     SGP4->t5cof = 0.2*(3*SGP4->d4+12*SGP4->c1*SGP4->d3+6*SGP4->d2*
                   SGP4->d2+15*c1sq*(2*SGP4->d2+c1sq));
  }
  return 0;
}



int SGP4Calculate(double tsince,struct SGP4 *SGP4,
                  struct SGP4TLE *TLE,
		  struct SGP4XYZVector *pos,
                  struct SGP4XYZVector *vel) {

  int i;
  double xmdf,omgadf,xnoddf,omega,xmp,tsq,xnode,tempa,tempe,
         templ,delomg,delm,temp,tcube,tfour,a,e,xl,beta,xn,axn,
         xll,aynl,xlt,ayn,capu,temp2,sinepw,cosepw,temp1,temp3,temp4,temp5,
         temp6,epw,ecose,esine,elsq,pl,r,rdot,rfdot,betal,cosu,sinu,u,
         cos2u,xinck;
  double sin2u,rk,uk,xnodek,rdotk,rfdotk;
  double cosuk,sinuk,cosik,sinik,cosnok,sinnok;
  double xmx,xmy;
  double vx,vy,vz;
  double ux,uy,uz;

  /* Update for secular gravity and atmospheric drag. */

  xmdf = TLE->xmo+SGP4->xmdot*tsince;
  omgadf = TLE->omegao+SGP4->omgdot*tsince;
  xnoddf = TLE->xnodeo+SGP4->xnodot*tsince;
  omega = omgadf;
  xmp = xmdf;
  tsq = tsince*tsince;
  xnode = xnoddf+SGP4->xnodcf*tsq;
  tempa = 1-SGP4->c1*tsince;
  tempe = TLE->bstar*SGP4->c4*tsince;
  templ = SGP4->t2cof*tsq;
  if (!SGP4->simple) {
    delomg = SGP4->omgcof*tsince;
    delm = SGP4->xmcof*(pow(1+SGP4->eta*cos(xmdf),3)-SGP4->delmo);
    temp = delomg+delm;
    xmp = xmdf+temp;
    omega = omgadf-temp;
    tcube = tsq*tsince;
    tfour = tsince*tcube;
    tempa = tempa-SGP4->d2*tsq-SGP4->d3*tcube-SGP4->d4*tfour;
    tempe = tempe+TLE->bstar*SGP4->c5*(sin(xmp)-SGP4->sinmo);
    templ = templ+SGP4->t3cof*tcube+tfour*(SGP4->t4cof+tsince*SGP4->t5cof);
  }

  a = SGP4->aodp*pow(tempa,2);
  e = TLE->eo-tempe;
  xl = xmp+omega+xnode+SGP4->xnodp*templ;
  beta = sqrt(1-e*e);
  xn = XKE/pow(a,1.5);

  /* Long period periodics */
  axn = e*cos(omega);
  temp = 1/(a*beta*beta);
  xll = temp*SGP4->xlcof*axn;
  aynl = temp*SGP4->aycof;
  xlt = xl+xll;
  ayn = e*sin(omega)+aynl;

  /* Solve Kepler's' Equation */
  capu = fmod2p(xlt-xnode);
  temp2 = capu;

  i = 0;
  do {
    sinepw = sin(temp2);
    cosepw = cos(temp2);
    temp3 = axn*sinepw;
    temp4 = ayn*cosepw;
    temp5 = axn*cosepw;
    temp6 = ayn*sinepw;
    epw = (capu-temp4+temp3-temp2)/(1-temp5-temp6)+temp2;
    if(fabs(epw-temp2) <= E6A) break;
    temp2 = epw;
  } while( i++ < 10 );

  /* Short period preliminary quantities */
  ecose = temp5+temp6;
  esine = temp3-temp4;
  elsq = axn*axn+ayn*ayn;
  temp = 1-elsq;
  pl = a*temp;
  r = a*(1-ecose);
  temp1 = 1/r;
  rdot = XKE*sqrt(a)*esine*temp1;
  rfdot = XKE*sqrt(pl)*temp1;
  temp2 = a*temp1;
  betal = sqrt(temp);
  temp3 = 1/(1+betal);
  cosu = temp2*(cosepw-axn+ayn*esine*temp3);
  sinu = temp2*(sinepw-ayn-axn*esine*temp3);
  u = actan(sinu,cosu);
  sin2u = 2*sinu*cosu;
  cos2u = 2*cosu*cosu-1;
  temp = 1/pl;
  temp1 = CK2*temp;
  temp2 = temp1*temp;

  /* Update for short periodics */
  rk = r*(1-1.5*temp2*betal*SGP4->x3thm1)+0.5*temp1*SGP4->x1mth2*cos2u;
  uk = u-0.25*temp2*SGP4->x7thm1*sin2u;
  xnodek = xnode+1.5*temp2*SGP4->cosio*sin2u;
  xinck = TLE->xincl+1.5*temp2*SGP4->cosio*SGP4->sinio*cos2u;
  rdotk = rdot-xn*temp1*SGP4->x1mth2*sin2u;
  rfdotk = rfdot+xn*temp1*(SGP4->x1mth2*cos2u+1.5*SGP4->x3thm1);

  /* Orientation vectors */
  sinuk = sin(uk);
  cosuk = cos(uk);
  sinik = sin(xinck);
  cosik = cos(xinck);
  sinnok = sin(xnodek);
  cosnok = cos(xnodek);
  xmx = -sinnok*cosik;
  xmy = cosnok*cosik;
  ux = xmx*sinuk+cosnok*cosuk;
  uy = xmy*sinuk+sinnok*cosuk;
  uz = sinik*sinuk;
  vx = xmx*cosuk-cosnok*sinuk;
  vy = xmy*cosuk-sinnok*sinuk;
  vz = sinik*cosuk;

  /* Position and velocity */
  pos->x = rk*ux;
  pos->y = rk*uy;
  pos->z = rk*uz;
  vel->x = rdotk*ux+rfdotk*vx;
  vel->y = rdotk*uy+rfdotk*vy;
  vel->z = rdotk*uz+rfdotk*vz;
 
  /* roll in the conversions so they are meaningful */

  pos->x=pos->x*XKMPER;
  pos->y=pos->y*XKMPER;
  pos->z=pos->z*XKMPER;
 
  vel->x=vel->x*XKMPER*XMNPDA/SECDAY;
  vel->y=vel->y*XKMPER*XMNPDA/SECDAY;
  vel->z=vel->z*XKMPER*XMNPDA/SECDAY;
  return 0;
}


 int SGP4GeodeticPosition(double time,struct SGP4XYZVector *pos,
                          struct SGP4Geodetic *geo) {

  double r,e2,phi,c;

  geo->theta = actan(pos->y,pos->x);

  geo->lon = fmod2p(geo->theta - thetag_jd(time));
  r = sqrt(pos->x*pos->x + pos->y*pos->y);
  e2 = F*(2 - F);
  geo->lat = actan(pos->z,r);

  do {
    phi = geo->lat;
    c = 1/sqrt(1 - e2*sin(phi)*sin(phi));
    geo->lat = actan(pos->z + XKMPER*c*e2*sin(phi),r);
  } while(fabs(geo->lat - phi) >= 1E-10);

  geo->alt = r/cos(geo->lat) - XKMPER*c;

  if (geo->lat > PIO2) geo->lat -= TWOPI;

  geo->lon=geo->lon/DE2RA;
  geo->lat=geo->lat/DE2RA;
  return 0;
}


double SGP4YMDHMSToTsince(int yr,int mo,int dy,int hr,int mt,double sc,
                          struct SGP4TLE *TLE) {
  double jdoy;
  jdoy=TimeYMDHMSToJulian(yr,mo,dy,hr,mt,sc)+5.787937e-6;
  return (jdoy-SGP4TLEToJulian(TLE))*XMNPDA;
}

double SGP4EpochToTsince(double tval,struct SGP4TLE *TLE) {

  int yr,mo,dy,hr,mt;
  double sc;
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  return SGP4YMDHMSToTsince(yr,mo,dy,hr,mt,sc,TLE);
}

double SGP4EpochToJdoy(double tval) {

  int yr,mo,dy,hr,mt;
  double sc;
  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  return TimeYMDHMSToJulian(yr,mo,dy,hr,mt,sc)+5.787937e-6;
}

double SGP4YMDHMSToJdoy(int yr,int mo,int dy,int hr,int mt,double sc) {
  return TimeYMDHMSToJulian(yr,mo,dy,hr,mt,sc)+5.787937e-6;
}
