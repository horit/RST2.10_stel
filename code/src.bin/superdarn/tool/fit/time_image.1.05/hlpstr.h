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
"time_image - Generate a simple image file of time series data.\n",
"time_image --help\n",
"time_image [options] fitname [inxname]\n",
"time_image -new [options] [fitacfname] [inxname]\n",
"time_image -cfit [options] [cfitname]\n",
"time_image -csd [options] [csdname]\n",
"time_image -smr [options] [smrname]\n",
"time_image -ppm [options] fitname [inxname]\n",
"time_image -ppm -new [options] [fitacfname] [inxname]\n",
"time_image -ppm -cfit [options] [cfitname]\n",
"time_image -ppm -csd [options] [csdname]\n",
"time_image -ppm -smr [options] [smrname]\n",
"time_image -ppmx [options] fitname [inxname]\n",
"time_image -ppmx -new [options] [fitacfname] [inxname]\n",
"time_image -ppmx -cfit [options] [cfitname]\n",
"time_image -ppmx -csd [options] [csdname]\n",
"time_image -ppmx -smr [options] [smrname]\n",
"time_image -xml [options] fitname [inxname]\n",
"time_image -xml -new [options] [fitacfname] [inxname]\n",
"time_image -xml -cfit [options] [cfitname]\n",
"time_image -xml -csd [options] [csdname]\n",
"time_image -xml -smr [options] [smrname]\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-sd yyyymmdd\tplot starting from the date yyyymmdd.\n",
"-st hr:mn\tplot starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop plotting at the date yyyymmdd.\n",
"-et hr:mn\tstop plotting at the time hr:mn.\n",
"-ex hr:mn\tplot an interval whose extent is hr:mn.\n",
"-name name\ttitle the image name, for those format  an interval whose extent is hr:mn.\n",
"-wdt width\tset the width of the plot to width.\n",
"-hgt height\tset the height of the plot to height.\n",
"-bgcol rrggbb\tset the background color to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-gscol rrggbb\tset the color of ground scatter to rrggbb, specified as the hexadecimal value for the 24-bit red,green and blue component color.\n",
"-key keyname\tload the colorkey from the file keyname.\n",
"-b bmnum\tplot beam number bmnum, a value of -1 plots all beams.\n",
"-c channel\tfor stereo mode data, plot channel channel, if omitted then both channels are plotted.\n",
"-cp cpid\tplot beams that contain the program identifier cpid only.\n",
"-scan scan\tplot beams that have a value for the scan flag of scan.\n",
"-p\tplot power.\n",
"-v\tplot velocity.\n",
"-w\tplot spectral width.\n",
"-pe\tplot power error.\n",
"-ve\tplot velocity error.\n",
"-we\tplot spectral width error.\n",
"-n\tplot noise.\n",
"-f\tplot frequency.\n",
"-a\tplot attenuation.\n",
"-av\tplot number of averages.\n",
"-min min\tset the minimum value of the scale to <a>min.\n",
"-max max\tset the maximum value of the scale to <a>max.\n",
"-gs\tcolor ground scatter.\n",
"-gm\tmask out ground scatter.\n",
"-ns\tignore non-scan data (data with a scan flag of -1)..\n",
"-km\tplot in terms of distance in kilometers.\n",
"-frang frang\tforce the distance to the first range to be frang kilometers.\n",
"-erang erang\tforce the distance to the last range to be erang kilometers.\n",
"-rsep rsep\tforce the range separation to be rsep kilometers.\n",
"-lr\tlimit the ranges plotted.\n",
"-srng start\tset the first range to plot to be start.\n",
"-erng end\tset the last range to plot to be end.\n",
"-geo\tplot in terms of geographic latitude.\n",
"-mag\tplot in terms of geomagnetic latitude.\n",
"-latmin latmin\tforce the lower latitude limit to be latmin degrees.\n",
"-latmax latmax\tforce the upper latitude limit to be latmax degrees.\n",
"-fn expr\tapply the mathematic expression expr to the data before it is plotted.\n",
"fitname\tfilename of the fit format file to plot.\n",
"inxname\tfilename of the index file.\n",
"-new\tfile is in the fitacf format.\n",
"fitacfname\tfilename of the fitacf format file to plot.\n",
"-cfit\tfile is in the cfit format.\n",
"cfitname\tfilename of the cfit format file to plot.\n",
"-csd\tfile is in the csd format.\n",
"csd\tfilename of the csd format file to plot.\n",
"-smr\tfile is in the smr format.\n",
"-fbeam fbeam\tassume that the first beam in a scan in the summary file is  fbeam.\n",
"smr\tfilename of the smr format file to plot.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",

NULL};
