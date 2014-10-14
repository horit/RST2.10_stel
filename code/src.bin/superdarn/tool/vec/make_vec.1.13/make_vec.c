/* make_vec.c
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
#include <time.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "radar.h"
#include "griddata.h"
#include "oldgridread.h"
#include "gridread.h"
#include "rmath.h"
#include "hlpstr.h"

/*
 $Log: make_vec.c,v $
 Revision 1.19  2005/08/17 18:10:25  barnes
 Fixed bug that meant the task would free-fall.

 Revision 1.18  2005/08/10 15:40:34  barnes
 Added option to dump the command line options.

 Revision 1.17  2004/10/14 18:05:03  barnes
 Moved to DataMap format for grid files.

 Revision 1.16  2004/06/21 22:52:36  barnes
 Added missing header.

 Revision 1.15  2004/06/04 01:41:38  barnes
 Adopted the use of strings as radar identifier.

 Revision 1.14  2004/05/10 21:47:56  barnes
 Removed redundant code.

 Revision 1.13  2003/04/25 14:53:19  barnes
 Fixed bug in printing empty records.

 Revision 1.12  2003/02/25 23:25:07  barnes
 Modification to use the base libraries.

 Revision 1.11  2001/06/27 21:07:28  barnes
 Added license tag

 Revision 1.10  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.9  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.8  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.7  1999/10/27 22:00:12  barnes
 Fixed bug in the linear regression algorithm.

 */


struct RadarNetwork *network;  

struct GridData rcd;

#define VSTRING "8.0"

struct OptionData opt;


int linreg(struct GridGVec *pnt,int npnt,
	   double *parvel,double *pervel,
	   double *parvelx,double *pervelx,
	   double *frms) {

  int k;

  double sx2=0,cx2=0,ysx=0,ycx=0,cxsx=0;
  double sum=0,den;
  double xval,yval;
  double ds,d2sdv2;
  for (k=0;k<npnt;k++) {
     sx2=sx2+sind(pnt[k].azm)*sind(pnt[k].azm);
     cx2=cx2+cosd(pnt[k].azm)*cosd(pnt[k].azm);
     ysx=ysx-pnt[k].vel.median*sind(pnt[k].azm);
     ycx=ycx-pnt[k].vel.median*cosd(pnt[k].azm);
     cxsx=cxsx+sind(pnt[k].azm)*cosd(pnt[k].azm);
  }   

  den=sx2*cx2-cxsx*cxsx;
  
  if (den==0) return 1;
  
  *parvel=(sx2*ycx-cxsx*ysx)/den;
  *pervel=(cx2*ysx-cxsx*ycx)/den;

  for (k=0;k<npnt;k++) {
    xval=pnt[k].azm;
    yval=*parvel*cosd(xval)+*pervel*sind(xval);
    sum=sum+(pnt[k].vel.median-yval)*(pnt[k].vel.median-yval);
  }
  *frms=sqrt(sum/npnt);
  ds=25.0;

  d2sdv2=1/(*frms*npnt)*cx2;

  *parvelx=sqrt((2*ds)/d2sdv2);
  d2sdv2=1/(*frms*npnt)*sx2;
  *pervelx=sqrt((2*ds)/d2sdv2); 

  return 0;
}

