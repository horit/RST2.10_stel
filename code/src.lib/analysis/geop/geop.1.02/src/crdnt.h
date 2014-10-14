/* crdnt.h
   =======
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

/*
 $Log: crdnt.h,v $
 Revision 1.1  2004/07/12 21:28:47  barnes
 Initial revision

*/


int sun(int iyear, int imnth, int iday, int ihour,
	int imin, double rsec);
int kpl(int iy, int m, int id, double u, double f, double al,
		double dt, double *x, double *y, double *a, double *d,
		double *g, double *q);
int sky(double h, double d, double f, double *x, double *y);
double vallt(double xlong);
int idleap(int iyear);
int ttdym(int iyear, int imnth, int *itday);
int daydoy(int iyear, int imnth, int iday, int *idoy);
int doyday(int iyear, int idoy, int *imnth, int *iday);
int sodhms(double sod, int *ihour, int *imin, double *rsec);
int hmssod(int ihour, int imin, double rsec, double *sod);
int soymdh(int iyear, double soy, int *imnth, int *iday, int *ihour,
		int *imin, double *rsec);
int mdhsoy(int iyear, int imnth, int iday, int ihour, int imin,
		double rsec, double *soy);
int dipole(int iyear, int imnth, int iday, int ihour, int imin,
		double rsec);



int setmtr(double *dmmtrx);
int getmtr(double *dmmtrx);
int invtrs();
int cnvrt(double *frmvct, double *tovct);
int geigeo();
int geogei();
int geigse();
int gsegei();
int geism();
int smgei();
int geomag();
int maggeo();
int geosm();
int smgeo();
int geogsm();
int gsmgeo();
int gsexyp(double *posgse, double *xypp);
int xypgse(double *posgse, double *xypp);
int geogse();
int gsegeo();
int gsevdh(double *posgse);
int vdhgse(double *posgse);
int geovdh(double *posgeo);
int vdhgeo(double *posgeo);
int geopen(double *posgeo);
int pengeo(double *posgeo);
int penvdh(double *posgeo);
int gsesm();
int smgse();
int gsmsm();
int smgsm();
int gsegsm();
int gsmgse();
int gsmgsw();
int gsmhng(double *posgsm, double *poshng);
int gsegsw();
int gswgse();
int gsease();
int asegse();
int lmnmod(int jmode);
int gselmn(double *posgse);
int lmngse(double *posgse);
int r_lat_lt(double *posv, double *posa);
   
