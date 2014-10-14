/* loadconfig.c
   ============ */


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
#include <unistd.h>
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "submit.h"

/*
 $Log: loadconfig.c,v $
 Revision 1.6  2006/11/18 14:23:50  code
 Fixed bug in setting the submitc command.

 Revision 1.5  2005/03/22 16:41:07  barnes
 Preserved query through system.

 Revision 1.4  2004/06/21 21:48:34  barnes
 Added missing header.

 Revision 1.3  2004/06/02 14:37:40  barnes
 Added post processing command.

 Revision 1.2  2004/04/27 17:25:58  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/04/15 17:24:16  barnes
 Initial revision

*/

int decodetree(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                  void *data) {
  int i;
  struct config *config;
  struct XMLDBelement *e;
 
  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"path")==0) {
      if (config->tree.path !=NULL) free(config->tree.path);
      config->tree.path=getDBstring(e->data);
      if (config->tree.path==NULL) break;
    } else if (strcmp(e->name,"delim")==0) {
      char *d=NULL;
      d=getDBstring(e->data);
      if (d==NULL) break;
      config->tree.delim=d[0];
      free(d);
    }
  }
  if (i<ptr->num) return -1;
  return 0;
}

int decodesection(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,n;
  struct config *config;
  struct XMLDBelement *e;
 
  config=(struct config *)data;
  
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"tag") !=0) continue;

    n=config->display.num;

    if (config->display.tag==NULL) config->display.tag=malloc(sizeof(char *)); 
    else config->display.tag=realloc(config->display.tag,
                                     sizeof(char *)*(n+1)); 
  
    if (config->display.tag==NULL) break;
    config->display.tag[n]=getDBstring(e->data);
    if (config->display.tag[n]==NULL) break;
    config->display.num++;
  }
  if (i<ptr->num) return -1;
  
  return 0;
}


int decodeaction(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                 void *data) {
  int i;
  struct config *config;
  struct XMLDBelement *e;

  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];

    if (strcmp(e->name,"name")==0) {
      if (config->action.name !=NULL) free(config->action.name);
      config->action.name=getDBstring(e->data);
      if (config->action.name==NULL) break;
    } else if (strcmp(e->name,"add")==0) {
      if (config->action.add !=NULL) free(config->action.add);
      config->action.add=getDBstring(e->data);
      if (config->action.add==NULL) break;
    } else if (strcmp(e->name,"change")==0) {
      if (config->action.change !=NULL) free(config->action.change);
      config->action.change=getDBstring(e->data);
      if (config->action.change==NULL) break;
    } else if (strcmp(e->name,"delete")==0) {
      if (config->action.delete !=NULL) free(config->action.delete);
      config->action.delete=getDBstring(e->data);
      if (config->action.delete==NULL) break;
    }
  }
  if (i<ptr->num) return -1;
  return 0;
}





int decodesfname(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                    void *data) {
  int i,j;
  struct config *config;
  struct XMLDBelement *e;
  char zero[1]={0};

  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *entptr;

  entptr=EntityMake();
  if (entptr==NULL) return -1;
  EntityAddEntityStringArray(entptr,entity);
  EntityAddSymbolStringArray(entptr,symbol);

  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"submit")==0) {
      if (config->sfname.submit.buf !=NULL) 
          XMLDBFreeBuffer(config->sfname.submit.buf);

      config->sfname.submit.type=0;
      for (j=0;j<e->atnum;j++) {
	if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) 
        config->sfname.submit.type=1;
      }
      if (config->sfname.submit.type==1) { 
        config->sfname.submit.buf=XMLDBCopyBuffer(e->data);
        XMLDBAddBuffer(config->sfname.submit.buf,zero,1);
      } else config->sfname.submit.buf=EntityDecodeBuffer(entptr,e->data);
      if (config->sfname.submit.buf==NULL) break;
    }
    if (strcmp(e->name,"user.mail")==0) {
      if (config->sfname.usermail.buf !=NULL) 
          XMLDBFreeBuffer(config->sfname.usermail.buf);

      config->sfname.usermail.type=0;
      for (j=0;j<e->atnum;j++) {
	if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) 
        config->sfname.usermail.type=1;
      }
      if (config->sfname.usermail.type==1) { 
        config->sfname.usermail.buf=XMLDBCopyBuffer(e->data);
        XMLDBAddBuffer(config->sfname.usermail.buf,zero,1);
      } else config->sfname.usermail.buf=EntityDecodeBuffer(entptr,e->data);
      if (config->sfname.usermail.buf==NULL) break;
    }
    if (strcmp(e->name,"super.mail")==0) {
      if (config->sfname.supermail.buf !=NULL) 
          XMLDBFreeBuffer(config->sfname.supermail.buf);

      config->sfname.supermail.type=0;
      for (j=0;j<e->atnum;j++) {
	if ((e->atval[j] !=NULL) && (strcmp(e->atval[j],"file")==0)) 
        config->sfname.supermail.type=1;
      }
      if (config->sfname.supermail.type==1) { 
        config->sfname.supermail.buf=XMLDBCopyBuffer(e->data);
        XMLDBAddBuffer(config->sfname.supermail.buf,zero,1);
      } else config->sfname.supermail.buf=EntityDecodeBuffer(entptr,e->data);
      if (config->sfname.supermail.buf==NULL) break;
    }

  }
  free(entptr);
  if (i<ptr->num) return -1;
  return 0;
}

