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
"rtcsd - Creates a csd format file from a fitacf TCP/IP data stream.\n",
"rtcsd --help\n",
"rtcsd [-vb] [-t minpwr] [-b bmnum] [-a] [-pwr pmax] [-vel vmax] [-wdt wmax] [-m bufsze] [-r repeat] [-tl tlen] [-nd] [-s stream] [-L logname] [-p pathname] [-f filename] [-if pidname] [-s  stream] host port\n",
"rtcsd -rpf [-vb] [-t minpwr] [-b bmnum] [-a] [-pwr pmax] [-vel vmax] [-wdt wmax] [-m bufsze] [-r repeat] [-tl tlen] [-nd] [-L logname] [-p pathname] [-f filename] [-if pidname] [-s  stream] host portname\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-t minpwr\tfilter out data with values of lag-zero power less than minpwr.\n",
"-b bmnum\tstore data from the beam bmnum only.\n",
"-a\tstore the lambda power, velocity and lambda spectral width.\n",
"-pwr pmax\tstore lambda power and set the maximum of the power scale to pmax.\n",
"-vel vmax\tstore velocity and set the maximum of the velocity scale to +vmax and the minimum to -vmin.\n",
"-wdt wmax\tstore lambda spectral width and set the maximum of the spectral width scale to wmax.\n",
"-m bufsze\tuse a memory buffer of bufsze bytes.\n",
"-r repeat\tinstead of writing the buffer at the end of each scan, write the buffer after repeat packets are received.\n",
"-tl hr:mn\tbuffer hr:mn hours of data.\n",
"-nd\tdo not create daily files.\n",
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-p pathname\tstore the daily files in the directory pathname.\n",
"-f filename\tstore the buffered data in the file filename.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-s stream\tfor a multiplexed stream, read the stream number given by stream. A value of 255 will read all streams\n",
"host\thostname or IP number of the system to connect to.\n",
"rport\tport number to connect to on the server.\n",
"-rpf\tThe remote port number is stored in a text file.\n",
"rportname\tfilename of the text file containing the remote port number.\n",

NULL};
