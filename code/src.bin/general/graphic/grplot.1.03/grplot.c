/* grplot.c
   ======== */

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

#ifdef _XLIB_
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "rxml.h"
#include "rfbuffer.h"
#include "rps.h"
#include "rtime.h"
#include "rplot.h"
#include "rplotin.h"
#include "fbrplot.h"
#include "psrplot.h"

#include "fontdb.h"
#include "imagedb.h"

#ifdef _XLIB_
#include "xwin.h"
#endif

#include "rplotg.h"
#include "grplot.h"

#include "hlpstr.h"

/*
 $Log: grplot.c,v $
 Revision 1.5  2005/08/08 15:06:53  barnes
 Added option to dump the command line options

 Revision 1.4  2004/06/21 21:54:09  barnes
 Moved "name" variable.

 Revision 1.3  2004/04/27 17:36:07  barnes
 Audit - Enforced warning set to "all".

 Revision 1.2  2004/04/26 19:04:18  barnes
 Added missing return value.

 Revision 1.1  2003/04/18 23:32:35  barnes
 Initial revision

*/




struct OptionData opt;

void *fileopen(char *fname,char *encode,char *x,char *y,void *data) {
  FILE *fp=NULL;
  int *index;
  index=(int *) data;
  fp=fopen(fname,"r");

  sscanf(x,"%d",&index[0]);
  sscanf(y,"%d",&index[1]);

  index[0]--;
  index[1]--;

  return (void *) fp;
}

void fileclose(void *fv,void *data) {
  FILE *fp=NULL;
  fp=(FILE *) fv;
  if (fp==NULL) return;
  fclose(fp);
}

int fileread(void *fv,char *encode,char *x,char *y,double *xval,
             double *yval,void *data) {
  char *delim=", \t";
  char *t;
  int c=0;
  int max=0;
  char *tok;
  char lbuf[256];
  int *index;
  FILE *fp=NULL;
  fp=(FILE *) fv;

  index=(int *) data;
  max=index[0];
  if (index[1]>max) max=index[1];    

  t=fgets(lbuf,255,fp);
  if (t==NULL) return -1;

  tok=strtok(lbuf,delim);
  do {
    if (c==index[0]) sscanf(tok,"%lg",xval);
    if (c==index[1]) sscanf(tok,"%lg",yval);
    c++;
    if (c>max) break;
    tok=strtok(NULL,delim);
  } while (tok !=NULL);
  return 0;
}


int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
}

int xmldecode(char *buf,int sze,void *data) {
  struct XMLdata *xml;
  xml=(struct XMLdata *) data;
  return XMLDecode(xml,buf,sze);
}

int include(char *name,int (*user)(char *,int,void *),void *udata,
		     void *data) {
  int s=0;
  char lbuf[256];
  FILE *fp;
  if (user==NULL) return 0;

  fp=fopen(name,"r");
  if (fp==NULL) return 0;

  while(fgets(lbuf,255,fp) !=NULL) {
    s=(*user)(lbuf,strlen(lbuf),udata);
    if (s !=0) break;
  }
  fclose(fp);
  return s;
}

int isize(char *name,int *x,int *y,void *data) {
  struct FrameBufferDB *imagedb;
  struct FrameBuffer *img;
  if (name==NULL) return -1;
  if (x==NULL) return -1;
  if (y==NULL) return -1;
  imagedb=(struct FrameBufferDB *)data;
  if (imagedb==NULL) return -1;
  *x=128;
  *y=128;

  img=FrameBufferDBFind(imagedb,name);
  if (img !=NULL) {
    *x=img->wdt;
    *y=img->hgt;
  }
  return 0;
}

int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data) {

  struct FrameBufferFontDB *fontdb;
  struct FrameBufferFont *fnt;
  int tbox[3];

  fontdb=(struct FrameBufferFontDB *)data;
  if (fontdb==NULL) return -1;
  fnt=FrameBufferFontDBFind(fontdb,fntname,sze);
  
  FrameBufferTextBox(fnt,num,txt,tbox);
  
  box[0]=1.1*tbox[0];
  box[1]=1.1*tbox[1];
  box[2]=1.1*tbox[2];
 
  return 0;
}

