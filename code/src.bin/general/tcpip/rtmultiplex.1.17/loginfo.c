/* loginfo.c
   =========
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

/*
 $Log: loginfo.c,v $
 Revision 1.2  2004/06/21 21:59:11  barnes
 Added extra logging information.

 Revision 1.1  2004/04/27 18:02:29  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


void loginfo(char *fname,char *str) {
  FILE *fp;
  char *date;
  char pid[128];
  char logpath[1024];
  time_t ltime;
  struct tm *time_of_day;
 
  time(&ltime);  
  time_of_day=localtime(&ltime);

  date=asctime(time_of_day);  

  date[strlen(date)-1]=':';
  
  sprintf(pid,"(%d):",getpid());
  fprintf(stderr,date);
  fprintf(stderr,pid);
  fprintf(stderr,str);
  fprintf(stderr,"\n");

  sprintf(logpath,"%s.%.4d%.2d%.2d",fname,1900+
          time_of_day->tm_year,time_of_day->tm_mon+1,
          time_of_day->tm_mday);

  fp=fopen(logpath,"a");
  if (fp==NULL) {
    fprintf(stderr,"WARNING:Log failed.\n");
    return;
  }

  fprintf(fp,date);
  fprintf(fp,pid);
  fprintf(fp,str);
  fprintf(fp,"\n");
  fclose(fp);
}










