/* convert.c
   ========= */


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
#include <unistd.h>
#include <string.h>
#include "rcgi.h"
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"

/*
 $Log: convert.c,v $
 Revision 1.2  2004/04/27 17:17:40  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/04/03 15:06:52  barnes
 Initial revision

*/

char *XMLToHTML(struct XMLDBbuffer *src) {
  char *symbol[]={"\"","\\","<",">","&","<br>",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;","\n",0};
  struct Entitydata *ent=NULL;
  char *txt=NULL;
  struct XMLDBbuffer *dst=NULL;
  ent=EntityMake();
  EntityAddEntityStringArray(ent,entity);
  EntityAddSymbolStringArray(ent,symbol);
  dst=EntityDecodeBuffer(ent,src);
  txt=getDBstring(dst);
  XMLDBFreeBuffer(dst);
  free(ent);
  return txt;
}

char *XMLToRaw(struct XMLDBbuffer *src) {
  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *ent=NULL;
  char *txt=NULL;
  struct XMLDBbuffer *tmp=NULL;
  struct XMLDBbuffer *dst=NULL;
  ent=EntityMake();
  EntityAddEntityStringArray(ent,entity);
  EntityAddSymbolStringArray(ent,symbol);
  tmp=EntityDecodeBuffer(ent,src);
  dst=EntityDecodeBuffer(ent,tmp);
  txt=getDBstring(dst);
  XMLDBFreeBuffer(tmp);
  XMLDBFreeBuffer(dst);
  free(ent);
  return txt;
}

char *XMLToURL(struct XMLDBbuffer *src) {
  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *ent=NULL;
  char *txt=NULL;
  struct XMLDBbuffer *tmp=NULL;
  struct XMLDBbuffer *dst=NULL;
  ent=EntityMake();
  EntityAddEntityStringArray(ent,entity);
  EntityAddSymbolStringArray(ent,symbol);
  tmp=EntityDecodeBuffer(ent,src);
  dst=EntityDecodeBuffer(ent,tmp);
  txt=CGIMakeURLString(dst->buf,dst->sze);
  XMLDBFreeBuffer(tmp);
  XMLDBFreeBuffer(dst);
  free(ent);
  return txt;
}

struct XMLDBbuffer *RawToXML(char *buf,int sze) {
  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *ent=NULL;
  struct XMLDBbuffer *src=NULL;
  struct XMLDBbuffer *tmp=NULL;
  struct XMLDBbuffer *dst=NULL;
  ent=EntityMake();
  EntityAddEntityStringArray(ent,entity);
  EntityAddSymbolStringArray(ent,symbol);
  src=XMLDBMakeBuffer(sze);
  XMLDBAddBuffer(src,buf,sze);
  tmp=EntityEncodeBuffer(ent,src);
  dst=EntityEncodeBuffer(ent,tmp);
  XMLDBFreeBuffer(src);
  XMLDBFreeBuffer(tmp);
  free(ent);
  return dst;
}


char *RawToHTML(char *buf,int sze) {
  char *symbol[]={"\"","\\","<",">","&","\n",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;","<br>",0};
  char *dtxt=NULL;
  struct Entitydata *ent=NULL;
  struct XMLDBbuffer *src=NULL;
  struct XMLDBbuffer *dst=NULL;
  ent=EntityMake();
  EntityAddEntityStringArray(ent,entity);
  EntityAddSymbolStringArray(ent,symbol);
  src=XMLDBMakeBuffer(sze);
  XMLDBAddBuffer(src,buf,sze);
  dst=EntityEncodeBuffer(ent,src);
  dtxt=getDBstring(dst);
  XMLDBFreeBuffer(src);
  XMLDBFreeBuffer(dst);
  free(ent);
  return dtxt;
}

struct XMLDBbuffer *ApplyModifier(struct XMLDBbuffer *mod,char *name,
                                  struct XMLDBbuffer *txt) {
  struct XMLDBbuffer *dst; 
  struct XMLDBbuffer *nxml;
  char *vraw,*vurl,*vhtml;
  char *nraw,*nurl,*nhtml;
  
  struct Entitydata *ent=NULL;
  ent=EntityMake();

  vraw=XMLToRaw(txt);
  vurl=XMLToURL(txt);
  vhtml=XMLToHTML(txt);
 
  nraw=name;
  nurl=CGIMakeURLString(name,strlen(name));
  nhtml=RawToHTML(name,strlen(name));
  nxml=RawToXML(name,strlen(name));
 
  EntityAddEntityString(ent,"&vraw;");
  EntityAddSymbolString(ent,vraw);
  EntityAddEntityString(ent,"&nraw;");
  EntityAddSymbolString(ent,nraw);

  EntityAddEntityString(ent,"&vurl;");
  EntityAddSymbolString(ent,vurl);
  EntityAddEntityString(ent,"&nurl;");
  EntityAddSymbolString(ent,nurl);

  EntityAddEntityString(ent,"&vhtml;");
  EntityAddSymbolString(ent,vhtml);
  EntityAddEntityString(ent,"&nhtml;");
  EntityAddSymbolString(ent,nhtml);

  EntityAddEntityString(ent,"&vxml;");
  EntityAddSymbol(ent,txt);
  EntityAddEntityString(ent,"&nxml;");
  EntityAddSymbol(ent,nxml);


  dst=EntityDecodeBuffer(ent,mod);
  
  free(vraw);
  free(vurl);
  free(vhtml);
  free(nurl);
  free(nhtml);
  XMLDBFreeBuffer(nxml);
  free(ent);
  return dst;
}


void DoReplace(char **replace,char *name,struct XMLDBbuffer *buf,
               struct XMLDBbuffer *mod,char *sep,int type) {
  struct XMLDBbuffer *tbuf;
  char *txt;
  if (buf==NULL) return;
  if (buf->sze==0) return;

  if (mod==NULL) tbuf=XMLDBCopyBuffer(buf); 
  else tbuf=ApplyModifier(mod,name,buf);

  if (type==3) txt=getDBstring(tbuf);	  
  else if (type==2) txt=XMLToRaw(tbuf);
  else if (type==1) txt=XMLToURL(tbuf);
  else txt=XMLToHTML(tbuf);

  if (*replace==NULL) *replace=txt;
  else {
    char *tmp;
    if (sep !=NULL) {
      tmp=realloc(*replace,strlen(*replace)+strlen(sep)+strlen(txt)+1);
      strcat(tmp,sep);
    } else tmp=realloc(*replace,strlen(*replace)+strlen(txt)+1);
    strcat(tmp,txt);
    free(txt);
    *replace=tmp;
  }
  XMLDBFreeBuffer(tbuf);
}

