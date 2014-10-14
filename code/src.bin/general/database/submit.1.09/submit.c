/* submit.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include "rcgi.h"
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "submit.h"
#include "loadconfig.h"
#include "convert.h"
#include "post.h"
#include "ssi.h"
#include "findorder.h"

/*
 $Log: submit.c,v $
 Revision 1.7  2007/03/26 16:45:58  code
 Fixed sign of char pointer on Darwin.

 Revision 1.6  2005/03/22 16:41:07  barnes
 Preserved query through system.

 Revision 1.5  2005/03/22 15:07:30  barnes
 Fixed the modifier syntax to agree with show.cgi.

 Revision 1.4  2004/06/02 14:37:40  barnes
 Added post processing command.

 Revision 1.3  2004/04/28 15:40:52  barnes
 Changed the temporary filename to make it more portable.

 Revision 1.2  2004/04/27 17:25:58  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/05/01 14:09:27  barnes
 Initial revision

*/

struct ssidata ssidata;
struct config config;
struct postp postp;

struct XMLDBtable *table=NULL;

struct compare {
  char *code;
  char *value;
  int num;
  char **txt;
  struct XMLDBtable *table;
  int flg;
};

struct store {
  FILE *fp;
  int action;
  char *code;
  char *value;
  char *created;
  char *modified;
  char *date;
  struct {
    int num;
    char **txt;
  } data;
  struct {
    int num;
    struct order **ptr;
  } order;
  struct XMLDBtable *table;
  int flg;
};

struct data {
  char *code;
  char *value;
  struct XMLDBtable *table;
  int flg;
};

struct data data;
struct store store;
struct compare compare;

unsigned int crctable[256];


void execute(char *command,int action,char *path,char *code,char *fname) {
  char cmd[4096];
  char mode[64];

  if (action==0) strcpy(mode,"add");
  if (action==1) strcpy(mode,"change");
  if (action==2) strcpy(mode,"delete");

  if (path !=NULL) sprintf(cmd,"%s -path %s %s %s %s",command,
                       path,fname,code,mode);
  else sprintf(cmd,"%s %s %s %s",command,
                    fname,code,mode);

  system(cmd);
}

void crc_init() {
  int i,j;
  unsigned int c=0;
  unsigned int crcpoly=0xedb88320L;
  for (i=0;i<256;i++) {
    c=(unsigned int) i;
    for (j=0;j<8;j++) {
      if ((c & 1) !=0) c=crcpoly ^ (c>>1);
      else c=c>>1;
    }
    crctable[i]=c;
  }
}


int mvfile(char *oldpath,char *newpath) {
  char buffer[4096];
  int status;
  int ifileno;
  int ofileno;
  int bytes;
  struct flock wflock;

  wflock.l_type=F_WRLCK;
  wflock.l_whence=SEEK_SET;
  wflock.l_start=0;
  wflock.l_len=0;

  ofileno=open(newpath,
               O_WRONLY | O_CREAT | O_TRUNC,S_IRWXU | S_IRGRP | S_IROTH);
  status=fcntl(ofileno,F_SETLKW,&wflock);

  if (status==-1) {
    close(ofileno);
    return -1;
  }

  ifileno=open(oldpath,O_RDONLY,NULL);
  
  while ((bytes=read(ifileno,buffer,4096)) !=0) {
    write(ofileno,buffer,bytes);
  }
  close(ifileno);
  close(ofileno);
  remove(oldpath);
  return 0;
}


int write_entry(FILE *fp,int action,char *tag,int num,char **name,
                struct XMLDBtable *table,char *code,char *value,
                char *created,char *modified,char *date) {
  
  struct XMLDBelement *e;
  int i,j,s=0;
  s=fprintf(fp,"<%s>\n",tag);
  if (s !=-1) s=fprintf(fp,"<%s>%s</%s>\n",code,value,code);

  if ((action==0) && (created !=NULL) && (s !=-1)) 
    s=fprintf(fp,"<%s>%s</%s>\n",created,date,created);
  if ((action !=0)  && (created !=NULL) && (s !=-1)) {
    for (j=0;j<table->num;j++) {
      e=table->element[j];
      if (strcmp("created",e->name) !=0) continue;
      if (s !=-1) s=fprintf(fp,"<%s>",e->name);
      if (s !=-1) s=fwrite((e->data)->buf,(e->data)->sze,1,fp);
      if (s !=-1) s=fprintf(fp,"</%s>\n",e->name);
    }
  }
  if ((modified !=NULL) && (s !=-1)) 
    s=fprintf(fp,"<%s>%s</%s>\n",modified,date,modified);
  for (i=0;i<num;i++) {
    for (j=0;j<table->num;j++) {
      e=table->element[j];
      if (strcmp(name[i],e->name) !=0) continue;
      if (s !=-1) s=fprintf(fp,"<%s>",e->name);
      if (s !=-1) s=fwrite((e->data)->buf,(e->data)->sze,1,fp);
      if (s !=-1) s=fprintf(fp,"</%s>\n",e->name);
      if (s==-1) break;
    }
    if (s==-1) break;
  }
  if (s !=-1) s=fprintf(fp,"</%s>\n",tag);
  if (s==-1) return -1;
  return 0;
}