int decodeorder(struct XMLDBtable *ptr,struct XMLDBtable *tree,
	      void *data) {
  int i,s=0,n;
  struct config *config;
  struct order *order;
  struct XMLDBelement *e;

  char *name=NULL;
  int otp=0;
  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];

    if (strcmp(e->name,"element")==0) {
      if (name !=NULL) free(name);
      name=getDBstring(e->data);
      if (name==NULL) break;
     } else if (strcmp(e->name,"type")==0) {
      char *type=NULL;
      type=getDBstring(e->data);
      if (strcmp(type,"tail")==0) otp=0;
      if (strcmp(type,"top")==0) otp=1;
      if (strcmp(type,"alpha")==0) otp=2;
      if (strcmp(type,"name")==0) otp=3;
      if (strcmp(type,"lt")==0) otp=4;
      if (strcmp(type,"gt")==0) otp=5;
      if (strcmp(type,"le")==0) otp=6;
      if (strcmp(type,"ge")==0) otp=7;
      free(type);
     }
  }
  if (i<ptr->num) s=-1;
 
  if ((s==0) && (name !=NULL)) {
    n=config->order.num;
    if (config->order.ptr==NULL)
      config->order.ptr=malloc(sizeof(struct order *));
    else {
      struct order **tmp;
      tmp=realloc(config->order.ptr,sizeof(struct order *)*(n+1));
      if (tmp==NULL) return s=-1;
      else config->order.ptr=tmp;
    }
    if (config->order.ptr==NULL) s=-1;
    if (s==0) {
      order=malloc(sizeof(struct order));
      if (order !=NULL) {
        order->name=name;
        order->type=otp;
        config->order.ptr[n]=order;
        config->order.num++;
      }
    }
  }
 
  if (s !=0) {
    if (name !=NULL) free(name);
    return -1;
  }
  return 0;
}



int decodedata(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,n;
  struct config *config;
  struct XMLDBelement *e;
 
  config=(struct config *)data;
  
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"element") !=0) continue;

    n=config->data.num;

    if (config->data.txt==NULL) config->data.txt=malloc(sizeof(char *)); 
    else config->data.txt=realloc(config->data.txt,
                                     sizeof(char *)*(n+1)); 
  
    if (config->data.txt==NULL) break;
    config->data.txt[n]=getDBstring(e->data);
    if (config->data.txt[n]==NULL) break;
    config->data.num++;
  }
  if (i<ptr->num) return -1;
  
  return 0;
}


int decoderequired(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,n;
  struct config *config;
  struct XMLDBelement *e;
 
  config=(struct config *)data;
  
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"element") !=0) continue;

    n=config->require.num;

    if (config->require.txt==NULL) config->require.txt=malloc(sizeof(char *)); 
    else config->require.txt=realloc(config->require.txt,
                                     sizeof(char *)*(n+1)); 
  
    if (config->require.txt==NULL) break;
    config->require.txt[n]=getDBstring(e->data);
    if (config->require.txt[n]==NULL) break;
    config->require.num++;
  }
  if (i<ptr->num) return -1;
  
  return 0;
}


int decodecompare(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,n;
  struct config *config;
  struct XMLDBelement *e;
 
  config=(struct config *)data;
  
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"element") !=0) continue;

    n=config->compare.num;

    if (config->compare.txt==NULL) config->compare.txt=malloc(sizeof(char *)); 
    else config->compare.txt=realloc(config->compare.txt,
                                     sizeof(char *)*(n+1)); 
  
    if (config->compare.txt==NULL) break;
    config->compare.txt[n]=getDBstring(e->data);
    if (config->compare.txt[n]==NULL) break;
    config->compare.num++;
  }
  if (i<ptr->num) return -1;
  
  return 0;
}

