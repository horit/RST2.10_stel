 /* map_grd.c
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
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rmath.h"

#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"

#include "cnvgrid.h"
#include "cnvmap.h"
#include "oldcnvmapwrite.h"
#include "cnvmapwrite.h"
#include "aacgm.h"

#include "radar.h" 


/* 
 $Log: map_grd.c,v $
 Revision 1.12  2005/08/10 22:46:05  barnes
 Added option to dump command line options.

 Revision 1.11  2004/10/12 21:52:22  barnes
 Moved to DataMap format.

 Revision 1.10  2004/06/21 22:33:57  barnes
 Added missing header.

 Revision 1.9  2004/05/10 22:07:50  barnes
 Removed redundant code.

 Revision 1.8  2003/03/09 17:35:52  barnes
 Changed AACGM header name.

 Revision 1.7  2003/02/26 15:43:17  barnes
 Removed debugging code.

 Revision 1.6  2003/02/26 03:15:52  barnes
 Modification to deal with base libraries.

 Revision 1.5  2001/10/24 12:48:21  barnes
 Initial Modifications for Southern Hemisphere Data.

 Revision 1.4  2001/07/31 13:39:20  barnes
 Fixed bug in setting the latitude for the southern hemisphere.

 Revision 1.3  2001/07/30 22:54:44  barnes
 Added flag to force Southern Hemisphere.

 Revision 1.2  2001/06/27 21:10:09  barnes
 Added license tag

 Revision 1.1  2000/08/17 15:00:24  barnes
 Initial revision

 */

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;


#include "hlpstr.h"

struct CnvMapData map;
struct GridData grd;

struct OptionData opt;

int main(int argc,char *argv[]) {

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */

  int old=0;
  int new=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  char *envstr;
  FILE *fp;

  char *fname=NULL;
  int tme;
  int yrsec;
  int cnt=0;
  float latmin=60;
  float latshft=0;
  unsigned char sh=0;

  int yr,mo,dy,hr,mt;
  double sc;

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
   
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"sh",'x',&sh);
  OptionAdd(&opt,"l",'f',&latmin);
  OptionAdd(&opt,"s",'f',&latshft);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);
  
  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }  

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  } 

  
  if (arg !=argc) fname=argv[arg];
 
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"Grid file not found.\n");
    exit(-1);
  }

  memset(&map,0,sizeof(struct CnvMapData));  
  map.fit_order=4;
  map.doping_level=1;
  map.model_wt=1;
  map.error_wt=1;
  map.hemisphere=1;
  map.lat_shft=latshft;
  
  if (sh==1) map.hemisphere=-1;
  if (old) {
    while (OldGridFread(fp,&grd) !=-1) {
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (cnt==0) {
        int i;
        for (i=0;i<grd.stnum;i++) {
	  radar=RadarGetRadar(network,grd.sdata[i].st_id);
          if (radar!=NULL) 
            site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
          if ((site !=NULL) && (site->geolat<0)) {
             map.hemisphere=-1;
             break;
           }
        }
      } 

      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map.mlt.start=AACGMConvertMLT(yr,yrsec,0.0);

      TimeEpochToYMDHMS(grd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map.mlt.end=AACGMConvertMLT(yr,yrsec,0.0);

      tme=(grd.st_time+grd.ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map.mlt.av=AACGMConvertMLT(yr,yrsec,0.0);
 
      if (latshft !=0) {
        map.lon_shft=(map.mlt.av-12)*15.0;
        map.latmin-=latshft;
      }

      if (map.hemisphere==1) map.latmin=latmin;  
      else map.latmin=-latmin;

      map.st_time=grd.st_time;
      map.ed_time=grd.ed_time;

      OldCnvMapFwrite(stdout,&map,&grd);
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,
	      hr,mt,(int) sc);  
      cnt++;
    }
  } else {
    while (GridFread(fp,&grd) !=-1) {
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

      if (cnt==0) {
        int i;
        for (i=0;i<grd.stnum;i++) {
	  radar=RadarGetRadar(network,grd.sdata[i].st_id);
          if (radar!=NULL) 
            site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,(int) sc);
          if ((site !=NULL) && (site->geolat<0)) {
             map.hemisphere=-1;
             break;
           }
        }
      } 

      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map.mlt.start=AACGMConvertMLT(yr,yrsec,0.0);

      TimeEpochToYMDHMS(grd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map.mlt.end=AACGMConvertMLT(yr,yrsec,0.0);

      tme=(grd.st_time+grd.ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);
      map.mlt.av=AACGMConvertMLT(yr,yrsec,0.0);
 
      if (latshft !=0) {
        map.lon_shft=(map.mlt.av-12)*15.0;
        map.latmin-=latshft;
      }

      if (map.hemisphere==1) map.latmin=latmin;  
      else map.latmin=-latmin;

      map.st_time=grd.st_time;
      map.ed_time=grd.ed_time;

      CnvMapFwrite(stdout,&map,&grd);
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,
	      hr,mt,(int) sc);  
      cnt++;
    }
  }


  return 0;
}






















