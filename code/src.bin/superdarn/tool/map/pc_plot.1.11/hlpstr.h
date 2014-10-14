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
"pc_plot - Plot the cross polar cap potential, scatter statistics or IMF conditions from a convection map file.\n",
"pc_plot --help\n",
"pc_plot [options] name\n",
"pc_plot -ps [-l] [-xp xoff] [-yp yoff] [options] name\n",
"pc_plot -ppm [options] name\n",
"pc_plot -ppmx [options] name\n",
"pc_plot -xml [options] name\n",
"pc_plot -x [-display display] [-xoff xoff] [-yoff yoff] [options] name\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-vb\tverbose. Log information to the console.\n",
"-w width\tset the width of the plot to width.\n",
"-h height\tset the height of the plot to height.\n",
"-sd yyyymmdd\tplot starting from the date yyyymmdd.\n",
"-st hr:mn\tplot starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop plotting at the date yyyymmdd.\n",
"-et hr:mn\tstop plotting at the time hr:mn.\n",
"-ex hr:mn\tplot an interval whose extent is hr:mn.\n",
"-s\tinclude the scatter statistics in the plot.\n",
"-b\tinclude the IMF in the plot.\n",
"-nodelay\tdo not use the IMF delay when plotting the IMF.\n",
"-pmin pmin\tset the minimum of the potential scale to be pmin.\n",
"-pmax pmax\tset the maximum of the potential scale to be pmax.\n",
"-smin smin\tset the minimum of the scatter scale to be smin.\n",
"-smax smax\tset the maximum of the scatter scale to be smax.\n",
"-bxmin bxmin\tset the minimum of the X component of the IMF  scale to be bxmin.\n",
"-bxmax bxmax\tset the maximum of the X component of the IMF scale to be bxmax.\n",
"-bymin bymin\tset the minimum of the Y component of the IMF  scale to be bymin.\n",
"-bymax bymax\tset the maximum of the Y component of the IMF scale to be bymax.\n",
"-bzmin bzmin\tset the minimum of the Z component of the IMF  scale to be bzmin.\n",
"-bzmax bzmax\tset the maximum of the Z component of the IMF scale to be bzmax.\n",
"name\tthe filename of the map format file to plot.\n",
"-ps\tproduce a PostScript plot as the output.\n",
"-xp xoff\tset the X offset of the PostScript plot to xoff.\n",
"-yp xoff\tset the Y offset of the PostScript plot to yoff.\n",
"-l\tset the orientation of the PostScript plot to landscape,\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",
"-x\tplot the data on an X-terminal.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",

NULL};