int gvec_sort(const void *a,const void *b) {
  struct GridGVec *x;
  struct GridGVec *y;
  x=(struct GridGVec *) a;
  y=(struct GridGVec *) b;

  if (x->mlat < y->mlat) return -1;
  if (x->mlat > y->mlat) return 1;
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
  char *idstr=NULL;

  char *envstr;
  FILE *fp;   
  char *fname=NULL;
 
  int inx=0;
  int cnt=0;
  int st_id=-1;
  int npnt;
  struct GridGVec *data=NULL;  
  int i,j,k,s=0;
 

  double parvel,pervel,parvelx,pervelx,frms,fac,qn,qr;
  int plat,plon,tflag,pvE,pvP,pvEx,pvPx,qlflag;
  int yr,mo,dy,hr,mt,yrsec;
  double sc;
  time_t ctime;
 
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

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"i",'t',&idstr); 

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

  if (idstr !=NULL) st_id=RadarGetID(network,idstr);
  if (arg<argc) fname=argv[arg];

  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (old) {
    if (OldGridFread(fp,&rcd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(1);
    }
  } else {
   if (GridFread(fp,&rcd)==-1) {
      fprintf(stderr,"Error reading file.\n");
      exit(1);
    }
  }

  ctime = time(NULL);
  if (st_id !=-1) 
    fprintf(stdout,"vlptm %s (QNX) %s\n",VSTRING,RadarGetName(network,st_id));
  else 
    fprintf(stdout,"vlptm %s (QNX) %s\n",VSTRING,
                    RadarGetName(network,rcd.sdata[0].st_id));
  fprintf(stdout,"%s",asctime(gmtime(&ctime)));
  fprintf(stdout,"make_vec\n\n");

  fprintf(stdout,
    "   M.LAT   M.LONG TYP QFL    Veast    Vpole    VEerr    VPerr\n");

  do {
    TimeEpochToYMDHMS(rcd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d",dy,mo,yr,hr,mt,(int) sc);
    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
    fprintf(stdout," %4d %4d %4d %4d %4d %4d    %8d",yr,mo,dy,hr,mt,
	    (int) sc,yrsec);
    TimeEpochToYMDHMS(rcd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (vb==1) fprintf(stderr,"-%d:%d:%d ",hr,mt,(int) sc);
  
    yrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
    fprintf(stdout," %8d\n",yrsec);
    fprintf(stdout,"Vlos: ");
    for (k=0;k<16;k++) {
       fprintf(stdout,"%8.1f",0.0);
       if ((k+1) % 8==0) fprintf(stdout,"\n");
    }
    
 
    if (st_id !=-1) { 
      for (inx=0;(inx<rcd.stnum) && (rcd.sdata[inx].st_id != st_id);inx++); 
      if (inx==rcd.stnum) {
        if (vb==1) fprintf(stderr,"npnt=%d\n",0);
        fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);
        continue;    
      }
    }
   
    if (rcd.stnum==0) {
      if (vb==1) fprintf(stderr,"npnt=%d\n",0);
      fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);
         
    }
 

    if (rcd.sdata[inx].npnt<3)  {
      if (vb==1) fprintf(stderr,"npnt=%d\n",0);
      fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);

      if (old) s=OldGridFread(fp,&rcd);
      else s=GridFread(fp,&rcd);
      if (s==-1) break;
      continue;
    }

  
    npnt=rcd.sdata[inx].npnt;
    data=malloc(sizeof(struct GridGVec)*npnt);
    j=0;
    
    for (i=0;i<rcd.vcnum;i++) {     
      if ((st_id==-1) || (rcd.data[i].st_id==st_id)) {
        memcpy(&data[j],&rcd.data[i],sizeof(struct GridGVec));
        if (data[j].mlat<0)  data[j].azm=data[j].azm-90.0;
        else data[j].azm=90.0-data[j].azm;
        j++;
      }
    }
    

    /* okay best approach is to sort the vector table into latitude order */
    qsort(data,npnt,sizeof(struct GridGVec),gvec_sort);
    j=0;
    cnt=0;
    for (i=0;i<npnt;i++) {
      if (data[i].mlat !=data[j].mlat) {
        /* got the end of one row */
        
        if ((i-j) > 3) {
           if (linreg(&data[j],i-j,&parvel,&pervel,&parvelx,
                      &pervelx,&frms) !=0) continue;
           cnt++;
           plat=100*data[j].mlat;
           plon=0;
           for (k=j;k<i;k++) plon=plon+data[k].mlon;
           plon=100*plon/(i-j);
           tflag=3;
           fac=sqrt(parvel*parvel+pervel*pervel)/500.0;
           pvE=10*parvel;
           pvP=10*pervel;
           pvEx=10*parvelx;
           pvPx=10*pervelx;
           qn=1-(1.0/(16-5))*(i-j-5);
           qr=1-(1.0/(fac*100.0))*((fac*100.0)-frms);
           qlflag=3.0*qn+2.0*qr;           
         
           if (frms> fac*100.0) qlflag=5;
           fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
		   plat,plon,tflag,qlflag,pvE,pvP,pvEx,pvPx);

         
        }
        j=i;
      }
    }  
    
   
    
    if (vb==1) fprintf(stderr,"npnt=%d\n",cnt);
    fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);
    free(data);
    if (old) s=OldGridFread(fp,&rcd);
    else s=GridFread(fp,&rcd);
  } while (s !=-1);

  if (fp !=stdin) fclose(fp);

  return 0;
}






















