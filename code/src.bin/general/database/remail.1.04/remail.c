/* remail.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "rcgi.h"
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "remail.h"
#include "loadconfig.h"
#include "convert.h"
#include "post.h"
#include "ssi.h"

/*
 $Log: remail.c,v $
 Revision 1.6  2005/03/22 16:41:07  barnes
 Preserved query through system.

 Revision 1.5  2005/03/22 15:20:59  barnes
 Fixed the modifier syntax to agree with show.cgi.

 Revision 1.4  2004/06/21 21:45:07  barnes
 Added missing header.

 Revision 1.3  2004/04/28 15:23:45  barnes
 Changed the temporary filename generation to be more portable.

 Revision 1.2  2004/04/27 17:06:25  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/05/01 14:05:53  barnes
 Initial revision

*/

struct ssidata ssidata;
struct config config;
struct XMLDBtable *table=NULL;


struct data {
  char *mail;
  char *value;
  struct XMLDBtable *table;
  int flg;
};

struct data data;
struct postp postp;

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

int decode(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {
  int i,j;
  struct data *d;
  struct XMLDBelement *e;
  struct XMLDBelement *c;
  d=(struct data *)data;
  if (d->mail==NULL) return 0;
  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,d->mail)==0) {
      char *tmp;
      tmp=getDBstring(e->data);
      if (tmp==NULL) continue;
      if (d->value==NULL) continue;
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
      if (strcmp(e->name,d->mail)==0) continue;
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

int main(int argc,char *argv[]) {
  int i,j,s;
  FILE *fp;
  char lbuf[256];
  char *cfgfile=NULL;
  struct ScriptData *pscript;
  struct ScriptData *script;
  struct ScriptHTML *shtml;
  struct CGIdata *ptr;
   
  struct XMLdata *xmldata;
  struct XMLDBdata *xmldbdata;
  struct XMLDBtree *tree;
  struct XMLDBelement *e;

  int num;


  char *mail=NULL;
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

  char fname[4096];
  FILE *mfp;
  char tmpfile[64];
  char cmd[1024];

  time_t ltime;
  pid_t pid;


  ptr=CGIMake();
  
  CGISetText(ptr,text,NULL);
  CGIGet(ptr); 
  
  for (i=0;i<ptr->num;i++) {
    if (strcmp((ptr->arg[i])->name,"cfg")==0)
        cfgfile=CGIGetString(ptr->arg[i]);
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
    if ((strcmp((ptr->arg[j])->name,config.mail)==0)  &&
        ((ptr->arg[j])->value.raw.sze !=0)) mail=CGIGetString(ptr->arg[j]);

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

  data.mail=config.mail;
  data.value=mail;
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
  icnt++; 

  ignore=malloc(sizeof(char *)*icnt);

  if (data.flg==0) ignore[0]=config.ok;
  else ignore[0]=config.fail;   

  icnt=1;
  for (i=0;i<config.display.num;i++) if (iflg[i]==0) {
    ignore[icnt]=config.display.tag[i];
    icnt++;
  }

  rcnt=config.display.num+2; 
  remove=malloc(sizeof(char *)*rcnt);
  remove[0]=config.ok;
  remove[1]=config.fail;
  rcnt=2;
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
  

  if (config.sfname.remail.buf !=NULL) {
    if (config.sfname.remail.type==1) {
      fp=fopen((config.sfname.remail.buf)->buf,"r");
      if (fp !=NULL) {
         while(fgets(lbuf,255,fp) !=NULL) { 
           s=ScriptDecode(script,lbuf,strlen(lbuf));
           if (s !=0) break;
         }
         fclose(fp);
      }
    } else s=ScriptDecode(script,(config.sfname.remail.buf)->buf,
                                 (config.sfname.remail.buf)->sze);
  }
   
  if (data.flg==0) exit(0);

  if (config.sfname.remailmail.buf !=NULL) {

    pid=getpid();
    time(&ltime);  
    sprintf(tmpfile,"remail.%d.%d",pid,(int) ltime);
    mfp=fopen(tmpfile,"w");
    postp.fp=mfp;
    ScriptHTMLSetText(shtml,stream_output,mfp);

    if (config.sfname.remailmail.type==1) {
      fp=fopen((config.sfname.remailmail.buf)->buf,"r");
      if (fp !=NULL) {
         while(fgets(lbuf,255,fp) !=NULL) { 
           s=ScriptDecode(script,lbuf,strlen(lbuf));
           if (s !=0) break;
         }
         fclose(fp);
      }
    } else s=ScriptDecode(script,(config.sfname.remailmail.buf)->buf,
                                 (config.sfname.remailmail.buf)->sze);
 
    fclose(mfp);
  
    sprintf(cmd,"%s %s < %s\n",
              config.mailc,mail,tmpfile);
    system(cmd);
    unlink(tmpfile);
  }
  fflush(stdout);  
  return 0;




}
