/* errstr.h
   ========
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


char *errstr[]={
"field_plot - Plot fit, fitacf, cfit and csd format files.\n",
"field_plot --help\n",
"field_plot [options] fitname [inxname]\n",
"field_plot -new [options] [fitacfname] [inxname]\n",
"field_plot -cfit [options] [cfitname]\n",
"field_plot -csd [options] [csdname]\n",
"field_plot -ps [-xp xoff] [-yp yoff] [options] fitname [inxname]\n",
"field_plot -new -ps [-xp xoff] [-yp yoff] [options] [fitacfname] [inxname]\n",
"field_plot -cfit -ps [-xp xoff] [-yp yoff] [options] [cfitname]\n",
"field_plot -csd -ps [-xp xoff] [-yp yoff] [options] [csdname] [csdname]\n",
"field_plot -ppm [options] fitname [inxname]\n",
"field_plot -new -ppm [options] [fitacfname] [inxname]\n",
"field_plot -cfit -ppm [options] [cfitname]\n",
"field_plot -csd -ppm [options] [csdname] \n",
"field_plot -ppmx [options] fitname [inxname]\n",
"field_plot -new -ppmx [options] [fitacfname] [inxname]\n",
"field_plot -cfit -ppmx [options] [cfitname]\n",
"field_plot -csd -ppmx [options] [csdname]\n",
"field_plot -xml [options] fitname [inxname]\n",
"field_plot -new -xml [options] [fitacfname] [inxname]\n",
"field_plot -cfit -xml [options] [cfitname]\n",
"field_plot -csd -xml [options] [csdname]\n",
"field_plot -x [-display display] [-xoff xoff] [-yoff yoff] [-delay delay] [options] fitname [inxname]\n",
"field_plot -new -x [-display display] [-xoff xoff] [-yoff yoff] [-delay delay] [options] [fitacfname] [inxname]\n",
"field_plot -cfit -x [-display display] [-xoff xoff] [-yoff yoff] [-delay delay] [options] [cfitname]\n",
"field_plot -csd -x [-display display] [-xoff xoff] [-yoff yoff] [-delay delay] [options] [csdname]\n",

NULL};
