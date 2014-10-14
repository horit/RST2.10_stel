 /* map_addhmb.c
   =============
   Author: R.J.Barrnes
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

#include "cnvgrid.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "oldcnvmapread.h"
#include "cnvmapwrite.h"
#include "oldcnvmapwrite.h"
#include "aacgm.h"

#include "hlpstr.h"

/* 
 $Log: map_addhmb.c,v $
 Revision 1.15  2005/08/10 22:11:38  barnes
 Added option to dump command line options.

 Revision 1.14  2004/10/19 16:59:58  barnes
 Fixed bug in writing last record.

 Revision 1.13  2004/10/12 22:41:34  barnes
 Fixed bug that meant old and new files were mixed.

 Revision 1.12  2004/10/12 22:28:55  barnes
 Moved to DataMap format.

 Revision 1.11  2004/10/06 14:36:44  barnes
 Corrected bug that meant the latitude limit could not be set on the command line.

 Revision 1.10  2004/05/10 22:04:03  barnes
 Removed redundant code.

 Revision 1.9  2003/03/09 17:32:47  barnes
 Changed AACGM header name.

 Revision 1.8  2003/02/26 15:44:18  barnes
 Modification to deal with the base libraries.

 Revision 1.7  2001/07/31 13:38:02  barnes
 Added Southern Hemisphere code.

 Revision 1.6  2001/06/27 21:09:22  barnes
 Added license tag

 Revision 1.5  2000/10/24 18:31:37  barnes
 Added the ability to exclude stations from the determination.

 Revision 1.4  2000/08/17 15:00:24  barnes
 Changed return type of main().

 Revision 1.3  2000/08/14 14:32:48  barnes
 Modified the way the time is read in so that seconds can be specified as
 a floating point number.

 Revision 1.2  2000/08/01 17:22:57  barnes
 Added the ability to fix the latitude on the command line.

 Revision 1.1  2000/08/01 12:47:42  barnes
 Initial revision

 */


struct CnvMapData map[3];
struct GridData grd[3];


float latref=59;
int nlat=26;

float bndarr[24][26];
float lathmb[26];
int latcnt[26];

struct OptionData opt;

#define HMBSTEP 50

struct hmbtab {
  int num;
  double *time;
  float *median;
  float *actual;
};


struct hmbtab *load_hmb(FILE *fp) {
  struct hmbtab *ptr;
  int yr,mo,dy,hr,mt;
  float sc;
  float median,actual;
  char line[256];
  int i;
  int cnt=0;
  ptr=malloc(sizeof(struct hmbtab));
  ptr->time=malloc(sizeof(double)*HMBSTEP);
  ptr->median=malloc(sizeof(float)*HMBSTEP);
  ptr->actual=malloc(sizeof(float)*HMBSTEP);
 
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]==' ') || (line[i]=='\t') ||
             (line[i] =='\n'));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;
  
    if (sscanf(line,"%d %d %d %d %d %g %g %g",&yr,&mo,&dy,&hr,&mt,&sc,
              &median,&actual) != 8) continue;

    ptr->time[cnt]=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    ptr->median[cnt]=median;
    ptr->actual[cnt]=actual;
    cnt++;
    if ((cnt % HMBSTEP)==0) {
        int blk;
        blk=1+cnt/HMBSTEP;
        ptr->time=realloc(ptr->time,sizeof(double)*HMBSTEP*blk);
        ptr->median=realloc(ptr->median,sizeof(float)*HMBSTEP*blk);
        ptr->actual=realloc(ptr->actual,sizeof(float)*HMBSTEP*blk);
    }

  }

  ptr->num=cnt;
  ptr->time=realloc(ptr->time,sizeof(double)*cnt);
  ptr->median=realloc(ptr->median,sizeof(float)*cnt);
  ptr->actual=realloc(ptr->actual,sizeof(float)*cnt);
 
  return ptr;
}
 
 
void add_hmb_grd(float latmin,int yr,int yrsec,struct CnvMapData *map) {
  int i;
  float lat;
  float lstp;
  int nlon;
  float mlt,latx,lon;
  float bfac,del_L;

  char *lflg=NULL;
  int c=0;
  int off;

  /* We should do something about the hemisphere here */

  for (lat=latmin+0.5;lat<90;lat++) {  
    nlon=(int) (360*cos(lat*PI/180)+0.5);
    lstp=360.0/nlon;

    if (lflg==NULL) lflg=malloc(nlon);
    else lflg=realloc(lflg,nlon);
    memset(lflg,0,nlon);

    /* walk round in longitude locating cells that should be filled */

    c=0;
    lon=0.5*lstp;
    for (i=0;i<nlon;i++) {
            

      mlt=AACGMConvertMLT(yr,yrsec,lon);
    
      bfac=(90-latmin)/(90-latref);
      del_L=bfac*5.5;
      latx=latmin;
      if ((mlt>=11) && (mlt<=19)) 
        latx=latmin+del_L*(1+cos((PI/8)*(mlt-11)));      
      else if ((mlt<11) && (mlt>=5)) 
        latx=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
   
      if (lat<=latx) {
        lflg[i]=1;
        c++;
      }      
      lon+=lstp;
    }
    if (c==0) break; /* no vectors at this latitude */

    off=map->num_model;
    map->num_model+=c;
    if (map->model==NULL) 
      map->model=malloc(sizeof(struct GridGVec)*map->num_model);
    else map->model=realloc(map->model,sizeof(struct GridGVec)*map->num_model);
     
    lon=0.5*lstp;
    
    for (i=0;i<nlon;i++) {
      if (lflg[i] !=0) {     
        if (map->hemisphere==1) map->model[off].mlat=lat;
        else map->model[off].mlat=-lat; 
        map->model[off].mlon=lon;  
        map->model[off].azm=45.0;
        map->model[off].vel.median=1.0;
        map->model[off].vel.sd=0.0;
        off++;
      }
      lon+=lstp;
    }
  }



}


