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
"map_addhmb - Calculates the position of the Heppner Maynard Boundary and adds it to a map format file.\n",
"map_addhmb --help\n",
"map_addhmb [-vb] [-new] [-cnt count] [-vel vmin] [-ex exlist] [mapname]\n",
"map_addhmb -t [-vb] [-new] [-cnt count] [-vel vmin] [-ex exlist] [mapname]\n",
"map_addhmb -lf latname [-vb] [-new] [mapname]\n",
"map_addhmb -lat latmin [-vb] [-new] [mapname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-cnt count\tset the number of scatter points to get a match to be count.\n",
"-vel vmin\tset the minimum velocity to  vmin.\n",
"-ex exlist\texclude data from the stations listed in  exlist, which is a comma separated list of radar station numbers.\n",
"mapname\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",
"-t\tinstead of adding the calculated boundary to the map file, write the record time and the calculated location as ASCII text to standard output.\n",
"-lf latname\tdo not calculate the boundary, instead read it from the ASCII text file latname.\n",
"-lat latmin\tdo not calculate the boundary, instead set the lower latitude boundary to latmin degrees.\n",

NULL};
