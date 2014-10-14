/* time_image.c
   ============
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
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <ctype.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "radar.h"
#include "rpos.h"
#include "rawdata.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "csddata.h"
#include "csdread.h"
#include "cfitread.h"
#include "fitread.h"
#include "fitindex.h"
#include "oldfitread.h"

#include "stdkey.h"
#include "key.h"
#include "tplot.h"
#include "oldfit.h"
#include "fit.h"
#include "smr.h"
#include "csd.h"
#include "cfit.h"
#include "expr.h"
#include "hlpstr.h"
#include "errstr.h"

/*
 $Log: time_image.c,v $
 Revision 1.5  2005/08/15 21:04:47  barnes
 Added extra options for controlling the number of ranges plotted and for
 loading a color koey.

 Revision 1.4  2005/08/10 19:07:42  barnes
 Added option to dump command line options.

 Revision 1.3  2004/10/28 14:01:34  barnes
 Added missing header.

 Revision 1.2  2004/05/10 21:58:04  barnes
 Removed redundant code.

 Revision 1.1  2003/04/25 14:50:12  barnes
 Initial revision

*/




struct OptionData opt;
struct OptionFile *optf=NULL;

struct RadarParm prm;
struct FitData fit;
struct CSDdata csd;
struct CFitdata cfit;

struct FitIndex *inx=NULL;

struct tplot tplot;
struct key key;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

