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
"rtfittofitacf - Convert a fit format TCP/IP data stream to a fitacf format stream.\n",
"rtfittofitacf --help\n",
"rtfittofitacf [-t timeout] [-s stream] [-L logname] [-T timename] [-pf portname] [-if pidname] [-lp lport] host rport\n",
"rtfittofitacf -rpf [-t timeout] [-s stream] [-L logname] [-T timename] [-pf portname] [-if pidname] [-lp lport] host rportname\n",

"--help\tprint the help message and exit.\n",
"-t timeout\tset the time to wait before resetting the connection to timeout seconds\n",
"-s stream\tfor a multiplexed stream, read the stream number given by stream. A value of 255 will read all streams\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-T timename\tlog the time that the last packet was received in the file timename. By default, the time is recorded in time.rt.\n",
"-pf portname\trecord the port number that the server is listening for connections on in the file portname. By default, the port is  recorded in port.id.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-lp lport\tforce the server to listen on the local port number port. By default, the server requests a port number from the Operating System.\n",
"host\thostname or IP number of the system to connect to.\n",
"rport\tport number to connect to on the server.\n",
"-rpf\tThe remote port number is stored in a text file.\n",
"rportname\tfilename of the text file containing the remote port number.\n",

NULL};
