/* polygon.c
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
#include "block.h"
#include "rplotin.h"


/*
 $Log: polygon.c,v $
 Revision 1.2  2004/04/26 20:11:23  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/

int RplotInPolyPointStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInPolyPoint *ptr;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInPolyPoint *) blk->data;
  if (ptr==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    ptr->state=0;
    return 0;
  }

  if (strcmp(name,"x")==0) {
    struct RplotInFloat *flt;
    flt=malloc(sizeof(struct RplotInFloat));
    flt->inc=ptr->inc;
    flt->val=ptr->x;
    ptr->state=ptr->state | 0x01;
    blk=RplotInSave(blk->xml,name,flt);
    XMLSetStart(blk->xml,RplotInFloatStart,blk);
    XMLSetText(blk->xml,RplotInFloatText,blk);
    XMLSetEnd(blk->xml,RplotInFloatEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }
  if (strcmp(name,"y")==0) {
    struct RplotInFloat *flt;
    flt=malloc(sizeof(struct RplotInFloat));
    flt->inc=ptr->inc;
    flt->val=ptr->y;
    ptr->state=ptr->state | 0x02;
    blk=RplotInSave(blk->xml,name,flt);
    XMLSetStart(blk->xml,RplotInFloatStart,blk);
    XMLSetText(blk->xml,RplotInFloatText,blk);
    XMLSetEnd(blk->xml,RplotInFloatEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }
  if (strcmp(name,"t")==0) {
    struct RplotInInt *it;
    it=malloc(sizeof(struct RplotInInt));
    it->inc=ptr->inc;
    it->val=ptr->t;
    ptr->state=ptr->state | 0x04;
    blk=RplotInSave(blk->xml,name,it);
    XMLSetStart(blk->xml,RplotInIntStart,blk);
    XMLSetText(blk->xml,RplotInIntText,blk);
    XMLSetEnd(blk->xml,RplotInIntEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"include")==0) {
    struct RplotInInclude *inc;
    inc=malloc(sizeof(struct RplotInInclude));
    inc->inc=ptr->inc;
    blk=RplotInSave(blk->xml,name,inc);
    XMLSetStart(blk->xml,RplotInIncludeStart,blk);
    XMLSetEnd(blk->xml,RplotInIncludeEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ignore")==0) {
    blk=RplotInSave(blk->xml,name,NULL);
    XMLSetStart(blk->xml,RplotInIgnoreStart,blk);
    XMLSetText(blk->xml,RplotInIgnoreText,blk);
    XMLSetEnd(blk->xml,RplotInIgnoreEnd,blk);
    return 0;
  }

  return 0;
}

int RplotInPolyPointEnd(char *name,char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInPolyPoint *ptr;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInPolyPoint *) blk->data;
  if (ptr==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    if (ptr->state !=0x07) return -1;
    free(ptr);
    RplotInRestore(blk);
    return 0;
  }
  return 0;
}



int RplotInPolygonStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInPolygon *ptr;
  struct RplotInPlot *plt;


  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInPolygon *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];
    int i,s=0;
    int dflg=0;
    ptr->color=0;
    ptr->mask=0;
    ptr->fill=0;
    ptr->matrix=NULL;
    ptr->x=0;
    ptr->y=0;
    ptr->num=0;
    ptr->px=NULL;
    ptr->py=NULL;
    ptr->t=NULL;   

 
    for (i=0;i<atnum;i++) {
      if (strcmp(atname[i],"color")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&ptr->color);
      } else if (strcmp(atname[i],"mask")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&mask);
        ptr->mask=mask;
      } else if (strcmp(atname[i],"width")==0) ptr->width=atof(atval[i]);
      else if (strcmp(atname[i],"dash")==0) {
        dflg=1;
        if (plt->dash.user !=NULL) s=(plt->dash.user)(atval[i],plt->dash.data);
        if (s !=0) return -1;
      } else if (strcmp(atname[i],"fill")==0) ptr->fill=1;
      else if (strcmp(atname[i],"matrix")==0)
        ptr->matrix=RplotInMatrixString(atval[i]);
      else if (strcmp(atname[i],"position")==0)
        RplotInPointString(atval[i],&ptr->x,&ptr->y);
    }
    if ((dflg==0) && (plt->dash.user !=NULL)) 
       s=(plt->dash.user)(NULL,plt->dash.data);
    return s;
  }

  if (strcmp(name,"pp")==0) {
    int n;
    struct RplotInPolyPoint *pnt;

    n=ptr->num;
    if (ptr->px==NULL) ptr->px=malloc(sizeof(float));
    else {
      float *tmp;
      tmp=realloc(ptr->px,sizeof(float)*(n+1));
      if (tmp==NULL) return -1;
      ptr->px=tmp;

    }
    if (ptr->py==NULL) ptr->py=malloc(sizeof(float));
    else {
      float *tmp;
      tmp=realloc(ptr->py,sizeof(float)*(n+1));
      if (tmp==NULL) return -1;
      ptr->py=tmp;
    }
    if (ptr->t==NULL) ptr->t=malloc(sizeof(int));
    else {
      int *tmp;
      tmp=realloc(ptr->t,sizeof(int)*(n+1));
      if (tmp==NULL) return -1;
      ptr->t=tmp;
    }
    pnt=malloc(sizeof(struct RplotInPolyPoint));
    pnt->inc=&plt->inc;
    pnt->x=&(ptr->px[n]);
    pnt->y=&(ptr->py[n]);
    pnt->t=&(ptr->t[n]);
    ptr->num++;
    blk=RplotInSave(blk->xml,name,pnt);
    XMLSetStart(blk->xml,RplotInPolyPointStart,blk);
    XMLSetEnd(blk->xml,RplotInPolyPointEnd,blk);
    XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
    return 0;
  }

  if (strcmp(name,"include")==0) {
    struct RplotInInclude *inc;
    inc=malloc(sizeof(struct RplotInInclude));
    inc->inc=&plt->inc;
    blk=RplotInSave(blk->xml,name,inc);
    XMLSetStart(blk->xml,RplotInIncludeStart,blk);
    XMLSetEnd(blk->xml,RplotInIncludeEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ignore")==0) {
    blk=RplotInSave(blk->xml,name,NULL);
    XMLSetStart(blk->xml,RplotInIgnoreStart,blk);
    XMLSetText(blk->xml,RplotInIgnoreText,blk);
    XMLSetEnd(blk->xml,RplotInIgnoreEnd,blk);
    return 0;
  }

  return 0;
}

int RplotInPolygonEnd(char *name,char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInPolygon *ptr;
  struct RplotInPlot *plt;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInPolygon *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
 
  if (strcmp(name,blk->name)==0) {
    int s=0;
    struct RplotInMatrix *matrix=NULL;
    
    if (ptr->num==0) return -1;
    matrix=ptr->matrix;

    if (plt->matrix !=NULL) {
      RplotInMatrixTransform(plt->matrix,ptr->x,ptr->y,&ptr->x,&ptr->y);
      if (matrix !=NULL) RplotInMatrixMultiply(plt->matrix,matrix,matrix);
      else matrix=plt->matrix;
    }
    ptr->x+=plt->x;
    ptr->y+=plt->y;
 
    if (plt->polygon.user !=NULL) s=(plt->polygon.user)(matrix,ptr->x,ptr->y,
				   ptr->num,ptr->px,ptr->py,ptr->t,
                                   ptr->fill,ptr->color,ptr->mask,
			           ptr->width,plt->polygon.data);
    

    if (ptr->matrix !=NULL) free(ptr->matrix);
    if (ptr->px !=NULL) free(ptr->px);
    if (ptr->py !=NULL) free(ptr->py);
    if (ptr->t !=NULL) free(ptr->t);

    free(ptr);
    RplotInRestore(blk);
    return s;
  }
  return 0;
}

