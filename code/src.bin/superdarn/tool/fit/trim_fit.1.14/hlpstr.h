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
"trim_fit - Extracts a section from a fit or fitacf format file.\n",
"trim_fit --help\n",
"trim_fit [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] fitinput fitoutput\n",
"trim_fit -i [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] fitinput inxinput fitoutput inxoutput\n",
"trim_fit -new [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] [fitacfname]\n",
"trim_fit -i -new [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] fitacfname inxname \n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-cp cp\textract only those records with the program identifier cp.\n",
"-cn channel\textract only those records from channel channel.\n",
"fitinput\tfilename of the fit format file to trim.\n",
"fitoutput\tfilename of the fit format file to create.\n",
"-i\tindex files are available, for fit format files, an index file is created for the ouput file.\n",
"inxinput\tfilename of the index file associated with the fit or fitacf format input file.\n",
"inxoutput\tfilename of the index file to create.\n",
"-new\tthe input file is a fitacf format file.\n",
"fitacfname\tfilename of the fitacf (dat) format file to trim. If this is omitted, the file will be read from standard input\n",

NULL};