double dmax[10]={30,1000,500,10,200,100,30,20000,3,80};
double dmin[10]={0,-1000,0,0,0,0,0,9000,0,0};

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}

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
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return (double) hr*3600L+mn*60L;
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

  int arg=0;

  char *envstr;

  char *stmestr=NULL;
  char *etmestr=NULL;
  char *sdtestr=NULL;
  char *edtestr=NULL;
  char *exstr=NULL;

  double otime=-1;
  double atime=-1;

  double stime=-1;
  double etime=-1;
  double extime=0;
 
  double sdate=-1;
  double edate=-1;

  FILE *fitfp=NULL;
  struct OldFitFp *oldfitfp=NULL;
  struct CSDfp *csdfp=NULL;
  struct CFitfp *cfitfp=NULL;

  FILE *smrfp=NULL;

  int bmnum=-1;
  char *chtxt=NULL;
  int chnum=-1;
  int cpid=-1; 
  int sflg=0;
  int scan=-8000;


  int dinx=0;
  unsigned char velflg=0;
  unsigned char pwrflg=0;
  unsigned char wdtflg=0;

  unsigned char veleflg=0;
  unsigned char pwreflg=0;
  unsigned char wdteflg=0;

  unsigned char frqflg=0;
  unsigned char nseflg=0;
  unsigned char aveflg=0;
  unsigned char attflg=0;

  unsigned char gsflg=0;
  unsigned char gmflg=0;
  unsigned char nsflg=0;

  double min=-1;
  double max=-1;

  unsigned char lrngflg=0;
  unsigned char kmflg=0;

  int frang=-1;
  int erang=-1;
  int rsep=0;

  int erng=MAX_RANGE;
  int srng=0;
  int sprng=1;
  int eprng=MAX_RANGE;


  unsigned char geoflg=0;
  unsigned char magflg=0;
  float latmin=50;
  float latmax=80;

  char *expr=NULL;

  unsigned int gscolor=0;
  char *gsctxt=NULL;

  struct FrameBuffer *img=NULL;
  char *name=NULL;
  char *dname="tseries";
  char *bgtxt=NULL;
  int wdt=500;
  int hgt=300;
  unsigned int bgcolor=0;

  int lft,rgt,btm,top;

  unsigned char ppmflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmxflg=0;

  unsigned char fitflg=0;
  unsigned char smrflg=0;
  unsigned char csdflg=0;
  unsigned char cfitflg=0;

  unsigned char help=0;
  unsigned char option=0;

  char *cfname=NULL;
  FILE *fp=NULL;

  int rng;

  int fbeam=8;
  
  int yr,mo,dy,hr,mt;
  double sc;

  char *key_fname=NULL;
  FILE *keyfp=NULL;


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

  OptionAdd(&opt,"cf",'t',&cfname);

  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);
  OptionAdd(&opt,"xml",'x',&xmlflg);

  OptionAdd(&opt,"name",'t',&name);
  OptionAdd(&opt,"wdt",'i',&wdt);
  OptionAdd(&opt,"hgt",'i',&hgt);
  OptionAdd(&opt,"bgcol",'t',&bgtxt);

  OptionAdd(&opt,"gscol",'t',&gsctxt);

  OptionAdd(&opt,"key",'t',&key_fname);

  OptionAdd(&opt,"st",'t',&stmestr);
  OptionAdd(&opt,"et",'t',&etmestr);
  OptionAdd(&opt,"sd",'t',&sdtestr);
  OptionAdd(&opt,"ed",'t',&edtestr);
  OptionAdd(&opt,"ex",'t',&exstr);

  OptionAdd(&opt,"fit",'x',&fitflg);
  OptionAdd(&opt,"smr",'x',&smrflg);
  OptionAdd(&opt,"csd",'x',&csdflg);
  OptionAdd(&opt,"cfit",'x',&cfitflg);

  OptionAdd(&opt,"b",'i',&bmnum);
  OptionAdd(&opt,"c",'t',&chtxt);
  OptionAdd(&opt,"cp",'i',&cpid);
  OptionAdd(&opt,"scan",'i',&scan);


  OptionAdd(&opt,"p",'x',&pwrflg);
  OptionAdd(&opt,"v",'x',&velflg);
  OptionAdd(&opt,"w",'x',&wdtflg);

  OptionAdd(&opt,"pe",'x',&pwreflg);
  OptionAdd(&opt,"ve",'x',&veleflg);
  OptionAdd(&opt,"we",'x',&wdteflg);

  OptionAdd(&opt,"n",'x',&nseflg);
  OptionAdd(&opt,"f",'x',&frqflg);
  OptionAdd(&opt,"a",'x',&attflg);
  OptionAdd(&opt,"av",'x',&aveflg);

  OptionAdd(&opt,"min",'d',&min);
  OptionAdd(&opt,"max",'d',&max);

  OptionAdd(&opt,"gs",'x',&gsflg);
  OptionAdd(&opt,"gm",'x',&gmflg);

  OptionAdd(&opt,"ns",'x',&nsflg);

  OptionAdd(&opt,"km",'x',&kmflg);
  OptionAdd(&opt,"frang",'i',&frang);
  OptionAdd(&opt,"erang",'i',&erang);
  OptionAdd(&opt,"rsep",'i',&rsep);
 
  OptionAdd(&opt,"srng",'i',&srng); /* first range */
  OptionAdd(&opt,"erng",'i',&erng); /* end range */
 
  OptionAdd(&opt,"geo",'x',&geoflg);
  OptionAdd(&opt,"mag",'x',&magflg);
 
  OptionAdd(&opt,"latmin",'f',&latmin);
  OptionAdd(&opt,"latmax",'f',&latmax);
 
  OptionAdd(&opt,"lr",'x',&lrngflg); /* limit ranges */


  OptionAdd(&opt,"fn",'t',&expr);

  OptionAdd(&opt,"fbeam",'i',&fbeam);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  if (cfname !=NULL) { /* load the configuration file */
    int farg;
    do {
     fp=fopen(cfname,"r");
     if (fp==NULL) break;
      free(cfname);
      cfname=NULL;
      optf=OptionProcessFile(fp);
      if (optf !=NULL) {
        farg=OptionProcess(0,optf->argc,optf->argv,&opt,NULL);
        OptionFreeFile(optf);
      }   
      fclose(fp);
    } while (cfname !=NULL);
  } 

  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }



  if ((ppmflg==0) && (xmlflg==0) && (ppmxflg==0)) ppmxflg=1;

  if ((wdt==0) || (hgt==0)) {
    fprintf(stderr,"Invalid image size.\n");
    exit(-1);
  }

  bgcolor=FrameBufferColor(0xff,0xff,0xff,0xff);

  if (bgtxt !=NULL) {
     sscanf(bgtxt,"%x",&bgcolor);
     bgcolor=bgcolor | 0xff000000;
  }

  gscolor=FrameBufferColor(0xa0,0xa0,0xa0,0xff);

  if (gsctxt !=NULL) {
    sscanf(gsctxt,"%x",&gscolor);
    gscolor=gscolor | 0xff000000;
  }

  if (key_fname !=NULL) {
    keyfp=fopen(key_fname,"r");
    fprintf(stderr,"%s\n",key_fname); 
    if (keyfp !=NULL) {
      load_key(keyfp,&key);
      fclose(keyfp);
    }
  }


  if (exstr !=NULL) extime=strtime(exstr);
  if (stmestr !=NULL) stime=strtime(stmestr);
  if (etmestr !=NULL) etime=strtime(etmestr);
  if (sdtestr !=NULL) sdate=strdate(sdtestr);
  if (edtestr !=NULL) edate=strdate(edtestr);

   
  if ((smrflg==0) && (csdflg==0) && (cfitflg==0)) fitflg=1;
   
  if (scan !=-8000) sflg=1;


  if (chtxt !=NULL) {
    if (tolower(chtxt[0])=='a') chnum=1;
    if (tolower(chtxt[0])=='b') chnum=2;
  }
  
  dinx=0;
  if (pwrflg) dinx=0;
  if (velflg) dinx=1;
  if (wdtflg) dinx=2;
  if (pwreflg) dinx=3;
  if (veleflg) dinx=4;
  if (wdteflg) dinx=5;

  if (nseflg) dinx=6;
  if (frqflg) dinx=7;
  if (attflg) dinx=8;
  if (aveflg) dinx=9;



  if (min==-1) min=dmin[dinx];
  if (max==-1) max=dmax[dinx];
  
  if (key.num==0) {
    if (dinx==1) {
      key.num=KeyLinearMax;
      key.r=KeyLinearR[1];
      key.g=KeyLinearG[1];
      key.b=KeyLinearB[1];
    } else if (dinx<3) {
      key.num=KeyLinearMax;
      key.r=KeyLinearR[0];
      key.g=KeyLinearG[0];
      key.b=KeyLinearB[0];
    } else {
      key.num=KeyTenBlkMax;
      key.r=KeyTenBlkR[0];
      key.g=KeyTenBlkG[0];
      key.b=KeyTenBlkB[0];
    }
  }

  if (fitflg) {
    if (old) {
       if (arg==argc) {
       OptionPrintInfo(stderr,errstr);
        exit(-1);
      }
      if (argc-arg>1)  oldfitfp=OldFitOpen(argv[arg],argv[arg+1]);
      else oldfitfp=OldFitOpen(argv[arg],NULL);
      if (oldfitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[arg]);
        exit(-1);
      }
      stime=oldfit_find(oldfitfp,&prm,&fit,sdate,stime);
    } else {
      if (arg==argc) fp=stdin;
      else {
         if (argc-arg>1)  {
	   fp=fopen(argv[arg+1],"r");
           if (fp==NULL) {
             fprintf(stderr,"Index not found.\n");
	   } else {
             inx=FitIndexFload(fp);
             fclose(fp);
             if (inx==NULL) {
               fprintf(stderr,"Error reading index.\n");
             }
	   }
	 }
         fitfp=fopen(argv[arg],"r");
         if (fp==NULL) {
          fprintf(stderr,"file %s not found\n",argv[arg]);
          exit(-1);
        }
      }
      stime=fit_find(fitfp,&prm,&fit,sdate,stime,inx);
     
    } 
  } else if (smrflg) {
    if (arg==argc) {
     OptionPrintInfo(stderr,errstr);
      exit(-1);
    }
    smrfp=fopen(argv[arg],"r");
    if (smrfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    }
    
    stime=smr_find(smrfp,&prm,&fit,fbeam,sdate,stime);
  } else if (csdflg) {
    if (arg==argc) {
     OptionPrintInfo(stderr,errstr);
      exit(-1);
    }
    csdfp=CSDOpen(argv[arg]);
    if (csdfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    } 
    stime=csd_find(csdfp,&csd,sdate,stime);
  } else if (cfitflg) {
    if (arg==argc) {
     OptionPrintInfo(stderr,errstr);
      exit(-1);
    }
    cfitfp=CFitOpen(argv[arg]);
    if (cfitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(-1);
    } 
    stime=cfit_find(cfitfp,&cfit,sdate,stime);
  }


  if (etime !=-1) {
    if (edate==-1) etime+=stime - ( (int) stime % (24*3600));
    else etime+=edate;
  } else etime=stime+24*3600;
  if (extime !=0) etime=stime+extime;    

  if (name==NULL) name=dname;
  img=FrameBufferMake(name,wdt,hgt,24);
  FrameBufferClear(img,bgcolor,0x0f);

  if (fitflg) {
    if (old) atime=oldfit_scan(stime,oldfitfp,0,&prm,
                             &fit,bmnum,chnum,cpid,sflg,scan); 
    else atime=fit_scan(stime,fitfp,0,&prm,
                             &fit,bmnum,chnum,cpid,sflg,scan); 
  } else if (smrflg) atime=smr_scan(stime,smrfp,fbeam,0,&prm,
                                  &fit,bmnum,chnum,cpid,sflg,scan); 
  else if (csdflg) atime=csd_scan(stime,csdfp,0,&csd,bmnum,chnum,cpid,sflg,
				  scan); 

  if (kmflg) {
    if (frang==-1) frang=prm.frang;
    if (rsep==0) rsep=prm.rsep;
    if (erang==-1) erang=frang+MAX_RANGE*rsep;
  }
 
  if (geoflg || magflg) { 

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
    if ((fitflg) || (smrflg)) {
     radar=RadarGetRadar(network,prm.stid);
     site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);
    }
    if (csdflg) {
      TimeEpochToYMDHMS(csd.time,&yr,&mo,&dy,&hr,&mt,&sc);
      radar=RadarGetRadar(network,csd.stid);
      site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
    }

    if (cfitflg) {
      TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&sc);
      radar=RadarGetRadar(network,csd.stid);
      site=RadarYMDHMSGetSite(radar,yr,mo,dy,hr,mt,sc);
    }


    if (site==NULL) { 
      fprintf(stderr,"Station not found.\n");
      exit(-1);
    }  
  }

  
  do {   
    otime=atime;
    if ((fitflg) || (smrflg)) fit_tplot(&prm,&fit,&tplot);
    if (csdflg) csd_tplot(&csd,&tplot);
    if (cfitflg) cfit_tplot(&cfit,&tplot);

     if (fitflg) {
      if (old) atime=oldfit_scan(atime,oldfitfp,1,&prm,&fit,
                               bmnum,chnum,cpid,sflg,scan);
      else atime=fit_scan(atime,fitfp,1,&prm,&fit,
                               bmnum,chnum,cpid,sflg,scan); 

    } else if (smrflg) atime=smr_scan(atime,smrfp,fbeam,1,&prm,
                                    &fit,bmnum,chnum,cpid,sflg,scan);
    else if (csdflg) atime=csd_scan(atime,csdfp,1,&csd,bmnum,chnum,cpid,
				    sflg,scan);
    else if (cfitflg) atime=cfit_scan(atime,cfitfp,1,&cfit,bmnum,chnum,cpid,
				      sflg,scan);

    if (atime==-1) break;
    
    if ((tplot.scan<0) && (nsflg)) {
      if (atime>=etime) break;
      continue;
    }  
   
    lft=wdt*(otime-stime)/(etime-stime);
    rgt=wdt*(atime-stime)/(etime-stime);


    if (dinx<6) {

      if (lrngflg==0) {
        sprng=1;
        eprng=tplot.nrang;
      } else {
        sprng=srng+1;
        eprng=erng;
      }
      for (rng=sprng;rng<=eprng;rng++) {

        if ((expr !=NULL) && (eval_expr(expr,&tplot,rng)==0)) continue;
   

        if (rng>0) btm=hgt*(rng-1-srng)/(erng-srng);
        else btm=0; 
        top=hgt*(rng-srng)/(erng-srng);

        if (kmflg) {

          btm=tplot.frang+(rng-1)*tplot.rsep;
          top=tplot.frang+rng*tplot.rsep;
          
          if (top<frang) continue;
          if (btm>erang) continue;
          if (top>=erang) top=erang;
          if (btm<frang) btm=frang;

          btm=hgt*(btm-frang)/(erang-frang);
          top=hgt*(top-frang)/(erang-frang);
	}

        if ((geoflg) || (magflg)) {
          double rho,blat,tlat,lon,tmp;
          if (magflg) RPosMag(0,tplot.bmnum,rng-1,site,tplot.frang,
                                 tplot.rsep,tplot.rxrise,300,&rho,
                                 &blat,&lon);   
          else RPosGeo(0,tplot.bmnum,rng-1,site,tplot.frang,tplot.rsep,
                        tplot.rxrise,300,&rho,&blat,&lon);   
   
          if (magflg) RPosMag(0,tplot.bmnum,rng,site,tplot.frang,tplot.rsep,
                               tplot.rxrise,300,&rho,&tlat,&lon);   
          else RPosGeo(0,tplot.bmnum,rng,site,tplot.frang,tplot.rsep,
                        tplot.rxrise,300,&rho,&tlat,&lon);   

          if (tlat<blat) {
            tmp=blat;
            blat=tlat;
            tlat=blat;
	  }
          btm=hgt*(blat-latmin)/(latmax-latmin);
          top=hgt*(tlat-latmin)/(latmax-latmin);
	}
        

        if (tplot.qflg[rng]==0) continue;
        if ((tplot.gsct[rng]==1) && (gmflg)) continue;
        if ((tplot.gsct[rng]==1) && (gsflg))
          FrameBufferRectangle(img,NULL,lft,hgt-1-top,1+rgt-lft,1+top-btm,1,
                               gscolor,
                               0x0f,0,NULL,NULL); 
        else {
          unsigned int color;
          int c;
          double val=0;
          if (dinx==0) val=tplot.p_l[rng];
          if (dinx==1) val=tplot.v[rng];
          if (dinx==2) val=tplot.w_l[rng];
          if (dinx==3) val=tplot.p_l_e[rng];
          if (dinx==4) val=tplot.v_e[rng];
          if (dinx==5) val=tplot.w_l_e[rng];

          if (val>max) val=max;
          if (val<min) val=min;

          if (dinx==1) {
            if (val>0) val=0.5+val/max;
            else val=0.5-val/min;
	  } else val=(val-min)/(max-min);
        
          c=val*key.num;
          if (c<0) c=0;
          if (c>=key.num) c=key.num-1;
  
          color=FrameBufferColor(key.r[c],key.g[c],key.b[c],0xff);
       
          FrameBufferRectangle(img,NULL,lft,hgt-1-top,rgt-lft,top-btm,1,
			     color,
                             0x0f,0,NULL,NULL); 
        } 
      }
    } else {
      unsigned int color;
      int c;
      double val=0;
     
      if ((dinx==6) && (tplot.noise>0)) val=max*log10(tplot.noise/200.0)/3.0;
      if (dinx==7) val=tplot.tfreq;
      if (dinx==8) val=tplot.atten;
      if (dinx==9) val=tplot.nave;
    
      val=(val-min)/(max-min);
        
      c=val*key.num;
      if (c<0) c=0;
      if (c>=key.num) c=key.num-1;

      color=FrameBufferColor(key.r[c],key.g[c],key.b[c],0xff); 
      FrameBufferRectangle(img,NULL,lft,0,rgt-lft,hgt,1,
			   color,0x0f,0,NULL,NULL); 
    }
 
  } while (atime<etime);


  if (fitflg){
    if (old) OldFitClose(oldfitfp);
    else if (fitfp !=stdin) fclose(fitfp);
  } else if (smrflg) fclose(smrfp);
  else if (csdflg) CSDClose(csdfp);
  
  if (xmlflg) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  return 0; 


}