char *mktext(char *style,double value,double min,double max,void *data) {
  int yr,mo,dy,hr,mt;
  double sc;
  int doy;
  char *txt=NULL;
  txt=malloc(64);
  txt[0]=0;
  if (style==NULL) {
    sprintf(txt,"%g",value);
    return txt; 
  }
  if (strcmp(style,"plain")==0) 
    sprintf(txt,"%g",value);
  else if (strcmp(style,"time.hrmt")==0) {
    hr=(int) value/3600;
    mt=value-hr*3600;
    sprintf(txt,"%.2d:%.2d",hr,mt);
  } else if (strcmp(style,"time.hrmtsc")==0) {
    hr=(int) value/3600;
    mt=(int) (value-hr*3600)/60;
    sc=value-hr*3600-mt*60;
    sprintf(txt,"%.2d:%.2d:%g",hr,mt,sc);
  } else if (strcmp(style,"date.yrmody")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    sprintf(txt,"%.4d%.2d%.2d",yr,mo,dy);
  } else if (strcmp(style,"date.yr-mo-dy")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    sprintf(txt,"%.4d-%.2d-%.2d",yr,mo,dy);
  } else if (strcmp(style,"date.hrmt")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    sprintf(txt,"%.2d:%.2d",hr,mt);
  } else if (strcmp(style,"date.hrmtsc")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    sprintf(txt,"%.2d:%.2d:%g",hr,mt,sc);
  } else if (strcmp(style,"date.yrmodyhrmtsc")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    sprintf(txt,"%.4d%.2d%.2d %.2d:%.2d:%g",yr,mo,dy,hr,mt,sc);
  } else if (strcmp(style,"date.yr-mo-dyhrmtsc")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    sprintf(txt,"%.4d-%.2d-%.2d %.2d:%.2d:%g",yr,mo,dy,hr,mt,sc);
  } else if (strcmp(style,"doy.doy")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    doy=(value-TimeYMDHMSToEpoch(yr,1,1,0,0,0))/(24*3600)+1;
    sprintf(txt,"%.3d",doy);
  } else if (strcmp(style,"doy.yr:doy")==0) {
    TimeEpochToYMDHMS(value,&yr,&mo,&dy,&hr,&mt,&sc);
    doy=(value-TimeYMDHMSToEpoch(yr,1,1,0,0,0))/(24*3600)+1;
    sprintf(txt,"%.4d:%.3d",yr,doy);
  }

  return txt;
}

int cnvtext(char *style,char *txt,double *value,void *data) {
  char *delim={"-: "};
  if (txt==NULL) return -1;
  if (value==NULL) return -1;
  if (strcmp(style,"double")==0) {
    *value=atof(txt);
    return 0;
  } else if (strcmp(style,"day")==0) {
    *value=atof(txt)*24*3600;
    return 0;
  } else if (strcmp(style,"time")==0) {
    int c=0,dflg=0;
    int yr,mo,dy,hr,mt;
    double sc;
    double dval[6];

    char *tmp;
    char *t;
    *value=0;
    tmp=malloc(strlen(txt)+1);
    strcpy(tmp,txt);
    t=strtok(tmp,delim);    

    if (strlen(t)==8) { /* yrmody */
      dval[2]=atof(t+6);
      t[6]=0;
      dval[1]=atoi(t+4);
      t[4]=0;
      dval[0]=atoi(t);
      c=3;
      dflg=1;
    } else {
      dval[0]=atof(t);
      c=1;
    }
    while (((t=strtok(NULL,delim)) !=NULL) && (c<6)) {
      dval[c]=atof(t);
      c++;
    }

    if (c==6) {
      yr=dval[0];
      mo=dval[1];
      dy=dval[2];
      hr=dval[3];
      mt=dval[4];
      sc=dval[5];
      *value=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    } else if (c==5) {
      yr=dval[0];
      mo=dval[1];
      dy=dval[2];
      hr=dval[3];
      mt=dval[4];
      sc=0;
      *value=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
    } else if ((c==3) && (dflg==1)) {
      yr=dval[0];
      mo=dval[1];
      dy=dval[2];
      *value=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0.0); 
   } else if ((c==3) && (dflg==0)) {
      hr=dval[0];
      mt=dval[1];
      sc=dval[2];
      *value=hr*3600+mt*60+sc;
    } else if (c==2) {
      hr=dval[0];
      mt=dval[1];
      *value=hr*3600+mt*60;
    }
    free(tmp);
    return 0;
  }
 
  *value=atof(txt);
  return 0;
}

void loadppm(struct FrameBufferDB *imagedb,struct OptionText *ppm) {
 FILE *fp;
 int i;
 struct FrameBuffer *img=NULL;
 if (ppm==NULL) return;
 for (i=0;i<ppm->num;i++) {
    fp=fopen(ppm->txt[i],"r");
    if (fp==NULL) continue;
    img=FrameBufferLoadPPM(fp,ppm->txt[i]);
    fclose(fp);
    if (img==NULL) continue;
    FrameBufferDBAdd(imagedb,img);
 }
}

