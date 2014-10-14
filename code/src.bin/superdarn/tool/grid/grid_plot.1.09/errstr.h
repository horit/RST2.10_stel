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
"grid_plot - Plot the grid and grdmapformat files.\n",
"grid_plot --help\n",
"grid_plot [options] gridname [inxname]\n",
"grid_plot -stdout [options] gridname [inxname]\n",
"grid_plot -ps [-xp xoff] [-yp yoff] [options] gridname [inxname]\n",
"grid_plot -stdout -ps [-land] [-xp xoff] [-yp yoff] [options] gridname [inxname]\n",
"grid_plot -ppm [options] gridname [inxname]\n",
"grid_plot -ppm -stdout [options] gridname [inxname]\n",
"grid_plot -ppmx [options] gridname [inxname]\n",
"grid_plot -ppmx -stdout [options] gridname [inxname]\n",
"grid_plot -xml [options] gridname [inxname]\n",
"grid_plot -xml -stdout [options] gridname [inxname]\n",
"grid_plot -x [-display display] [-xoff xoff] [-yoff yoff] [-delay delay] [options] gridname [inxname]\n",

NULL};
