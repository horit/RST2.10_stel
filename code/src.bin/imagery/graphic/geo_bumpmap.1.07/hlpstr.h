/* hlpstr.h
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


char *hlpstr[]={
"geo_bumpmap - Generate a bump-map from elevation data.\n",
"geo_bumpmap --help\n",
"geo_bumpmap [options] elvname\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-wdt width\tset the width of the bump map to width.\n",
"-hgt height\tset the height of the bump map to height.\n",
"-sf scale\tset the scale factor (magnification) to scale. The default scale factor is 1.\n",
"-ortho\tuse an orthographic projection.\n",
"-stereo\tuse a stereographic projection.\n",
"-gvp\tuse a general vertical projection.\n",
"-lat lat\tcenter the plot on the latitude given by lat.\n",
"-lon lon\tcenter the plot on the longitude given by lon.\n",
"-latmin latmin\tadjust the scale factor so that the lowest visible latitude is  latmin. Applies when the stereographic projection is used.\n",
"-mag\tuse magnetic coordinates.\n",
"-rotate\trotate the plot so that the local noon is at the top of the plot.\n",
"-flip\tflip the direction of the X-axis.\n",
"-square\tforce the use of a square bounding box around the bump map.\n",
"-lst\tuse local solar time rather than local time.\n",
"-d yyyymmdd\tplot for the date yyyymmdd.\n",
"-t hr:mn\tplot for the time hr:mn.\n",
"-smooth\tApply a simple linear interpolation to smooth the elevation data. This is useful when the plot covers a small area of the globe.\n",
"-ka ka\tset the ambient co-efficient of the Phong model to ka.\n",
"-kd kd\tset the diffuse co-efficient of the Phong model to kd.\n",
"-ks ks\tset the specular co-efficient of the Phong model to ks.\n",
"-n n\tset the power the specular component is raised to in the Phong model to n.\n",
"elvname\tfilename of the elevation data.\n",

NULL};
