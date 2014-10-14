/* include.c
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
#include <string.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "fontdb.h"
#include "rplotg.h"
#include "grplot.h"
#include "block.h"

/*
 $Log: include.c,v $
 Revision 1.2  2004/04/26 20:59:24  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 03:16:45  barnes
 Initial revision

*/

int GrPlotIncludeUser(char *buf,int sze,void *data) {
  int s=0;
  struct XMLdata *xmldata;
 
  xmldata=(struct XMLdata *)data;
  if (xmldata==NULL) return -1;
  s=XMLDecode(xmldata,buf,sze);
  
  return s;
}

int GrPlotIncludeStart(char *name,int atnum,char **atname,char **atval,
		    char *buf,int sze,void *data) {
 
  struct GrPlotBlock *blk;
  struct GrPlotInclude *ptr;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotInclude *) blk->data;
  if (ptr==NULL) return -1;
 
  if (strcmp(name,blk->name)==0) {
    return 0;
  }

  if (strcmp(name,"name")==0) {
    struct GrPlotString *str;
    str=malloc(sizeof(struct GrPlotString));
    str->inc=ptr->inc;
    str->txt=&ptr->name;
    blk=GrPlotSave(blk->xml,"name",str);
    XMLSetStart(blk->xml,GrPlotStringStart,blk);
    XMLSetText(blk->xml,GrPlotStringText,blk);
    XMLSetEnd(blk->xml,GrPlotStringEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"include")==0) {
    struct GrPlotInclude *inc;
    inc=malloc(sizeof(struct GrPlotInclude));
    inc->inc=ptr->inc;
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


int GrPlotIncludeEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  struct GrPlotBlock *blk;
  struct GrPlotInclude *ptr;
  struct GrPlotIncludeBlock *inc;
  struct XMLdata *xml;

  blk=(struct GrPlotBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct GrPlotInclude *) blk->data;
  if (ptr==NULL) return -1;
  inc=ptr->inc;
  if (inc==NULL) return -1;
  if (strcmp(name,blk->name)==0) {
  
    if (ptr->name==NULL) return -1;
    
    xml=blk->xml;
    GrPlotRestore(blk);

    if (inc->user !=NULL) 
       s=(inc->user)(ptr->name,GrPlotIncludeUser,xml,inc->data);


    free(ptr->name);
    free(ptr);
    return s;
  }
  return 0;
}