int decodepost(struct XMLDBtable *ptr,struct XMLDBtable *tree,
	      void *data) {
  int i,s=0,n;
  struct config *config;
  struct post *post;
  struct XMLDBelement *e;

  char *name=NULL;
  char *tag=NULL;
  int ptp=0;
  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];

    if (strcmp(e->name,"tag")==0) {
      if (tag !=NULL) free(tag);
      tag=getDBstring(e->data);
      if (tag==NULL) break;
    } else if (strcmp(e->name,"name")==0) {
      if (name !=NULL) free(name);
      name=getDBstring(e->data);
      if (name==NULL) break;
     } else if (strcmp(e->name,"type")==0) {
      char *type=NULL;
      type=getDBstring(e->data);
      if (strcmp(type,"checkbox")==0) ptp=0;
      if (strcmp(type,"select")==0) ptp=1;
      free(type);
     }
  }
  if (i<ptr->num) s=-1;


  if ((s==0) && (tag !=NULL)) {
    n=config->post.num;
    if (config->post.ptr==NULL)
      config->post.ptr=malloc(sizeof(struct post *));
    else {
      struct post **tmp;
      tmp=realloc(config->post.ptr,sizeof(struct post *)*(n+1));
      if (tmp==NULL) return s=-1;
      else config->post.ptr=tmp;
    }
    if (config->post.ptr==NULL) s=-1;
    if (s==0) {
      post=malloc(sizeof(struct post));
      if (post !=NULL) {
        post->tag=tag;
        post->name=name;
        post->type=ptp;
        config->post.ptr[n]=post;
        config->post.num++;
      }
    }
  }

  if (s !=0) {
    if (tag !=NULL) free(tag);
    if (name !=NULL) free(name);
    return -1;
  }
  return 0;
}




int decodemap(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                    void *data) {
  int i,s=0,n;
  struct config *config;
  struct map *map;
  struct XMLDBelement *e;
 
  char *search=NULL;
  char *replace=NULL;
  struct XMLDBbuffer *defval=NULL;
  struct XMLDBbuffer *sepval=NULL;   
  struct XMLDBbuffer *modval=NULL;
  int mtp=0;  
  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
 
    if (strcmp(e->name,"search")==0) {
      if (search !=NULL) free(search);
      search=getDBstring(e->data);
      if (search==NULL) break;
    } else if (strcmp(e->name,"type")==0) {
       char *type=NULL;
       type=getDBstring(e->data);
       if (strcmp(type,"html")==0) mtp=0;
       if (strcmp(type,"url")==0) mtp=1;
       if (strcmp(type,"raw")==0) mtp=2;
       free(type);
    } else if (strcmp(e->name,"replace")==0) {
      if (replace !=NULL) free(replace);
      replace=getDBstring(e->data);
      if (replace==NULL) break;
    } else if (strcmp(e->name,"default")==0) defval=e->data;
    else if (strcmp(e->name,"separator")==0) sepval=e->data;
    else if (strcmp(e->name,"modifier")==0) modval=e->data;

  }
  if (i<ptr->num) s=-1;  


  if ((s==0) && (search !=NULL)) {
    n=config->map.num; 
    if (config->map.ptr==NULL) 
      config->map.ptr=malloc(sizeof(struct map *));
    else {
      struct map **tmp;
      tmp=realloc(config->map.ptr,sizeof(struct map *)*(n+1));
      if (tmp==NULL) return s=-1;
      else config->map.ptr=tmp;
    } 
    if (config->map.ptr==NULL) s=-1;
    if (s==0) {
      map=malloc(sizeof(struct map));
      if (map !=NULL) {
	map->search=search;
        map->replace=replace;
        map->type=mtp;
        map->def=XMLDBCopyBuffer(defval);  
        map->sep=XMLDBCopyBuffer(sepval);
        map->mod=XMLDBCopyBuffer(modval);
        config->map.ptr[n]=map;
        config->map.num++;
      }
    }
  }
 
  if (s !=0) {
    if (search !=NULL) free(search);
    if (replace !=NULL) free(replace);
    return -1;
  }

  return 0;
}


int decodesuper(struct XMLDBtable *ptr,struct XMLDBtable *tree,
                   void *data) {
  int i,n;
  struct config *config;
  struct XMLDBelement *e;
 
  config=(struct config *)data;
  
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"mail") !=0) continue;

    n=config->super.num;

    if (config->super.mail==NULL) config->super.mail=malloc(sizeof(char *)); 
    else config->super.mail=realloc(config->super.mail,
                                     sizeof(char *)*(n+1)); 
  
    if (config->super.mail==NULL) break;
    config->super.mail[n]=getDBstring(e->data);
    if (config->super.mail[n]==NULL) break;
    config->super.num++;
  }
  if (i<ptr->num) return -1;
  
  return 0;
}

