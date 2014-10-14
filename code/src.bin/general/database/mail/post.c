/* post.c
   ====== */


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
#include <ctype.h>
#include "rcgi.h"
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "mail.h"
#include "post.h"

/*
 $Log$
*/

int postscript(char *buf,int sze,void *data) {
  struct postp *ptr;
  struct config *cfg;
  struct XMLDBtable *table;
  struct XMLDBelement *el;
  char *txt;
  char *vstr="value=";
  int i,j,s=0,e=0,c,num;
  ptr=(struct postp *) data;
  cfg=ptr->cfg;
  table=ptr->table;

  if (buf==NULL) return 0;
  if (buf[0] !='<') {
    fwrite(buf,sze,1,ptr->fp);
    return 0;
  }
  
  for (num=0;num<cfg->post.num;num++) 
    if (strcmp(ptr->tag,(cfg->post.ptr[num])->tag)==0) break;
  if (num==cfg->post.num) {
    fwrite(buf,sze,1,ptr->fp);
    return 0;
  }
  
  /* find the name of the tag */
  for (i=1;i<sze;i++) if (!isspace(buf[i])) break;
  for (j=i;j<sze;j++) if (isspace(buf[j])) break;


  if ((cfg->post.ptr[num])->type==0) {
    if (strncmp(buf+i,"input",j-i) !=0) {
      fwrite(buf,sze,1,ptr->fp);
      return 0;
    } 
  } else {
    if (strncmp(buf+i,"option",j-i) !=0) {
      fwrite(buf,sze,1,ptr->fp);
      return 0;
    } 
  }

  /* find the value tag */
  i=j;
  while (i<sze) {
    if (buf[i]==vstr[s]) s++;
    else s=0;
    if (vstr[s]==0) break;
    i++;
  }
  if (i==sze) {
    fwrite(buf,sze,1,ptr->fp);
    return 0;
  } 
  i++;
  for (j=i;j<sze;j++) if (!isspace(buf[j])) break;
  fwrite(buf,j,1,ptr->fp);

  /* decode the value */
  if (buf[j]=='\'') {
    s=j+1;
    for (i=j+1;i<sze;i++) if (buf[i]=='\'') break;
    e=i;
    i++;
  } else if (buf[j]=='"') {
    s=j+1;
    for (i=j+1;i<sze;i++) if (buf[i]=='"') break;
    e=i;
    i++;
  } else {
    s=j;
    for (i=j;i<sze;i++) if (isspace(buf[i])) break; 
    e=j;
  }
  fwrite(buf+j,i-j,1,ptr->fp);

  for (c=0;c<table->num;c++) {
    el=table->element[c];
    if (strcmp(el->name,(cfg->post.ptr[num])->name) !=0) continue;
    txt=getDBstring(el->data);
    if (strncmp(buf+s,txt,e-s)==0) break;
  }
  if (c<table->num) {
    if ((cfg->post.ptr[num])->type==0) fprintf(ptr->fp," checked");
    else fprintf(ptr->fp," selected");
  }  
  fwrite(buf+i,sze-i,1,ptr->fp);

  return 0;
}