void make_hmb() {
  int m,n;
  float  mlt,latx;
  float latmin;
  float bfac,del_L;
 
  for (m=0;m<24;m++) {
    for (n=0;n<nlat;n++) {

      mlt=m;
 
      lathmb[n]=n+50.0;
      latmin=lathmb[n];

      bfac=(90-latmin)/(90-latref);
      del_L=bfac*5.5;

      latx=latmin;
      if ((mlt>=11) && (mlt<=19)) latx=latmin+del_L*(1+cos((PI/8)*(mlt-11)));
      else if ((mlt<11) && (mlt>=5)) 
         latx=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
      bndarr[m][n]=latx;
    }
  }
}


void map_addhmb(int yr,int yrsec,struct CnvMapData *map,
		int bndnp,float bndstep,
                float latref,float latmin) {
  int i;
  float bfac,del_L;
  float mlt;

  map->num_bnd=bndnp;

  if (map->bnd_lat==NULL) map->bnd_lat=malloc(sizeof(double)*map->num_bnd);
  else map->bnd_lat=realloc(map->bnd_lat,sizeof(double)*map->num_bnd);

  if (map->bnd_lon==NULL) map->bnd_lon=malloc(sizeof(double)*map->num_bnd);
  else map->bnd_lon=realloc(map->bnd_lon,sizeof(double)*map->num_bnd);

  for (i=0;i<map->num_bnd;i++) {
    map->bnd_lon[i]=i*bndstep;
    mlt=AACGMConvertMLT(yr,yrsec,map->bnd_lon[i]);
     
    bfac=(90-latmin)/(90-latref);
    del_L=bfac*5.5;
  
    if (map->hemisphere==1) map->bnd_lat[i]=latmin;
    else  map->bnd_lat[i]=-latmin;
    if (map->hemisphere==1) {
      if ((mlt>=11) && (mlt<=19)) 
        map->bnd_lat[i]=latmin+del_L*(1+cos((PI/8)*(mlt-11)));      
      else if ((mlt<11) && (mlt>=5)) 
        map->bnd_lat[i]=latmin+del_L*(1+cos((PI/6)*(11-mlt)));
    } else {
      if ((mlt>=11) && (mlt<=19)) 
        map->bnd_lat[i]=-latmin-del_L*(1+cos((PI/8)*(mlt-11)));      
      else if ((mlt<11) && (mlt>=5)) 
        map->bnd_lat[i]=-latmin-del_L*(1+cos((PI/6)*(11-mlt)));
    }

  }

  add_hmb_grd(latmin,yr,yrsec,map);
  if (map->hemisphere==1) map->latmin=latmin;  
  else map->latmin=-latmin; 
}