int decodeconfig(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int i;
  struct config *config;
  struct XMLDBelement *e;
  config=(struct config *)data;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"ok")==0) {
      if (config->ok !=NULL) free(config->ok);
      config->ok=getDBstring(e->data);
    } else  if (strcmp(e->name,"fail")==0) {
      if (config->fail !=NULL) free(config->fail);
      config->fail=getDBstring(e->data);
    } else  if (strcmp(e->name,"conflict")==0) {
      if (config->conflict !=NULL) free(config->conflict);
      config->conflict=getDBstring(e->data);
    } else  if (strcmp(e->name,"missing")==0) {
      if (config->missing !=NULL) free(config->missing);
      config->missing=getDBstring(e->data);
    } else  if (strcmp(e->name,"path")==0) {
      if (config->path !=NULL) free(config->path);
      config->path=getDBstring(e->data);
    } else  if (strcmp(e->name,"store")==0) {
      if (config->store !=NULL) free(config->store);
      config->store=getDBstring(e->data);
    } else  if (strcmp(e->name,"code")==0) {
      if (config->code !=NULL) free(config->code);
      config->code=getDBstring(e->data);
    } else  if (strcmp(e->name,"query")==0) {
      if (config->query !=NULL) free(config->query);
      config->query=getDBstring(e->data);
    } else  if (strcmp(e->name,"mail")==0) {
      if (config->mail !=NULL) free(config->mail);
      config->mail=getDBstring(e->data);
    } else  if (strcmp(e->name,"created")==0) {
      if (config->created !=NULL) free(config->created);
      config->created=getDBstring(e->data);
    } else  if (strcmp(e->name,"modified")==0) {
      if (config->modified !=NULL) free(config->modified);
      config->modified=getDBstring(e->data);
    } else  if (strcmp(e->name,"mailc")==0) {
      if (config->mailc !=NULL) free(config->mailc);
      config->mailc=getDBstring(e->data);
    } else  if (strcmp(e->name,"submitc")==0) {
      if (config->submitc !=NULL) free(config->submitc);
      config->submitc=getDBstring(e->data);
    } else  if (strcmp(e->name,"top")==0) {
      if (config->top !=NULL) free(config->top);
      config->top=getDBstring(e->data);
    } else  if (strcmp(e->name,"display")==0) {
      if (config->display.name !=NULL) free(config->display.name);
      config->display.name=getDBstring(e->data);
    } else if (strcmp(e->name,"section")==0) {
      XMLdocGetConfigSection(e->data,"section","section",
                             decodesection,config);
    } else if (strcmp(e->name,"action")==0) {
      XMLdocGetConfigSection(e->data,"action","action",decodeaction,config);
    } else if (strcmp(e->name,"tree")==0) {
      XMLdocGetConfigSection(e->data,"tree","tree",decodetree,config);
    } else if (strcmp(e->name,"post")==0) {
      XMLdocGetConfigSection(e->data,"post","post/entry",decodepost,config);
    } else if (strcmp(e->name,"order")==0) {
      XMLdocGetConfigSection(e->data,"order","order/entry",decodeorder,config);
    } else if (strcmp(e->name,"data")==0) {
      XMLdocGetConfigSection(e->data,"data","data",decodedata,
                             config);
    } else if (strcmp(e->name,"compare")==0) {
      XMLdocGetConfigSection(e->data,"compare","compare",decodecompare,
                             config);
    } else if (strcmp(e->name,"required")==0) {
      XMLdocGetConfigSection(e->data,"required","required",decoderequired,
                             config);
    } else if (strcmp(e->name,"compare")==0) {
      XMLdocGetConfigSection(e->data,"compare","compare",decodecompare,
                             config);
    } else if (strcmp(e->name,"super")==0) {
      XMLdocGetConfigSection(e->data,"super","super",decodesuper,
                             config);
    } else if (strcmp(e->name,"script")==0) {
      XMLdocGetConfigSection(e->data,"script","script",decodesfname,config);
    } else if (strcmp(e->name,"map")==0) {
      XMLdocGetConfigSection(e->data,"map","map/entry",decodemap,config);
    } 
  }
  return 0;
}

int loadconfig(FILE *fp,struct config *ptr) {
  int s=0;
  char *epath="config";
  char edelim='/';

  struct XMLdata *xmldata;
  struct XMLDBdata *xmldbdata;
  struct XMLDBtree *tree;

  char lbuf[255];

  xmldata=XMLMake();
  xmldbdata=XMLDBMake(xmldata);
  tree=XMLDBMakeTree();

  XMLDBBuildTree(epath,edelim,tree);   
  XMLDBSetTree(xmldbdata,tree);

  XMLDBSetText(xmldbdata,decodeconfig,ptr);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  
  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
    if (s !=0) break;
  
  }
  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);

  return 0;
}



