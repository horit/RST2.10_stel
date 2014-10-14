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
"geo_texturefb - Generate a texture mapped image of the Earth.\n",
"geo_texturefb --help\n",
"geo_texturefb [options] dayimage [nightimage]\n",
"geo_texturefb -ppm [options] dayimage [nightimage]\n",
"geo_texturefb -ppmx [options] dayimage [nightimage]\n",
"geo_texturefb -xml [options] dayimage [nightimage]\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-wdt width\tset the width of the plot to width.\n",
"-hgt height\tset the height of the plot to height.\n",
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
"-square\tforce the use of a square bounding box around the plot.\n",
"-coast\tplot coastlines.\n",
"-bnd\tplot state boundaries.\n",
"-grd\tplot a grid.\n",
"-lst\tuse local solar time rather than local time.\n",
"-term\tplot the terminator.\n",
"-fterm\tplot a filled, graduated terminator.\n",
"-tmin tmin\tset the filled terminator shading factor to tmin. The shading factor controls how dark the terminator appears, a value of 0 will make it appear black, a value of 1.0 will make the terminator disappear.\n",
"-tscl tscl\tsets the filled terminator graduation factor to tscl. The graduation factor controls how the edge of the terminator fades, a value of 0 will force a hard edge, values of 10 or more will produce a gradual fading.\n",
"-lnewdt lnewdt\tset the line width to lnewdt.\n",
"-bgcol rrggbb\tset the color of the background color to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-grdcol rrggbb\tset the color of the grid to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-cstcol rrggbb\tset the color of the coastline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-bndcol rrggbb\tset the color of the state boundaries to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-trmcol rrggbb\tset the color of the terminator outline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-d yyyymmdd\tplot for the date yyyymmdd.\n",
"-t hr:mn\tplot for the time hr:mn.\n",
"-smooth\tApply a simple linear interpolation to smooth the texture data. This is useful when the plot covers a small area of the globe.\n",
"-name name\tassign the name name to the bitmap. This is used by those image formats that support named images (PPMX,XML).\n",
"-dppm\tthe texture for the day side of the Earth is a Portable PixMap (PPM) image.\n",
"-dppmx\tthe texture for the day size of the  Earth is an extended Portable PixMap (PPMX) image.\n",
"-dxml\tthe texture for the day side of the Earth is an XML image representation.\n",
"-nppm\tthe texture for the night side of the Earth is  a Portable PixMap (PPM) image.\n",
"-nppmx\tthe texture for the night side of the Earth is an extended Portable PixMap (PPMX) image.\n",
"-nxml\tthe texture for the night side of the Earth is an XML image representation.\n",
"dayimage\tfilename of the texture for the day side of the Earth.\n",
"nightimage\tfilename of the texture for the night side of the Earth.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",

NULL};