int stream_output(char *buf,int sze,void *data) {
  /* Output to a stream */
  FILE *outp;
  outp=(FILE *) data;
  fwrite(buf,sze,1,outp);
  return 0;
}

int text(struct CGIarg*ptr,char *buf,int sze,void *data) {
  return 0;
}

int postproc(char *buf,int sze,char *tag,void *data) {
  struct ScriptData *pscript;
  struct postp *ptr;
  pscript=(struct ScriptData *) data;
  ptr=(struct postp *) pscript->text.data;
  ptr->tag=tag;
  return ScriptDecode(pscript,buf,sze);
}

int decode_store(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {
  int s=0;
  struct store *st;
  FILE *fp;
  char *tag;
  struct XMLDBelement *e;
  int i;
  st=(struct store *) data;
  fp=st->fp;
  
  e=tree->element[tree->num-1];
  tag=e->name;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,st->code)==0) {
      char *tmp;
      tmp=getDBstring(e->data);
      if (strcmp(tmp,st->value)==0) {
        free(tmp);
        break;
      } 
      free(tmp);
    }
  }

  if ((st->flg==0) && (st->action !=2) && 
      (findorder(st->order.num,st->order.ptr,st->table,ptr) !=0)) {
     s=write_entry(fp,st->action,tag,st->data.num,st->data.txt,
		 st->table,st->code,st->value,
                 st->created,st->modified,st->date);
     if (s !=0) return -1;
     st->flg=1;
  }

  if ((i<ptr->num) && (st->action !=0)) return 0;

  s=fprintf(fp,"<%s>\n",tag);
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (s !=-1) s=fprintf(fp,"<%s>",e->name);
    if (s !=-1) s=fwrite((e->data)->buf,(e->data)->sze,1,fp);
    if (s !=-1) s=fprintf(fp,"</%s>\n",e->name);
  }
  if (s !=-1) fprintf(fp,"</%s>\n",tag);  
  if (s==-1) return -1;
  return 0;
}

int decode_compare(struct XMLDBtable *ptr,
                   struct XMLDBtable *tree,void *data) {
  int i,j,k;
  char *x,*y;
  struct compare *c;
  struct XMLDBelement *e;
  struct XMLDBelement *d;
  char *tmp;

  c=(struct compare *) data;
  if (c->code==NULL) return 0;

  if (c->value !=NULL) {
    for (i=0;i<ptr->num;i++) {
      e=ptr->element[i];
      if (strcmp(e->name,c->code)==0) {
        tmp=getDBstring(e->data);
        if (strcmp(tmp,c->value)==0) {
          free(tmp);
          break;
        } 
        free(tmp);
      }
    }
    if (i<ptr->num) return 0;
  }
  for (i=0;i<c->num;i++) {
    for (j=0;j<ptr->num;j++) {
      e=ptr->element[j];
      if (strcmp(e->name,c->txt[i]) !=0) continue;
      x=getDBstring(e->data);
      for (k=0;k<(c->table)->num;k++) {
        d=(c->table)->element[k];
        if (strcmp(d->name,c->txt[i]) !=0) continue;
        y=getDBstring(d->data);
        if ((x !=NULL) && (y !=NULL) && (strcmp(x,y)==0)) {
	  free(y);
          break;
	}        
        free(y);
      }
      free(x);
      if (k<(c->table)->num) break;
    }
    if (j<ptr->num) break;
  }
  if (i<c->num) {
    c->flg=1;
    return 1;
  }
  return 0;
}

