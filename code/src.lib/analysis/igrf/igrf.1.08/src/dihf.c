/* dihf.c
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

/*
 $Log: dihf.c,v $
 Revision 1.6  2004/05/04 22:41:38  barnes
 Changed the header name.

 Revision 1.5  2004/04/26 21:23:56  barnes
 Audit - Enforced warning set to "all".

 Revision 1.4  2003/09/12 20:08:08  barnes
 Changed name of math constant header.

 Revision 1.3  2001/06/27 20:43:53  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:55  barnes
 Added Author Name

 Revision 1.1  1999/08/10 12:23:06  barnes
 Initial revision

 */

#include <math.h>
#include "rmath.h"

int dihf(float x, float y, float z, float *d, float *i, 
          float *h, float *f) {
   
    float h2;
    float sn, hpx;

    sn = (float)1e-4;

    h2 = x * x + y * y;
    *h = sqrt(h2);
    *f = sqrt(h2 + z * z);
    if (*f < sn) {
	*d = (float)999.;
	*i = (float)999.;
    } else {
	*i = atan2d(z,*h);

	if (*h < sn) {
	    *d = (float)999.;
	} else {
	    hpx = *h + x;
	    if (hpx < sn) {
		*d = (float) 180.;
	    } else {
		*d = atan2d(y,hpx) * (float)2.;
	    }
	}
    }
    return 0;
} 