int latcmp(const void *a,const void *b) {
  float *x,*y;
  x=(float *) a;
  y=(float *) b;

  if (*x<*y) return -1;
  if (*x>*y) return 1;
  return 0;
}

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

  FILE *fp;
  char *lname=NULL;
  char *fname=NULL; 
  
  
  int tme;
  int yrsec=0;
  int i,j;
  int s;

  int cnt=0;

  int yr,mo,dy,hr,mt;
  double sc;

  int cnt_req=3;
  int vel_min=100;
  int tflg=0;

  int mlti;

  
  int c;

  float latmin[3]={0,0,0};
  float lattmp[3];
  float latmed=0;
  float latdef=62;


  float bndstep=5;
  int bndnp;


  int buf=0;
  int idx;

  char *istr;
  int exid[64];
  int exnum=0;

  struct hmbtab *hmbtab=NULL;
  float hmblat=0;

  char *exstr=NULL;

  bndnp=360/bndstep+1;
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"cnt",'i',&cnt_req);
  OptionAdd(&opt,"vel",'i',&vel_min);
  OptionAdd(&opt,"t",'x',&tflg);
  OptionAdd(&opt,"lf",'t',lname);
  OptionAdd(&opt,"lat",'f',&hmblat);
  OptionAdd(&opt,"ex",'t',&exstr);

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

  if (exstr !=NULL) {
    istr=strtok(exstr,",");
    do {
      exid[exnum]=atoi(istr);
      exnum++;
    } while ((istr=strtok(NULL,",")) !=NULL);
  }
   
  if (lname !=NULL) {
    fp=fopen(lname,"r"); 
    if (fp !=NULL) {
      hmbtab=load_hmb(fp);
      fclose(fp);
    }
  }

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"Grid file not found.\n");
    exit(-1);
  }


  if ((hmbtab !=NULL) || (hmblat !=0))  {
      /* take the latitude limits from a file or from a predefined value */
     cnt=0;
     if (old) s=OldCnvMapFread(fp,&map[0],&grd[0]);
     else s=CnvMapFread(fp,&map[0],&grd[0]);
     while (s!=-1) { 

       tme=(grd[buf].st_time+grd[buf].ed_time)/2.0;
       TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
       yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);

       if (hmbtab !=NULL) {
         while ((cnt<hmbtab->num) && (hmbtab->time[cnt]<tme)) cnt++;
         if (cnt==0) {
            latmed=hmbtab->median[0];
            latmin[0]=hmbtab->actual[0];
         } else {
            latmed=hmbtab->median[cnt-1];
            latmin[0]=hmbtab->actual[cnt-1];
         } 
       } else {
         latmed=hmblat;
         latmin[0]=hmblat;
       }  
       if (tflg==0) {
         map_addhmb(yr,yrsec,&map[0],bndnp,bndstep,latref,latmed); 
         if (old) OldCnvMapFwrite(stdout,&map[0],&grd[0]);
         else CnvMapFwrite(stdout,&map[0],&grd[0]);
         TimeEpochToYMDHMS(grd[0].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         if (vb==1) 
              fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	            hr,mt,(int) sc,map[0].latmin); 
       } else {
         TimeEpochToYMDHMS(grd[0].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
         fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                 yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
         if (vb==1) 
         fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                 yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);  
       }
       if (old) s=OldCnvMapFread(fp,&map[0],&grd[0]);
       else s=CnvMapFread(fp,&map[0],&grd[0]);
     }
 

  } else { /* generate the latitude limit from the data */

    make_hmb();
    if (old) s=OldCnvMapFread(fp,&map[buf],&grd[buf]);
    else s=CnvMapFread(fp,&map[buf],&grd[buf]);
    while (s !=-1) {  

      tme=(grd[buf].st_time+grd[buf].ed_time)/2.0;
      TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
      yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,(int) sc);

   
      /* now do the HMB determination */
 
      c=0;
      for (i=0;i<grd[buf].vcnum;i++) 
        if (grd[buf].data[i].vel.median>vel_min) c++;

 

      if (c>0) { /* determine boundary */
        float latdif,l;
        int latc=0;

        for (j=0;j<nlat;j++) latcnt[j]=0;
        for (i=0;i<grd[buf].vcnum;i++) {
          if (fabs(grd[buf].data[i].vel.median)<vel_min) continue;

          /* if on the exclusion list ignore this data */

          for (j=0;j<exnum;j++) if (grd[buf].data[i].st_id==exid[j]) break;
          if (j !=exnum) continue;
	  
          mlti=(int) (AACGMConvertMLT(yr,yrsec,grd[buf].data[i].mlon)+0.5);
          if (mlti==24) mlti=0;
          latdif=90;  
          for (j=0;j<nlat;j++) {
            if (map[buf].hemisphere==1)
               l=fabs(bndarr[mlti][j]-(grd[buf].data[i].mlat-1));
            else l=fabs(bndarr[mlti][j]-(-grd[buf].data[i].mlat-1));
            if (l<latdif) {
              latdif=l;
              latc=j;
	    }
  	  }
          latcnt[latc]++;
        }
    
        for (i=0;(i<nlat) && (latcnt[i]<cnt_req);i++);
        if (i<nlat) latmin[buf]=lathmb[i];
        else latmin[buf]=latdef;

      } else latmin[buf]=latdef;
     
    

      /* add the boundary to the map */

      if (cnt>1) {
     
        /* do median filter here */
        
        memcpy(lattmp,latmin,3*sizeof(float));
        qsort(lattmp,3,sizeof(float),latcmp);
        latmed=lattmp[1];
       
        idx=buf-1;
        if (idx<0) idx+=3;

        if (cnt==2) { /* write the very first record*/

          if (tflg==0) {
            map_addhmb(yr,yrsec,&map[0],bndnp,bndstep,latref,latmed); 
            if (old) OldCnvMapFwrite(stdout,&map[0],&grd[0]);
            else CnvMapFwrite(stdout,&map[0],&grd[0]);

            TimeEpochToYMDHMS(grd[0].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
            if (vb==1) 
              fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	            hr,mt,(int) sc,map[0].latmin); 
	  } else {
 
           TimeEpochToYMDHMS(grd[0].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                 yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
           if (vb==1) 
           fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                 yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);  
	  }
        }

        if (tflg==0) {
          map_addhmb(yr,yrsec,&map[idx],bndnp,bndstep,latref,latmed); 
          if (old) OldCnvMapFwrite(stdout,&map[idx],&grd[idx]);
          else CnvMapFwrite(stdout,&map[idx],&grd[idx]);
          TimeEpochToYMDHMS(grd[idx].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
          if (vb==1) 
            fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	          hr,mt,(int) sc,map[idx].latmin);  
        } else {
           TimeEpochToYMDHMS(grd[idx].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                 yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);
           if (vb==1) 
           fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                 yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);  
        }

      }
      cnt++;
      buf++;
      buf=buf % 3;

      if (old) s=OldCnvMapFread(fp,&map[buf],&grd[buf]);
      else s=CnvMapFread(fp,&map[buf],&grd[buf]);

    }

    if (cnt==0) exit(0); /* no record to write out */
  
    idx=buf-1;
    if (idx<0) idx+=3;

    if (cnt<3) latmed=latmin[idx];
    if (cnt==2) { /* we must write out the first record */
      if (tflg==0) {
            map_addhmb(yr,yrsec,&map[0],bndnp,bndstep,latref,latmed); 
            if (old) OldCnvMapFwrite(stdout,&map[0],&grd[0]);
            else CnvMapFwrite(stdout,&map[0],&grd[0]);
            TimeEpochToYMDHMS(grd[0].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
            if (vb==1) 
              fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	            hr,mt,(int) sc,map[0].latmin); 
	  } else {
 
           TimeEpochToYMDHMS(grd[0].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
           fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
                   yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);
           if (vb==1) 
           fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
                   yr,mo,dy,hr,mt,(int) sc,latmed,latmin[0]);  
	  }
    }
   

    if (tflg==0) {  
      map_addhmb(yr,yrsec,&map[idx],bndnp,bndstep,latref,latmed); 
      if (old) OldCnvMapFwrite(stdout,&map[idx],&grd[idx]);
      else CnvMapFwrite(stdout,&map[idx],&grd[idx]);
      TimeEpochToYMDHMS(grd[idx].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      if (vb==1) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d latmin=%g\n",yr,mo,dy,
	      hr,mt,(int) sc,map[idx].latmin);  
    } else {
      TimeEpochToYMDHMS(grd[idx].st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d %.2g %.2g\n",
              yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);
      if (vb==1) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d latmin: median=%g actual=%g\n",
              yr,mo,dy,hr,mt,(int) sc,latmed,latmin[idx]);  

    }
  }
  return 0;

}






















