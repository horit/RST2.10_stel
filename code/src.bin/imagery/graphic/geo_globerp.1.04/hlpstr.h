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
"geo_globerp - Generate an rPlot XML plot of the Earth.\n",
"geo_globerp --help\n",
"geo_globerp [options]\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-wdt width\tset the width of the plot to width.\n",
"-hgt height\tset the height of the plot to height.\n",
"-pad pad\tset the paddding around the edges of the plot to pad.\n",
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
"-bg\tplot the background as a solid color.\n",
"-coast\tplot coastlines.\n",
"-fcoast\tplot filled coastlines.\n",
"-bnd\tplot state boundaries.\n",
"-grd\tplot a grid.\n",
"-grdlat grdlat\tset the latitude spacing of the grid to grdlat degrees.\n",
"-grdlon grdlon\tset the longitude spacing of the grid to grdlon degrees.\n",
"-igrd\tplot the inverse grid, if the plot is in geographic coordinates, a geomagnetic grid is plotted, if the plot is in geomagnetic coordinates, a geographic grid is plotted.\n",
"-igrdlat igrdlat\tset the latitude spacing of the inverse grid to grdlat degrees.\n",
"-igrdlon igrdlon\tset the longitude spacing of the inverse grid to grdlon degrees.\n",
"-tgrd\tplot the time grid. The grid is always aligned with Midnight.\n",
"-tgrdlat igrdlat\tset the latitude spacing of the time grid to grdlat degrees.\n",
"-tgrdlon igrdlon\tset the longitude spacing of the time grid to grdlon degrees.\n",
"-igrdontop\tplot the inverse grid on top.\n",
"-grdontop\tplot the grid on top.\n",
"-lst\tuse local solar time rather than local time.\n",
"-term\tplot the terminator.\n",
"-fterm\tplot a filled, graduated terminator.\n",
"-lnewdt lnewdt\tset the line width to lnewdt.\n",
"-bgcol rrggbb\tset the background color to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-grdcol rrggbb\tset the color of the grid to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-igrdcol rrggbb\tset the color of the inverse grid to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-tgrdcol rrggbb\tset the color of the time grid to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-cstcol rrggbb\tset the color of the coastline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-bndcol rrggbb\tset the color of the state boundaries to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-lndcol rrggbb\tset the color of the land to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"-seacol rrggbb\tset the color of the sea to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"-trmcol rrggbb\tset the color of the terminator outline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-ftrmcol rrggbb\tset the color of the filled terminator to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-d yyyymmdd\tplot for the date yyyymmdd.\n",
"-t hr:mn\tplot for the time hr:mn.\n",
"-name name\tassign the name name to the rPlot document.\n",

NULL};