void loadppmx(struct FrameBufferDB *imagedb,struct OptionText *ppmx) {
 FILE *fp;
 int i;
 struct FrameBuffer *img=NULL;
 if (ppmx==NULL) return;
 for (i=0;i<ppmx->num;i++) {
    fp=fopen(ppmx->txt[i],"r");
    if (fp==NULL) continue;
    img=FrameBufferLoadPPMX(fp);
    fclose(fp);
    if (img==NULL) continue;
    FrameBufferDBAdd(imagedb,img);
  }
}

void loadxml(struct FrameBufferDB *imagedb,struct OptionText *xml) {
  struct FrameBuffer *img=NULL;
  struct XMLdata *xmldata=NULL;
  struct FrameBufferXML xmlimg;

  FILE *fp;
  char lbuf[256];
  int i;
  int s=0;

  if (xml==NULL) return;

  xmldata=XMLMake();
  xmlimg.xml=xmldata;
  xmlimg.img=&img;

  XMLSetStart(xmldata,FrameBufferXMLStart,&xmlimg);
  XMLSetEnd(xmldata,FrameBufferXMLEnd,&xmlimg);

  for (i=0;i<xml->num;i++) {
    fp=fopen(xml->txt[i],"r");
    if (fp==NULL) continue;
    while(fgets(lbuf,255,fp) !=NULL) {
      s=XMLDecode(xmldata,lbuf,strlen(lbuf));
      if (s !=0) break;
    }
    fclose(fp);
    if (s !=0) continue;
    if (img==NULL) continue;
    FrameBufferDBAdd(imagedb,img);
    img=NULL;
  }
  XMLFree(xmldata);
}

int main(int argc,char *argv[]) {
  

#ifdef _XLIB_
  char *name="grplot";
  int xdf=0;
  int xd=0;
  struct XwinDisplay *dp;
  struct XwinWindow *win;
  char *display_name=NULL;
  int xdoff=-1;
  int ydoff=-1;
#endif

  int s=0;
  int arg=0;
  FILE *fp;
  char lbuf[256];

  int index[2]={0,0};

  struct FrameBuffer *img=NULL;
  struct PostScript *psdata=NULL;

  struct FrameBufferRplot *fbplot=NULL;
  struct PostScriptRplot *psplot=NULL;
  struct RplotInPlot *rplotin=NULL;


  struct FrameBufferFontDB *fontdb=NULL;
  struct FrameBufferDB *imagedb=NULL;

  struct XMLdata *xmldata=NULL;
  struct XMLdata *xmlplot=NULL;
  struct Rplot *rplot=NULL;
  struct GrPlotData *grplot=NULL;   

  char *fntdbfname=NULL;

  unsigned char land=0;
  float xpoff=40,ypoff=40;
   unsigned char psflg=0;
  unsigned char xmlflg=0;
  unsigned char ppmflg=0;
  unsigned char ppmxflg=0;


  unsigned char rflg=0;
  unsigned char gflg=0;
  unsigned char pflg=0;

  unsigned char help=0;
  unsigned char option=0;

  struct OptionText *ppmnme=NULL;
  struct OptionText *ppmxnme=NULL;
  struct OptionText *xmlnme=NULL;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"ps",'x',&psflg);
  OptionAdd(&opt,"ppm",'x',&ppmflg);
  OptionAdd(&opt,"ppmx",'x',&ppmxflg);

  OptionAdd(&opt,"xml",'x',&xmlflg);
  OptionAdd(&opt,"land",'x',&land);
  OptionAdd(&opt,"xp",'f',&xpoff);
  OptionAdd(&opt,"yp",'f',&ypoff);


  OptionAdd(&opt,"fdb",'t',&fntdbfname);
  OptionAdd(&opt,"ippm",'a',&ppmnme);
  OptionAdd(&opt,"ippmx",'a',&ppmxnme);
  OptionAdd(&opt,"ixml",'a',&xmlnme);

#ifdef _XLIB_ 
  OptionAdd(&opt,"x",'x',&xd);
  OptionAdd(&opt,"display",'t',&display_name);
  OptionAdd(&opt,"xoff",'i',&xdoff);
  OptionAdd(&opt,"yoff",'i',&ydoff);
#endif

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
      OptionDump(stdout,&opt);
      exit(0);
  }


  if (psflg) pflg=1;
  if (xmlflg) gflg=1; 
  if (ppmflg) gflg=1;
  if (ppmxflg) gflg=1;

