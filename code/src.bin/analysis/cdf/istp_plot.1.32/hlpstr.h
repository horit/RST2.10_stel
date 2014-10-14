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
"istp_plot - Plot ISTP data from CDF files.\n",
"istp_plot --help\n",
"istp_plot [-cf cfname] [options]\n",
"istp_plot -ps [-land] [-xp xoff] [-yp yoff] [options]\n",
"istp_plot -ppm [options]\n",
"istp_plot -ppmx [options]\n",
"istp_plot -xml [options]\n",
"istp_plot  -x [-display display] [-xoff xoff] [-yoff yoff] [options]\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-plot names...\tplot the parameters given by the comma separated list of parameters,names.\n",
"-path path\tread the data files from the directory path.\n",
"-w width\tset the width of the plot to width.\n",
"-h height\tset the height of the plot to height.\n",
"-sd yyyymmdd\tplot starting from the date yyyymmdd.\n",
"-st hr:mn\tplot starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop plotting at the date yyyymmdd.\n",
"-et hr:mn\tstop plotting at the time hr:mn.\n",
"-ex hr:mn\tplot an interval whose extent is hr:mn.\n",
"-gse\tplot in GSE coordinates, not GSM.\n",
"-pos\tinclude spacecraft position in the plot.\n",
"-mfi\tplot MFI data.\n",
"-swe\tplot SWE data.\n",
"-mag\tplot MAG data.\n",
"-pla\tplot PLA data.\n",
"-mgf\tplot MGF data.\n",
"-lep\tplot LEP data.\n",
"-cpi\tplot CPI data.\n",
"-ace\tplot ACE data.\n",
"-wind\tplot Wind data.\n",
"-imp8\tplot IMP8 data.\n",
"-geotail\tplot Geotail data.\n",
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
