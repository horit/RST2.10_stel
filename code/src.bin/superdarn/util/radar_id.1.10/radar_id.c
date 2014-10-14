/* radar_id.c
   ==========
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
#include <string.h>
#include <math.h> 
#include "option.h"
#include "rtime.h"
#include "radar.h" 
#include "errstr.h"
#include "hlpstr.h"


/*
 $Log: radar_id.c,v $
 Revision 1.10  2005/08/10 15:19:05  barnes
 Added option to dump the command line options.

 Revision 1.9  2004/06/03 19:40:59  barnes
 Adopted the use of strings as radar identifier codes and added the option
 to display radar location.

 Revision 1.8  2004/05/10 21:48:52  barnes
 Removed redundant code.

 Revision 1.7  2003/02/25 21:32:42  barnes
 Modification to use the base libraries.

 Revision 1.6  2001/06/27 21:07:31  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:12:02  barnes
 Added Author Name

 Revision 1.4  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.3  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.2  2000/08/11 12:37:40  barnes
 Added help information.

 Revision 1.1  2000/07/07 20:45:44  barnes
 Initial revision
 */

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;
struct OptionData opt;

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

double strtime(char *text) {
  int hr,mn;
  double sc;
  int i,j;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  for (j=i+1;(text[j] !=':') && (text[j] !=0);j++);
  if (text[j]==0) {
    mn=atoi(text+i+1);
    return (double) hr*3600L+mn*60L;
  }
  text[j]=0;
  mn=atoi(text+i+1);
  sc=atof(text+j+1);
  return (double) hr*3600L+mn*60L+sc;
}



int main(int argc,char *argv[]) {
  int arg;
  int yr,mo,dy,hr,mt,isc,usc;
  double sc;
  char *code=NULL;
  int st=0;
  FILE *fp;
  char *envstr;
  char *tmetxt=NULL,*dtetxt=NULL;
  double tval=0,dval=0;
 


  char *nlab[2]={"","Name:"};
  char *slab[2]={"","Status:"};
  char *olab[2]={"","Operator:"};
  char *clab[2]={"","Code Letter:"};
  char *ilab[2]={"","Station ID:"};
  char *hlab[2]={"","Location (lat,lon,alt,boresite):"};

  
  unsigned char nflg=0;
  unsigned char sflg=0;
  unsigned char oflg=0;
  unsigned char cflg=0;
  unsigned char hflg=0;
  unsigned char vb=0; 
  unsigned char help=0;
  unsigned char option=0;

  int status,c;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"n",'x',&nflg);
  OptionAdd(&opt,"s",'x',&sflg);
  OptionAdd(&opt,"o",'x',&oflg);
  OptionAdd(&opt,"c",'x',&cflg);
  OptionAdd(&opt,"h",'x',&hflg);

  OptionAdd(&opt,"t",'t',&tmetxt);
  OptionAdd(&opt,"d",'t',&dtetxt);


  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }




  if (arg==argc) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

  if (tmetxt !=NULL) tval=strtime(tmetxt);
  if (dtetxt !=NULL) dval=strdate(dtetxt);

  tval+=dval;
  if (tval !=0) TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);
  else TimeReadClock(&yr,&mo,&dy,&hr,&mt,&isc,&usc);
  sc=isc;

  if (cflg==0) code=argv[arg];
  else st=atoi(argv[arg]);

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADAR' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  if (cflg==0) st=RadarGetID(network,code);
  else code=RadarGetCode(network,st,0);

  

  if (st==-1) {
    fprintf(stderr,"Unknown station\n");
    exit(0);
  }

  status=RadarGetStatus(network,st);
 

  if (nflg==1) fprintf(stdout,"%s%s\n",nlab[vb],RadarGetName(network,st));

  if (sflg==1) {
      fprintf(stdout,"%s",slab[vb]);
      if (status==1) fprintf(stdout,"Operational\n");
      else if (status==0) fprintf(stdout,"Under Construction\n");
      else if (status==-1) fprintf(stdout,"Obsolete\n");
  }
  if (oflg==1) fprintf(stdout,"%s%s\n",olab[vb],RadarGetOperator(network,st));
  if (cflg==1) {
     fprintf(stdout,"%s",clab[vb]);
     for (c=0;(c<RadarGetCodeNum(network,st));c++) 
        if (c !=0) fprintf(stdout,",%s",RadarGetCode(network,st,c));
        else fprintf(stdout,"%s",RadarGetCode(network,st,c));
     fprintf(stdout,"\n");
  } else  fprintf(stdout,"%s%.2d\n",ilab[vb],st);  

  if (hflg) {
    site=RadarYMDHMSGetSite(RadarGetRadar(network,st),yr,mo,dy,hr,mt,(int) sc);
    fprintf(stdout,"%s%g,%g,%g,%g\n",hlab[vb],site->geolat,site->geolon,
	    site->alt,
	    site->boresite);
  }
  return 0;
}






 



   