int decode(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {
  int i,j;
  struct data *d;
  struct XMLDBelement *e;
  struct XMLDBelement *c;
  d=(struct data *)data;
  if (d->code==NULL) return 0;

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,d->code)==0) {
      char *tmp;
      tmp=getDBstring(e->data);
      if (strcmp(tmp,d->value)==0) {
        free(tmp);
        break;
      } 
      free(tmp);
    }
  }

  if (i<ptr->num) {
    for (j=0;j<ptr->num;j++) {
      e=ptr->element[j];
      if (strcmp(e->name,d->code)==0) continue;
      c=XMLDBMakeElement(e->name,0,NULL,NULL,100);
      XMLDBFreeBuffer(c->data);
      c->data=XMLDBCopyBuffer(e->data);
      XMLDBAddTable(d->table,c);
    }
    d->flg=1;
    return 1;
  }
  return 0;
}


char *make_code(struct config *cfg,struct XMLDBtable *table,char *date) {
  char *txt=NULL;
  int i,j;
  struct XMLDBelement *e;
  unsigned int crc=0xffffffffL;

  txt=malloc(32);
  sprintf(txt,"%x",getpid());
  
  for (i=0;i<strlen(txt);i++) 
    crc=crctable[(crc ^ txt[i]) & 0xff] ^ (crc>>8); 
  
  for (i=0;i<strlen(date);i++) 
    crc=crctable[(crc ^ date[i]) & 0xff] ^ (crc>>8);
  
      
  for (i=0;i<table->num;i++) { 
    e=table->element[i];
    for (j=0;j<cfg->data.num;j++) if (strcmp(e->name,cfg->data.txt[j])==0) 
      break;
    if  (j==cfg->data.num) continue;
    for (j=0;j<strlen(e->name);j++) 
      crc=crctable[(crc ^ e->name[j]) & 0xff] ^ (crc>>8);
    for (j=0;j<(e->data)->sze;j++) 
      crc=crctable[(crc ^ (e->data)->buf[j]) & 0xff] ^ (crc>>8);
  }  
      
  txt=malloc(20);
  sprintf(txt,"%.8x",crc);
  return txt;
}