#ifdef _XLIB_ 
   if (xd !=0) {
     pflg=0; 
     gflg=1;
   }
#endif
     
  if (pflg) gflg=0;
  rflg=pflg | gflg;
 
  if (fntdbfname==NULL) fntdbfname=getenv("FONTDB");
  if (fntdbfname !=NULL) {
    fp=fopen(fntdbfname,"r");
    if (fp !=NULL) {
      fontdb=FrameBufferFontDBLoad(fp);
      fclose(fp);
    }
  }
  if (fontdb==NULL) {
    fprintf(stderr,"Could not load fonts.\n");
    exit(-1);
  }
 
  imagedb=FrameBufferDBMake();
  loadxml(imagedb,xmlnme);
  loadppm(imagedb,ppmnme);
  loadppmx(imagedb,ppmxnme);


  if (rflg) {
    xmlplot=XMLMake();
    rplotin=RplotInMake(xmlplot);

    if (gflg) {
      fbplot=FrameBufferRplotMake(fontdb,imagedb);
      FrameBufferRplotSetAll(rplotin,fbplot);
    }
    if (pflg) {
      psdata=PostScriptMake();
      PostScriptSetText(psdata,stream,stdout);
      psplot=PostScriptRplotMake(psdata,imagedb);
      PostScriptRplotSetAll(rplotin,psplot);
      psplot->land=land;
      psplot->xoff=xpoff;
      psplot->yoff=ypoff;
    }
    XMLSetStart(xmlplot,RplotInStart,rplotin);
    XMLSetEnd(xmlplot,RplotInEnd,rplotin);
    XMLSetText(xmlplot,RplotInText,rplotin);
  }


  xmldata=XMLMake();
  rplot=RplotMake();
 
  if (rflg) RplotSetText(rplot,xmldecode,xmlplot);
  else RplotSetText(rplot,stream,stdout);

  grplot=GrPlotMake(xmldata);

  GrPlotSetRplot(grplot,rplot);
  GrPlotSetTextBox(grplot,txtbox,fontdb);
  GrPlotSetMakeText(grplot,mktext,NULL);
  GrPlotSetConvert(grplot,cnvtext,NULL);
  GrPlotSetImageSize(grplot,isize,imagedb); 
  GrPlotSetInclude(grplot,include,NULL);
  GrPlotSetOpenFile(grplot,fileopen);
  GrPlotSetCloseFile(grplot,fileclose);
  GrPlotSetReadFile(grplot,fileread);
  GrPlotSetFileData(grplot,index);

  XMLSetStart(xmldata,GrPlotStart,grplot);
  XMLSetEnd(xmldata,GrPlotEnd,grplot);
  XMLSetText(xmldata,GrPlotText,grplot);

  if (arg<argc) {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    }
  } else fp=stdin;

  while(fgets(lbuf,255,fp) !=NULL) {   
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;

  }
  if (s !=0) {
    fprintf(stderr,"Error parsing document.\n");
    exit(0);
  }

  if (arg<argc) fclose(fp);
  if (fbplot==NULL) exit(0);
  if (fbplot->img==NULL) {
    fprintf(stderr,"Nothing to plot.\n");
    exit(-1);
  }
  img=fbplot->img;

#ifdef _XLIB_
  if (xd !=0) {
    dp=XwinOpenDisplay(display_name,&xdf);
 
    if (dp==NULL) {
      fprintf(stderr,"Could not open display.\n");
      exit(-1);
    }

    if (xdoff==-1) xdoff=(dp->wdt-img->wdt)/2;
    if (ydoff==-1) ydoff=(dp->hgt-img->hgt)/2;


    win=XwinMakeWindow(xdoff,ydoff,img->wdt,img->hgt,0,
                       dp,name,
                       name,argv[0],name,argc,argv,&xdf);
    if (win==NULL) {
      fprintf(stderr,"Could not create window.\n");
      exit(-1);
    }

    XwinFrameBufferWindow(img,win);

    XwinShowWindow(win);

    XwinDisplayEvent(dp,1,&win,1,NULL);

    XwinFreeWindow(win);
    XwinCloseDisplay(dp);
  } else {
    if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
    else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
    else FrameBufferSavePPMX(img,stdout);
  }
  #else 
  if (xmlflg==1) FrameBufferSaveXML(img,stream,stdout);
  else if (ppmflg==1) FrameBufferSavePPM(img,stdout);
  else FrameBufferSavePPMX(img,stdout);
  #endif
  return 0; 
}




