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
"fitacfserver - Simple TCP/IP server program for fitacf and fit format files.\n",
"fitacfserver --help\n",
"fitacfserver [-a] [-d] [-r] [-new] [-L logname] [-pf portname] [-if pidname] [-lp port] [-b bound] [-s stid] [-i intt] name\n",

"--help\tprint the help message and exit.\n",
"-a\treplace the record times in the file with the actual time. \n",
"-d\tthe file contains one complete day of data and the server should send the appropriate record for the current time of day. \n",
"-r\tre-open the file when the end is reached.\n",
"-new\tinput file is in fitacf format rather than fit format.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-pf portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-lp port\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",
"-b bound\tsynchronize scans with an interval of bound seconds.\n",
"-s stid\tset the station identifier in the data records to stid.\n",
"-i intt\tignore the integration time in the data records and instead use intt seconds.\n",
"name\tfilename of the fitacf or fit format file to serve.\n",

NULL};