int main(int argc,char *argv[]) {
  int i,j,s;
  FILE *fp;
  char lbuf[256];
  char *cfgfile=NULL;
  char *submitfile=NULL;
 
  char *date;
  time_t ltime;
  struct tm *time_of_day;

  struct ScriptData *pscript;
  struct ScriptData *script;
  struct ScriptHTML *shtml;
  struct CGIdata *ptr;

  struct XMLdata *xmldata;
  struct XMLDBdata *xmldbdata;
  struct XMLDBtree *tree;
  struct XMLDBelement *e;

  int num;
  int conflict=0;
  int fail=0;

  int missing=0;
  int action=0;

  char *code=NULL;
  char *path=NULL;

  char **post;
  char **search=NULL;
  char **replace=NULL;
  int rcnt=0;
  int icnt=0;
  int *iflg;
  char **ignore;
  char **remove;
  char *sep=NULL;
  char tmpname[4096];
  char fname[4096];
  char cmd[4096];

  FILE *dfp;
  FILE *mfp;

 
  pid_t pid;


  time(&ltime);
  time_of_day=localtime(&ltime);

  date=asctime(time_of_day);
  date[strlen(date)-1]=0;

  ptr=CGIMake();
  
  CGISetText(ptr,text,NULL);
  CGIGet(ptr); 
 
  crc_init();

  for (i=0;i<ptr->num;i++) {
    if (strcmp((ptr->arg[i])->name,"cfg")==0)
        cfgfile=CGIGetString(ptr->arg[i]);
    if (strcmp((ptr->arg[i])->name,"submitscript")==0)
        submitfile=CGIGetString(ptr->arg[i]);
  }

  if (cfgfile==NULL) {
    fprintf(stdout,"Expires: Thu, 01 Dec 1994 16:00:00 GMT\n");
    fprintf(stdout,"Content-type: text/html\n\n");
    fprintf(stdout,"<h1>Missing Configuration file</h1>\n");
    exit(-1);
  } else {
    /* load configuration */
    fp=fopen(cfgfile,"r");
    if (fp==NULL) {
      fprintf(stdout,"Expires: Thu, 01 Dec 1994 16:00:00 GMT\n");
      fprintf(stdout,"Content-type: text/html\n\n");
      fprintf(stdout,"<h1>Failed to open configuration file</h1>\n");
      exit(-1);
    }
    s=loadconfig(fp,&config);
    fclose(fp);
    if (s !=0) {
      fprintf(stdout,"Expires: Thu, 01 Dec 1994 16:00:00 GMT\n");
      fprintf(stdout,"Content-type: text/html\n\n");
      fprintf(stdout,"<h1>Error reading configuration file</h1>\n");
      exit(-1);
    }
  }

  for (j=0;j<ptr->num;j++) {
    if ((strcmp((ptr->arg[j])->name,config.path)==0)  &&
        ((ptr->arg[j])->value.raw.sze !=0)) path=CGIGetString(ptr->arg[j]);

  }

  for (j=0;j<ptr->num;j++) {
    if ((strcmp((ptr->arg[j])->name,config.code)==0)  &&
        ((ptr->arg[j])->value.raw.sze !=0)) code=CGIGetString(ptr->arg[j]);

  }
  
  for (j=0;j<ptr->num;j++) {
    if ((strcmp((ptr->arg[j])->name,config.action.name)==0)  &&
        ((ptr->arg[j])->value.raw.sze !=0)) {
      char *actiontxt;
      actiontxt=CGIGetString(ptr->arg[j]);
      if (strcmp(actiontxt,config.action.change)==0) action=1;
      if (strcmp(actiontxt,config.action.delete)==0) action=2;
    }
  }


  table=XMLDBMakeTable();
  for (i=0;i<ptr->num;i++) {
    if  ((ptr->arg[i])->value.txt.sze !=0) {
      e=XMLDBMakeElement((ptr->arg[i])->name,0,NULL,NULL,100);
      XMLDBFreeBuffer(e->data);
      e->data=RawToXML( (ptr->arg[i])->value.txt.buf,
                     (ptr->arg[i])->value.txt.sze);
      XMLDBAddTable(table,e);
    }
  }  

  if (action==2) {
    data.code=config.code;
    data.value=code;
    data.table=table;

    script=ScriptMake();
    shtml=ScriptHTMLMake(script);
    ScriptSetText(script,HTMLWriter,shtml);
    ScriptHTMLSetText(shtml,stream_output,stdout);
  
    pscript=ScriptMake();
    ScriptSetText(pscript,postscript,&postp);
 
    xmldata=XMLMake();
    xmldbdata=XMLDBMake(xmldata);
    tree=XMLDBMakeTree();

    XMLDBBuildTree(config.tree.path,config.tree.delim,tree);   
    XMLDBSetTree(xmldbdata,tree);

    XMLDBSetText(xmldbdata,decode,&data);  
    XMLSetStart(xmldata,XMLDBStart,xmldbdata);
    XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);

    if (path !=NULL) sprintf(fname,"%s/%s/%s",config.top,path,config.store);
    else sprintf(fname,"%s/%s",config.top,config.store);   

    fp=fopen(fname,"r");
    if (fp !=NULL) {
      while(fgets(lbuf,255,fp) !=NULL) {
        s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
        if (s !=0) break;
      }    
      fclose(fp);
    }
    XMLFree(xmldata);
    XMLDBFree(xmldbdata);
    XMLDBFreeTree(tree);
    missing=! data.flg; 
  }
 
  if ((action !=2) && (config.compare.num !=0)) {

    compare.code=config.code;
    compare.value=code;
    compare.num=config.compare.num;
    compare.txt=config.compare.txt;  
    compare.table=table;

    xmldata=XMLMake();
    xmldbdata=XMLDBMake(xmldata);
    tree=XMLDBMakeTree();

    XMLDBBuildTree(config.tree.path,config.tree.delim,tree);   
    XMLDBSetTree(xmldbdata,tree);

    XMLDBSetText(xmldbdata,decode_compare,&compare);  
    XMLSetStart(xmldata,XMLDBStart,xmldbdata);
    XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);

    if (path !=NULL) sprintf(fname,"%s/%s/%s",config.top,path,config.store);
    else sprintf(fname,"%s/%s",config.top,config.store);   
 
    fp=fopen(fname,"r");
    if (fp !=NULL) {
      while(fgets(lbuf,255,fp) !=NULL) {
        s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
        if (s !=0) break;
      } 
      fclose(fp);
    } 
    XMLFree(xmldata);
    XMLDBFree(xmldbdata);
    XMLDBFreeTree(tree);
    conflict=compare.flg;
  }

  if (action<2) {
    for (i=0;i<config.require.num;i++) {
      for (j=0;j<table->num;j++) {
        e=table->element[j];
        if ((strcmp(e->name,config.require.txt[i])==0)  &&
	    ((e->data)->sze !=0)) break;
      }
      if (j==table->num) break;
    }
    if (i !=config.require.num) fail=1;
  }  

  if ((action==0) && (code==NULL) && (!fail) && (!conflict)) {
    code=make_code(&config,table,date);
    e=XMLDBMakeElement(config.code,0,NULL,NULL,100);
    XMLDBAddBuffer(e->data,code,strlen(code));
    XMLDBAddTable(table,e); 
  }

 

  script=ScriptMake();
  shtml=ScriptHTMLMake(script);
  ScriptSetText(script,HTMLWriter,shtml);
  ScriptHTMLSetText(shtml,stream_output,stdout);
  
  pscript=ScriptMake();
  ScriptSetText(pscript,postscript,&postp);
 

  num=config.map.num;
  search=malloc(sizeof(char *)*num);
  replace=malloc(sizeof(char *)*num);

  for (i=0;i<config.map.num;i++) {
    search[i]=(config.map.ptr[i])->search;
    replace[i]=NULL;
    if ((config.map.ptr[i])->sep !=NULL) {
      if ((config.map.ptr[i])->type==2) sep=XMLToRaw((config.map.ptr[i])->sep);
      else if ((config.map.ptr[i])->type==1)
        sep=XMLToURL((config.map.ptr[i])->sep);
      else sep=XMLToHTML((config.map.ptr[i])->sep);
    }

    if ((config.map.ptr[i])->replace !=NULL) {
      if ((config.query !=NULL) && 
          (strcmp((config.map.ptr[i])->replace,config.query)==0) && 
          (table!=NULL)) {
        int n;
        n=strlen(config.query);
        for (j=0;j<table->num;j++) {
          e=table->element[j];
          if ((strncmp(e->name,
		      (config.map.ptr[i])->replace,n)==0))
	    DoReplace(&replace[i],e->name,e->data,(config.map.ptr[i])->mod,
		    sep,(config.map.ptr[i])->type);
	}
      }
      if (table !=NULL) {
        for (j=0;j<table->num;j++) {
          e=table->element[j];
          if ((strcmp(e->name,
		   (config.map.ptr[i])->replace)==0)) 
              DoReplace(&replace[i],e->name,e->data,(config.map.ptr[i])->mod,
	                sep,(config.map.ptr[i])->type);
         }
      }
                  
      if (replace[i]==NULL) {
	if ((config.map.ptr[i])->def !=NULL) {
	  if ((config.map.ptr[i])->type==2) 
            replace[i]=XMLToRaw((config.map.ptr[i])->def);
	  else if ((config.map.ptr[i])->type==1)
	    replace[i]=XMLToURL((config.map.ptr[i])->def);
	  else  replace[i]=XMLToHTML((config.map.ptr[i])->def);
	}
      }
      
    }
    if (sep !=NULL) free(sep);
    sep=NULL;
  }
  

  if ((fail==0) && (conflict==0) && (missing==0)) {
  
    pid=getpid();
    time(&ltime);  
    sprintf(tmpname,"submit.%d.%d",pid,(int) ltime);

    dfp=fopen(tmpname,"w");
 
    store.fp=dfp;
    store.code=config.code;
    store.value=code;
    store.action=action;
    store.created=config.created;
    store.modified=config.modified;
    store.date=date;
    store.data.num=config.data.num;
    store.data.txt=config.data.txt;  
    store.order.num=config.order.num;
    store.order.ptr=config.order.ptr;
    store.table=table;

   

    xmldata=XMLMake();
    xmldbdata=XMLDBMake(xmldata);
    tree=XMLDBMakeTree();

    XMLDBBuildTree(config.tree.path,config.tree.delim,tree);   
    XMLDBSetTree(xmldbdata,tree);

    XMLDBSetText(xmldbdata,decode_store,&store);  
    XMLSetStart(xmldata,XMLDBStart,xmldbdata);
    XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);

  
    s=fprintf(dfp,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"); 
    
    if (s !=-1) s=fprintf(dfp,"<%s>",tree->name[0]);
    if (s==-1) {
      fprintf(stdout,"Expires: Thu, 01 Dec 1994 16:00:00 GMT\n");
      fprintf(stdout,"Content-type: text/html\n\n");
      fprintf(stdout,"<h1>Error writing data</h1>\n");
      exit(-1);
    }

    if (path !=NULL) sprintf(fname,"%s/%s/%s",config.top,path,config.store);
    else sprintf(fname,"%s/%s",config.top,config.store);   
 
    
    fp=fopen(fname,"r");
    s=0;
    if (fp !=NULL) {
      while(fgets(lbuf,255,fp) !=NULL) {
        s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
        if (s !=0) break;
      } 
      fclose(fp);
    }
  
    if (s!=0) {
      fprintf(stdout,"Expires: Thu, 01 Dec 1994 16:00:00 GMT\n");
      fprintf(stdout,"Content-type: text/html\n\n");
      fprintf(stdout,"<h1>Error writing data</h1>\n");
      exit(-1);
    }

    if ((action !=2) && (store.flg==0)) {
       s=write_entry(dfp,action,tree->name[tree->depth-1],
                 config.data.num,config.data.txt,
		 table,config.code,code,config.created,config.modified,
                 date);
    }
    if (s !=-1) s=fprintf(dfp,"</%s>",tree->name[0]);
    if (s==-1) {
      fprintf(stdout,"Expires: Thu, 01 Dec 1994 16:00:00 GMT\n");
      fprintf(stdout,"Content-type: text/html\n\n");
      fprintf(stdout,"<h1>Error writing data</h1>\n");
      exit(-1);
    }
 

    fclose(dfp);
  
    XMLFree(xmldata);
    XMLDBFree(xmldbdata);
    XMLDBFreeTree(tree);
    mvfile(tmpname,fname);
  }
 

  iflg=malloc(sizeof(int)*config.display.num);
  for (i=0;i<config.display.num;i++) iflg[i]=0;
  for (j=0;j<ptr->num;j++) {
    if ((strcmp((ptr->arg[j])->name,config.display.name)==0)  &&
        ((ptr->arg[j])->value.raw.sze !=0)) {
      char *modetxt;
      modetxt=CGIGetString(ptr->arg[j]);
      for (i=0;i<config.display.num;i++) 
        if (strcmp(modetxt,config.display.tag[i])==0) iflg[i]=1;
      if (modetxt !=NULL) free(modetxt);
    }
  }
  
  for (i=0;i<config.display.num;i++) if (iflg[i]==0) icnt++;
  icnt+=5; 

  ignore=malloc(sizeof(char *)*icnt);

  if (fail) ignore[0]=config.ok;
  else ignore[0]=config.fail;      

  ignore[1]=config.conflict;
  ignore[2]=config.missing;
  if (conflict) {
    ignore[0]=config.ok;
    ignore[1]=config.fail;
  }
  if (missing) {
    ignore[0]=config.ok;
    ignore[1]=config.fail;
    ignore[2]=config.conflict;
  }

  if (action==0) {
    ignore[3]=config.action.change;
    ignore[4]=config.action.delete;
  } else if (action==1) {
    ignore[3]=config.action.add;
    ignore[4]=config.action.delete;
  } else {
    ignore[3]=config.action.add;
    ignore[4]=config.action.change;
  }
  

  icnt=5;
  for (i=0;i<config.display.num;i++) if (iflg[i]==0) {
    ignore[icnt]=config.display.tag[i];
    icnt++;
  }

  rcnt=config.display.num+7; 
  remove=malloc(sizeof(char *)*rcnt);

  remove[0]=config.ok;
  remove[1]=config.fail;
  remove[2]=config.missing;
  remove[3]=config.conflict;
  remove[4]=config.action.add;
  remove[5]=config.action.change;
  remove[6]=config.action.delete;

  rcnt=7;
  for (i=0;i<config.display.num;i++) {
    remove[rcnt]=config.display.tag[i];
    rcnt++;
  }  

  post=malloc(sizeof(char *)*config.post.num);
  for (i=0;i<config.post.num;i++) post[i]=(config.post.ptr[i])->tag;
  postp.fp=stdout;
  postp.cfg=&config;
  postp.table=table;

  ssidata.path=config.top;
  ssidata.script=script;
  ssidata.shtml=shtml;
  ssidata.text.user=stream_output;
  ssidata.text.data=stdout;

  ScriptHTMLSetSearch(shtml,num,search);
  ScriptHTMLSetReplace(shtml,num,replace);
  ScriptHTMLSetIgnore(shtml,icnt,ignore);
  ScriptHTMLSetRemove(shtml,rcnt,remove);
  ScriptHTMLSetPost(shtml,config.post.num,post);
  ScriptHTMLSetPostp(shtml,postproc,pscript);
  ScriptHTMLSetSsi(shtml,ssiproc,&ssidata);
  
  if (submitfile !=NULL) {
    fp=fopen(submitfile,"r");
    if (fp !=NULL) {
       while(fgets(lbuf,255,fp) !=NULL) { 
         s=ScriptDecode(script,lbuf,strlen(lbuf));
         if (s !=0) break; 
       }
      fclose(fp);
    
    }
  } else if (config.sfname.submit.buf !=NULL) {
    if (config.sfname.submit.type==1) {
      fp=fopen((config.sfname.submit.buf)->buf,"r");
      if (fp !=NULL) {
         while(fgets(lbuf,255,fp) !=NULL) { 
           s=ScriptDecode(script,lbuf,strlen(lbuf));
           if (s !=0) break;
         }
         fclose(fp);
      }
    } else s=ScriptDecode(script,(config.sfname.submit.buf)->buf,
                                 (config.sfname.submit.buf)->sze);
  }

  
  fflush(stdout);
  if ((fail) || (conflict) || (missing)) exit(0);

  if (config.submitc !=NULL) execute(config.submitc,action,path,code,fname);


  if ((config.super.num !=0) && (config.sfname.supermail.buf !=NULL)) {
    char *mail=NULL;

    char *tmp;
    for (i=0;i<config.super.num;i++) {
      if (mail==NULL) mail=malloc(strlen(config.super.mail[i])+1);
      else { 
        tmp=realloc(mail,strlen(mail)+strlen(config.super.mail[i])+2);
        if (tmp==NULL) free(mail);
        mail=tmp;
      }
      if (mail==NULL) break;
      if (i==0) strcpy(mail,config.super.mail[i]);
      else {
        strcat(mail,",");
        strcat(mail,config.super.mail[i]);
      }
    }
    if (mail==NULL) exit(-1);

    pid=getpid();
    time(&ltime);  
    sprintf(tmpname,"mail.%d.%d",pid,(int) ltime);
    mfp=fopen(tmpname,"w");

    postp.fp=mfp;
    ScriptHTMLSetText(shtml,stream_output,mfp);

    if ((config.super.num !=0) && (config.sfname.supermail.type==1)) {    
      fp=fopen((config.sfname.supermail.buf)->buf,"r");
      if (fp !=NULL) {
         while(fgets(lbuf,255,fp) !=NULL) { 
           s=ScriptDecode(script,lbuf,strlen(lbuf));
           if (s !=0) break;
         }
         fclose(fp);
      }
    } else s=ScriptDecode(script,(config.sfname.supermail.buf)->buf,
                                 (config.sfname.supermail.buf)->sze);
    fclose(mfp);
    sprintf(cmd,"%s %s < %s\n",
            config.mailc,mail,tmpname);
    system(cmd);
    unlink(tmpname);
  }

  if ((config.mail !=NULL) && (config.sfname.usermail.buf !=NULL)) {
    char *mail=NULL;
    for (i=0;i<table->num;i++) {
      e=table->element[i];
      if (strcmp(e->name,config.mail)==0) {
        if (mail==NULL) free(mail);
        mail=XMLToRaw(e->data);
      }
    }
     
    if (mail !=NULL) {

      pid=getpid();
      time(&ltime);  
      sprintf(tmpname,"mail.%d.%d",pid,(int) ltime);
      mfp=fopen(tmpname,"w");

      postp.fp=mfp;
      ScriptHTMLSetText(shtml,stream_output,mfp);
      if (config.sfname.usermail.type==1) {
        fp=fopen((config.sfname.usermail.buf)->buf,"r");
        if (fp !=NULL) {
           while(fgets(lbuf,255,fp) !=NULL) { 
             s=ScriptDecode(script,lbuf,strlen(lbuf));
             if (s !=0) break;
          }
          fclose(fp);
        }
      } else s=ScriptDecode(script,(config.sfname.usermail.buf)->buf,
                                 (config.sfname.usermail.buf)->sze);
      fclose(mfp);
      sprintf(cmd,"%s %s < %s\n",
            config.mailc,mail,tmpname);
      system(cmd);
      unlink(tmpname);
    }
  }


  return 0;




}
