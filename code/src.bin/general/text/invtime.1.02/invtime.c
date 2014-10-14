/* invtime.c
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "rtime.h"
#include "option.h"
#include "hlpstr.h"


/*
 $Log: invtime.c,v $
 Revision 1.2  2005/08/08 15:16:26  barnes
 Added option to dump the command line options

 Revision 1.1  2003/02/25 00:29:33  barnes
 Initial revision

 */

char buffer[256];
char cflg[1024];

double strdate(char *text) {
  
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  FILE *fp=NULL;
  unsigned char day=0;
  int status=0;
  int yr,mo,dy,hr,mt;
  double sc;
  double etime;
  double dtval=-1;
  int i,c;
  char *dtstr=NULL;
  char *cstr=NULL;
  char *vstr=NULL;
  unsigned char help=0;
  unsigned char option=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"d",'x',&day);
  OptionAdd(&opt,"sd",'t',&dtstr);
  OptionAdd(&opt,"c",'t',&cstr);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

 
  if (cstr !=NULL) {
    char *tmp;
    tmp=strtok(cstr,",");
    do {
       c=atoi(tmp);
       if (c<2) continue;
       cflg[c-2]=1;
      } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  fprintf(stderr,"%s\n",dtstr);
  if (dtstr !=NULL) dtval=strdate(dtstr);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else fp=stdin;
  
  while ((fscanf(fp,"%lg",&etime)==1) && 
	 (status==0)) {

    if (day==1) {
      hr=(int) etime/3600;
      mt=( (int) etime/60) % 60;
      sc=( (int) etime % 60);
      if (dtval !=-1) { 
        etime+=dtval;
        TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);
      }
    } else TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&hr,&mt,&sc);

    if ((day==0) || (dtval !=-1)) 
       fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d",
                        yr,mo,dy,hr,mt,(int) sc);
    else fprintf(stdout,"%.2d %.2d %.2d",hr,mt,(int) sc);
    if (fgets(buffer,256,fp)==NULL) {
      status=-1;
      break;
    }

    /* okay now print out just the columns asked for */
 
    vstr=strtok(buffer," \t\n");
    i=0;
    do {
      if (cflg[i]==1) fprintf(stdout," %s",vstr);
      i++;
    } while ((vstr=strtok(NULL," \t\n")) !=NULL);
    fprintf(stdout,"\n");
  }


  return 0;

}
  
 







