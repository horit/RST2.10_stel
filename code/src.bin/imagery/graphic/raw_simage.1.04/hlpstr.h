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
"raw_simage - Plot an sImage file.\n",
"raw_simage --help\n",
"raw_simage [options] filename\n",
"fov_plot -ps [-land] [-xp xoff] [-yp yoff] [options]\n",
"raw_simage -ppm [options] filename\n",
"raw_simage -ppmx [options] filename\n",
"raw_simage -xml [options] filename\n",
"raw_simage -x [-display display] [-xoff xoff] [-yoff yoff] [options] filename \n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-wdt width\tset the width of the plot to width.\n",
"-hgt height\tset the height of the plot to height.\n",
"-inx color\tfor a multi-color image, plot the color channel color.\n",
"-key keyname\tload the color bar from the file vkeyname.\n",
"-min min\tset the minium value of the intensity scale to min.\n",
"-max max\tset the maximum value of the intensity scale to max.\n",
"-log\tuse a logarithmic scale.\n",
"-stretch\tUse the full range of the color bar. By default the the color bar ranges from zero to the maximum intensity.\n",
"-smooth\tApply a simple linear interpolation to smooth the image.\n",
"-time\tLabel the plot with the time and date.\n",
"-gl\tOnly plot those pixels that are marked as being on the globe.\n",
"-fontname fontname\tplot any labels using the font fontname.\n",
"-fontsize fontsize\tset the font size to fontsize points.\n",
"-bgcol rrggbb\tset the background color to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-txtcol rrggbb\tset the color of the text to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-ps\tproduce a PostScript plot as the output.\n",
"-xp xoff\tset the X offset of the PostScript plot to xoff.\n",
"-yp xoff\tset the Y offset of the PostScript plot to yoff.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",
"-x\tplot on an X-terminal.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",
"filename\tfilename of the sImage data.\n",

NULL};
