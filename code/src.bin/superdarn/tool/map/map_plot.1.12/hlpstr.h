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
"map_plot - Plot map and cnvmap format files.\n",
"map_plot --help\n",
"map_plot [options] mapname [inxname]\n",
"map_plot -stdout [options] mapname [inxname]\n",
"map_plot -ps  [-xp xoff] [-yp yoff] [options] mapname [inxname]\n",
"map_plot -stdout -ps [-xp xoff] [-yp yoff] [options] mapname [inxname]\n",
"map_plot -ppm [options] mapname [inxname]\n",
"map_plot -ppm -stdout [options] mapname [inxname]\n",
"map_plot -ppmx [options] mapname [inxname]\n",
"map_plot -ppmx -stdout [options] mapname [inxname]\n",
"map_plot -xml [options] mapname [inxname]\n",
"map_plot -xml -stdout [options] mapname [inxname]\n",
"map_plot -x [-display display] [-xoff xoff] [-yoff yoff] [-delay delay] [options] mapname [inxname]\n",

"--help\tprint the help message and exit.\n",
"-path pathname\twrite the plots to the directory given by pathname.\n",
"-post post\tpost process the plots using the command post.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-sd yyyymmdd\tplot starting from the date yyyymmdd.\n",
"-st hr:mn\tplot starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop plotting at the date yyyymmdd.\n",
"-et hr:mn\tstop plotting at the time hr:mn.\n",
"-ex hr:mn\tplot an interval whose extent is hr:mn.\n",
"-tn\tgenerate filenames of the form <em>hrmn.sc.xxx</em>.\n",
"-dn\tgenerate filenames of the form <em>yyyymmdd.hrmn.sc.xxx</em>.\n",
"-mn\tuse the time of the midpoint of each record to generate the filename.\n",
"-wdt width\tset the width of the plot to width.\n",
"-hgt height\tset the height of the plot to height.\n",
"-padpad\tset the padding around the edge of the plot to  pad.\n",
"-sf scale\tset the scale factor (magnification) to scale. The default scale factor is 1.\n",
"-ortho\tuse an orthographic projection.\n",
"-lat lat\tcenter the plot on the latitude given by lat.\n",
"-lon lon\tcenter the plot on the longitude given by lon.\n",
"-latmin latmin\tadjust the scale factor so that the lowest visible latitude is  latmin. Applies when the stereographic projection is used.\n",
"-mag\tuse magnetic coordinates.\n",
"-rotate\trotate the plot so that the local noon is at the top of the plot.\n",
"-flip\tflip the direction of the X-axis.\n",
"-square\tforce the use of a square bounding box around the plot.\n",
"-coast\tplot coastlines.\n",
"-fcoast\tplot filled coastlines.\n",
"-bnd\tplot state boundaries.\n",
"-grd\tplot a grid.\n",
"-grdlat grdlat\tset the latitude spacing of the grid to grdlat degrees.\n",
"-grdlon grdlon\tset the longitude spacing of the grid to grdlon degrees.\n",
"-igrd\tplot the inverse grid, if the plot is in geographic coordinates, a geomagnetic grid is plotted, if the plot is in geomagnetic coordinates, a geographic grid is plotted.\n",
"-igrdlat igrdlat\tset the latitude spacing of the inverse grid to grdlat degrees.\n",
"-igrdlon igrdlon\tset the longitude spacing of the inverse grid to grdlon degrees.\n",
"-igrdontop\tplot the inverse grid on top.\n",
"-grdontop\tplot the grid on top.\n",
"-tmk\tplot a clock-dial grid showing the time.\n",
"-fov\tplot the radar field of view.\n",
"-ffov\tplot the filled radar field of view.\n",
"-tmtick tick\tset the grid interval for the time clock-dial to tick hours.\n",
"-lst\tuse local solar time rather than local time.\n",
"-term\tplot the terminator.\n",
"-fterm\tplot a filled terminator.\n",
"-tmlbl\tLabel the time clock-dial.\n",
"-lnewdt lnewdt\tset the line width to lnewdt.\n",
"-bgcol rrggbb\tset the background color to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-txtcol rrggbb\tset the color of the text to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-grdcol rrggbb\tset the color of the grid to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-igrdcol rrggbb\tset the color of the inverse grid to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-cstcol rrggbb\tset the color of the coastline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-bndcol rrggbb\tset the color of the state boundaries to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-lndcol rrggbb\tset the color of the land to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"-seacol rrggbb\tset the color of the sea to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"-tmkcol rrggbb\tset the color of the time clock-dial to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-fovcol rrggbb\tset the color of the field of view outline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-ffovcol rrggbb\tset the color of the filled field of view to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-vecccol rrggbb\tif a colorkey is not used then set the color of vectors to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-ctrcol rrggbb\tset the color of contours to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-hmbcol rrggbb\tset the color of the Heppner-Maynard boundary  to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-trmcol rrggbb\tset the color of the terminator outline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-ftrmcol rrggbb\tset the color of the filled terminator outline to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-vkey vkeyname\tload the velocity colorkey from the file vkeyname.\n",
"-xkey xkeyname\tload the extra colorkey (used to plot power or spectral width) from the file xkeyname.\n",
"-fit\tplot the fitted two-dimensional velocity vectors.\n",
"-raw\tplot raw line of sight velocity vectors.\n",
"-mrg\tplot merged two-dimensional vectors.\n",
"-mod\tplot the model vectors.\n",
"-ctr\tplot potential contours.\n",
"-hmb\tplot Heppner-Maynard boundary.\n",
"-exc\tplot velocity vectors excluded from the fit.\n",
"-pwr\tplot power.\n",
"-swd\tplot spectral width.\n",
"-avg\tif a cell contains more than one data point, plot the average power or spectral width.\n",
"-max\tif a cell contains more than one data point, plot the maximum power or spectral width.\n",
"-min\tif a cell contains more than one data point, plot the minimum power or spectral width.\n",
"-modn\tplot the name of the statistical model.\n",
"-imf\tplot the IMF clock-dial.\n",
"-pot\tplot the cross polar cap potential.\n",
"-extra\tplot extra diagnostic information.\n",
"-source\tplot the source string.\n",
"-vkeyp\tplot the color key for the velocity scale.\n",
"-xkeyp\tplot the extra color key for the power or spectrral width scale.\n",
"-vecp\tplot the example vector.\n",
"-vsf vsf\tset the vector scale factor to vsf.\n",
"-logo\tplot the logo and credits.\n",
"-time\tplot the time of the plotted data.\n",
"-vmax vmax\tset the absolute velocity scale maximum to vmax.\n",
"-pmax pmax\tset the power scale maximum to pmax.\n",
"-wmax vmax\tset the spectral width scale maximum to wmax.\n",
"-frame\tadd a frame around the borders of the plot.\n",
"-over\tthe output plot will be overlaid on another plot, do not paint the background with the bacground color.\n",
"mapname\tfilename of the map or cnvmap format file to plot.\n",
"inxname\tfilename of the index file associated with the  grid format file to plot.\n",
"-stdout\tplot a single data record and write the plot to standard output.\n",
"-d yyyymmdd\tplot the data on date yyyymmdd.\n",
"-t hr:mn\tplot the data at the time hr:mn.\n",
"-ps\tproduce a PostScript plot as the output.\n",
"-xp xoff\tset the X offset of the PostScript plot to xoff.\n",
"-yp xoff\tset the Y offset of the PostScript plot to yoff.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",
"-x\tplot the data on an X-terminal.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",
"-delay delay\tset the delay between frames to delay milliseconds, a value of 0 will pause the frame until a mouse button is pressed.\n",

NULL};
