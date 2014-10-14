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
"map_addimf - Adds IMF data to a convection map file.\n",
"map_addimf --help\n",
"map_addimf [-vb] [-new] [-ace] [-wind] [-p path] [-d hr:mn] [-ex hr:mn] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-wind] [-p path] [-df delayname] [-ex hr:mn] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-if imfname] [-d hr:mn] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-if imfname] [-df delayname] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-bx bx] [-by by] [-bz bz</bz>] [mapname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-ace\tuse IMF data from ACE.\n",
"-wind\tuse IMF data from wind.\n",
"-p pathname\tset the directory path to search for the IMF data in to pathname.\n",
"-d hr:mn\tset the IMF delay time to  hr:mn.\n",
"-ex hr:mn\tread hr hours and mn minutes of IMF data. By default, 24 hours of data is read.\n",
"mapname\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",
"-df delayname\tread the delay times from the ASCII text file delayname.\n",
"-if imfname\tread the IMF data from the ASCII text file imfname.\n",
"-bx bx\tset the X component of the magnetic field to bx.\n",
"-by bx\tset the Y component of the magnetic field to by.\n",
"-bz bx\tset the Z component of the magnetic field to bz.\n",

NULL};
