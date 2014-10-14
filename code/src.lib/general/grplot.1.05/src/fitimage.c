/* fitimage.c
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
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "fontdb.h"
#include "rplotg.h"
#include "grplot.h"
#include "block.h"

/*
 $Log: fitimage.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:13:49  barnes
 Initial revision

*/

int GrPlotFitImageXMLStart(char *name,int atnum,char **atname,char **atval,
		      char *buf,int sze,void *data) {
  struct GrPlotBlock *blk;
  struct FrameBufferXML *ximg;
  struct XMLdata *xml; 
  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;

 
  ximg=(struct FrameBufferXML *) blk->data;
  xml=ximg->xml;
  if (xml->interface.start.user !=NULL) return 
     (xml->interface.start.user)(name,atnum,atname,atval,
                                 buf,sze,xml->interface.start.data);
  return 0; 
}


int GrPlotFitImageXMLText(char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct FrameBufferXML *ximg;
  struct XMLdata *xml;
  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;

  ximg=(struct FrameBufferXML *) blk->data;
  xml=ximg->xml;

  if (xml->interface.text.user !=NULL) return
     (xml->interface.text.user)(buf,sze,xml->interface.text.data);
  return 0;
}

int GrPlotFitImageXMLEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  struct GrPlotBlock *blk;
  struct FrameBufferXML *ximg;
  struct XMLdata *xml;
 
  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;

  ximg=(struct FrameBufferXML *) blk->data;
  xml=ximg->xml;

  if (xml->interface.end.user !=NULL) 
     s=(xml->interface.end.user)(name,buf,sze,xml->interface.end.data);

  if (strcmp(name,blk->name)==0) {
    XMLFree(xml);
    free(blk->data);
    GrPlotRestore(blk);
    return s;
  }
  return s;
}



int GrPlotFitImageStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
  int i;
  struct GrPlotBlock *blk;
  struct GrPlotFitImage *ptr;
  struct GrPlotData *plt;
  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotFitImage *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];
    ptr->mask=0;
    ptr->img=NULL;
    for (i=0;i<atnum;i++) {
      if (strcmp(atname[i],"mask")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&mask);
        ptr->mask=mask;
      }
    }
    return 0;
  }



  if (strcmp(name,"img")==0) {
    struct XMLdata *xml=NULL;
    struct FrameBufferXML *ximg=NULL;
    xml=XMLMake();
    ximg=malloc(sizeof(struct FrameBufferXML));
    ximg->xml=xml;
    ximg->img=&(ptr->img);

    XMLSetStart(xml,FrameBufferXMLStart,ximg);
    XMLSetEnd(xml,FrameBufferXMLEnd,ximg);

    blk=GrPlotSave(blk->xml,"img",ximg);
    XMLSetStart(blk->xml,GrPlotFitImageXMLStart,blk);
    XMLSetText(blk->xml,GrPlotFitImageXMLText,blk);
    XMLSetEnd(blk->xml,GrPlotFitImageXMLEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
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


  return 0;
}


int GrPlotFitImageEnd(char *name,char *buf,int sze,void *data) {

  struct GrPlotBlock *blk;
  struct GrPlotFitImage *ptr;
  struct GrPlotData *plt;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotFitImage *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    struct PlotData *plot;
    plot=plt->plot;
    if (plot==NULL) return -1;

    if (ptr->img==NULL) return -1;

    PlotFitImage(plot,plt->num,ptr->img,ptr->mask);
    FrameBufferFree(ptr->img);
    free(ptr);
    GrPlotRestore(blk);
    return 0;
  }
  return 0;
}

