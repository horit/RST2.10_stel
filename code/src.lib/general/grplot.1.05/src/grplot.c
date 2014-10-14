/* grplot.c
   ======= 
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
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "fontdb.h"
#include "rplotg.h"
#include "grplot.h"
#include "block.h"

/*
 $Log: grplot.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:16:40  barnes
 Initial revision

*/


int GrPlotStart(char *name,int atnum,char **atname,char **atval,
		  char *buf,int sze,void *data) {
  char *wrap="<>"; 
  struct GrPlotBlock *blk=NULL;
  struct GrPlotData *plt=NULL;
  struct Rplot *rplot=NULL;
  int s=0;

  float wdt=0,hgt=0,xoff=0,yoff=0;
  float lpad=0,rpad=0,tpad=0,bpad=0;
  int xnum=0,ynum=0;
  int i;

  plt=(struct GrPlotData *) data;
  if (plt==NULL) return -1;
  rplot=plt->rplot;
  if (rplot==NULL) return -1;

  if (strcmp(name,"grplot")==0) {
    for (i=0;i<atnum;i++) {
      if ((strcmp(atname[i],"width")==0) && (atval[i] !=NULL))
        wdt=atof(atval[i]);
      if ((strcmp(atname[i],"height")==0) && (atval[i] !=NULL))
        hgt=atof(atval[i]);
      if ((strcmp(atname[i],"xnum")==0) && (atval[i] !=NULL))
        xnum=atoi(atval[i]);
      if ((strcmp(atname[i],"ynum")==0) && (atval[i] !=NULL))
        ynum=atoi(atval[i]);
      if ((strcmp(atname[i],"position")==0) && (atval[i] !=NULL))
         s=GrPlotPointString(atval[i],&xoff,&yoff);
      if (s !=0) break;
      if ((strcmp(atname[i],"pad")==0) && (atval[i] !=NULL))
         s=GrPlotPadString(atval[i],&lpad,&rpad,&bpad,&tpad);
      if (s !=0) break;
    }

    if (s !=0) return s;

    if (wdt==0) return -1;
    if (hgt==0) return -1;
    if (xnum==0) return -1;
    if (ynum==0) return -1;
 
    if (plt->plot !=NULL) {
      PlotFree(plt->plot);
      plt->plot=NULL;
    }    

    plt->plot=PlotMake(wdt,hgt,xnum,ynum,lpad,rpad,bpad,tpad,xoff,yoff);
    if (plt->plot==NULL) return -1;

    PlotSetRplot(plt->plot,plt->rplot);
    PlotSetTextBox(plt->plot,plt->text.box,plt->text.data);
    return 0;
  }
    
  if (strcmp(name,"panel")==0) {
    blk=GrPlotSave(plt->xml,"panel",plt);
    XMLSetStart(plt->xml,GrPlotPanelStart,blk);
    XMLSetEnd(plt->xml,GrPlotPanelEnd,blk);
    return XMLCallStart(plt->xml,name,atnum,atname,atval,buf,sze);
  }  

  if (strcmp(name,"include")==0) {
    struct GrPlotInclude *inc;
    inc=malloc(sizeof(struct GrPlotInclude));
    inc->inc=&plt->inc;
    blk=GrPlotSave(blk->xml,name,inc);
    XMLSetStart(blk->xml,GrPlotIncludeStart,blk);
    XMLSetEnd(blk->xml,GrPlotIncludeEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ignore")==0) {
    blk=GrPlotSave(blk->xml,name,NULL);
    XMLSetStart(blk->xml,GrPlotIgnoreStart,blk);
    XMLSetText(blk->xml,GrPlotIgnoreText,blk);
    XMLSetEnd(blk->xml,GrPlotIgnoreEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }


  if (rplot->text.user !=NULL) {
    s=(rplot->text.user)(wrap,1,rplot->text.data);
    if (s==0) s=(rplot->text.user)(buf,sze,rplot->text.data);
    if (s==0) s=(rplot->text.user)(wrap+1,1,rplot->text.data);
  }
  return s;
 

}


int GrPlotEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  struct GrPlotData *plt;
  struct Rplot *rplot;

  plt=(struct GrPlotData *) data;
  if (plt==NULL) return -1;
  rplot=plt->rplot;
  if (rplot==NULL) return -1;

  if (strcmp(name,"grplot")!=0) {
    char *wrap="<>"; 
    if (rplot->text.user !=NULL) {
      s=(rplot->text.user)(wrap,1,rplot->text.data);
      if (s==0) s=(rplot->text.user)(buf,sze,rplot->text.data);
      if (s==0) s=(rplot->text.user)(wrap+1,1,rplot->text.data);
    }
    return s;
  }

  if (plt->plot !=NULL) PlotFree(plt->plot);
  plt->plot=NULL;
  return s;
}


int GrPlotText(char *buf,int sze,void *data) {
  int s=0;
  struct GrPlotData *plt;
  struct Rplot *rplot;

  plt=(struct GrPlotData *) data;
  if (plt==NULL) return -1;
  rplot=plt->rplot;
  if (rplot==NULL) return -1;
  if (rplot->text.user !=NULL) s=(rplot->text.user)(buf,sze,
                                  rplot->text.data);

  return s;
}


