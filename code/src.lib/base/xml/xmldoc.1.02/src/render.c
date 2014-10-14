/* render.c
   ======== 
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
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"

#include "getstring.h"
#include "rxmldoc.h"

/*
 $Log: render.c,v $
 Revision 1.2  2004/04/26 20:29:10  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/10/21 13:58:05  barnes
 Initial revision

*/

int XMLdocRenderFile(struct XMLdocfile *ptr) {
  FILE *fp;
  char lbuf[256];
  struct Entitydata *ent;
 
  char *filename=NULL;
  int s=0;
  if (ptr==NULL) return 0;
  if (ptr->fname==NULL) return 0;
  if (ptr->text==NULL) return 0;
  if (ptr->path !=NULL) {
    filename=malloc(strlen(ptr->path)+strlen(ptr->fname)+2);
    sprintf(filename,"%s/%s",ptr->path,ptr->fname);
  } else {
    filename=malloc(strlen(ptr->fname)+1);
    strcpy(filename,ptr->fname);
  }
    
  fp=fopen(filename,"r");
  free(filename);
  if (fp==NULL) return 0;
 
  ent=EntityMake();
  if (ent==NULL) {  
    fclose(fp);
    return -1;
  }

  s=EntitySetText(ent,ptr->text,ptr->data);

  if ((s==0) && (ptr->type !=0)) { /* raw file so must encode entities */
    char *symbol[]={"\"","\\","<",">","&",0};
    char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
    s=EntityAddEntityStringArray(ent,entity);
    if (s==0) s=EntityAddSymbolStringArray(ent,symbol);
  }

  while ((s==0) && (fgets(lbuf,255,fp) !=NULL)) {
    s=EntityEncode(lbuf,strlen(lbuf),ent);
    if (s !=0) break;
  }
  fclose(fp);
  return s;
}

int XMLdocFileTrap(char *buf,int sze,void *data) {
  struct XMLdocfile *ptr;
  
  if (strncmp(buf,"&file;",sze) !=0) return 0; /* ignore this one */
  ptr=(struct XMLdocfile *) data; 
  if (ptr !=NULL) return XMLdocRenderFile(ptr);
  return 0;
}

int XMLdocRenderElement(struct TagDBdata *tag,struct TagDBtable *tagdb,
                 struct XMLdocelement *x,struct XMLDBelement *e) {
  int i,j,s=0;
  struct XMLdocmod *m=NULL;
  struct XMLDBbuffer *action=NULL;
  struct XMLDBbuffer *data=NULL;
  char *path=NULL;
  int xtp=0;
  int atp=0;

  path=x->path;
  action=x->action;
  xtp=x->type.xml;
  atp=x->type.action;
 
  TagDBSetTable(tag,tagdb); 
  if (x->tagdb !=NULL) TagDBSetTable(tag,x->tagdb);

  if (e !=NULL) data=e->data;
  else {
    data=x->defval;
    xtp=x->type.defval;
  }

  if (e !=NULL) {
    /* decide if we need a modifier */
    for (i=0;i<e->atnum;i++) {
      for (j=0;j<x->mod.num;j++) {
        m=x->mod.ptr[j];
        if (strcmp(e->atval[i],m->name)==0) break;
      }
      if (j<x->mod.num) {
	/* we have a matching attribute */
        if (m->action !=NULL) action=m->action;
        if (m->tagdb !=NULL) TagDBSetTable(tag,m->tagdb);
        if (m->path !=NULL) path=m->path;
        atp=m->type.action;
        xtp=m->type.xml;
      }
    }
  }

  /* we are now in a position to render this element */
   
  if (action==NULL) { /* no action - render as text */
    if (xtp==0) { /* data is text */
      s=TagDBDecode(data->buf,data->sze,tag);
      s=TagDBDecode(NULL,0,tag);
    } else {
      struct XMLdocfile file;
      file.fname=NULL;
      if (data !=NULL) file.fname=getDBstring(data);
      file.path=path;
      file.text=TagDBDecode;
      file.data=tag;
      file.type=xtp-1;
      s= XMLdocRenderFile(&file);
      if (file.fname !=NULL) free(file.fname);
    }   
  } else {

    /* use an entity decoder to handle the action */

    struct Entitydata *ent;
    ent=EntityMake();
    if (ent==NULL) return -1;
   
    s=EntityAddEntityString(ent,"&lt;");
    s=EntityAddEntityString(ent,"&gt;");
    s=EntityAddSymbolString(ent,"<");
    s=EntityAddSymbolString(ent,">");
   
    if (xtp==0) { 
       s=EntityAddEntityString(ent,"&xml;");
       if (s==0) s=EntityAddSymbol(ent,data);
    } else {
      struct XMLdocfile file;
      file.fname=NULL;
      if (data !=NULL) file.fname=getDBstring(data);
      file.path=path;
      file.text=TagDBDecode;
      file.data=tag;
      file.type=xtp-1;

      s=EntityAddEntityString(ent,"&filename;");
      if (s==0) s=EntityAddEntityString(ent,"&file;");
      if (s==0) s=EntityAddSymbol(ent,e->data); 
      if (s==0) s=EntitySetDecodeTrap(ent,XMLdocFileTrap,&file);
      
    }     

    EntitySetText(ent,TagDBDecode,tag);
    if (atp==0) { 
      s=EntityDecode(action->buf,action->sze,ent);
      if (s==0) s=EntityDecode(NULL,0,ent);
    } else {
      FILE *fp;
      char *afname=NULL;
      char lbuf[256];
      afname=getDBstring(action); 
      if (afname !=NULL) {	
        fp=fopen(afname,"r");
        if (fp !=NULL) {
          while ((s==0) && (fgets(lbuf,255,fp) !=NULL)) {
             s=EntityDecode(lbuf,strlen(lbuf),ent);
             if (s !=0) break;
          }
          if (s==0) s=EntityDecode(NULL,0,ent);
          fclose(fp);
        }
        free(afname);
      }  
    }
    EntityFree(ent);
  }
  return 0;
}


int XMLdocRender(struct XMLDBtable *ptr,struct XMLdocdata *xmldoc,int index) {
  struct XMLDBelement *e;
  struct XMLdocelement *x;
  int j;
  int dflg=1;
 
  x=xmldoc->xml.ptr[index];
  dflg=1;
  if (ptr !=NULL) for (j=0;j<ptr->num;j++) {
    e=ptr->element[j];
    if (strcmp(e->name,x->name)==0) { 
      /* decode and output the element */
      XMLdocRenderElement(xmldoc->tag,xmldoc->tagdb,x,e);
      dflg=0;
    }
  }
  if (dflg==1) XMLdocRenderElement(xmldoc->tag,xmldoc->tagdb,x,NULL);
  return 0;
}

int XMLdocIncludeText(struct XMLdocdata *xmldoc,char *buf,int sze) {
  struct Entitydata *ent;
  ent=xmldoc->ent;
  return (ent->text.user)(buf,sze,ent->text.data);
}



