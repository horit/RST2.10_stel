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
"radar_id - Print out information about the radar sites.\n",
"radar_id --help\n",
"radar_id [-vb] [-n] [-s] [-o] [-h] [-d yyyymmdd] [-t hr:mn] stid\n",
"radar_id -c [-vb] [-n] [-s] [-o] [-h] [-d yyyymmdd] [-t hr:mn] stnum\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Print out the extra information describing the output.\n",
"-n\tprint the station name.\n",
"-s\tprint the operational status of the station.\n",
"-o\tprint the name of the operator of the station.\n",
"-h\tprint the geographic (longitude,latitude), location of the station.\n",
"-d yyyymmdd\tget the hardware information for the date yyyymmdd.\n",
"-t hr:mn\tget the hardware information for the time hr:mn.\n",
"stid\tstation identifier code.\n",
"-c\tget the station identifier code, not the station number (assumes that the station number is given on the command line).\n",
"stnum\tstation number.\n",

NULL};
