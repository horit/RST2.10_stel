/* ssi.c
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
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "ssi.h"

/*
 $Log: ssi.c,v $
 Revision 1.4  2004/04/27 16:52:48  barnes
 Audit - Enforced warning set to "all".

 Revision 1.3  2004/04/26 18:55:31  barnes
 Fixed bug in assigning buffer.

 Revision 1.2  2004/04/22 18:41:15  barnes
 Fixed bug in allocating buffer.

 Revision 1.1  2003/05/01 14:04:26  barnes
 Initial revision

*/


int ssifbuf(char *buf,int sze,void *data) {
  char *tmp=NULL;
  struct ssitxt *ptr;
  ptr=(struct ssitxt*) data;

  if (ptr->buf==NULL) ptr->buf=malloc(sze);
  else {
    tmp=realloc(ptr->buf,ptr->sze+sze);
    if (tmp==NULL) {
      free(ptr->buf);
      ptr->buf=NULL;
    }
    ptr->buf=tmp;
  }
  if (ptr->buf==NULL) return -1;
  memcpy(ptr->buf+ptr->sze,buf,sze);
  ptr->sze+=sze;
  return 0;
}

int ssiproc(char *token,int sze,void *data) {
  FILE *fp;
  char fname[1024];
  char lbuf[256];
  struct ssitxt txt;
  struct ssidata *ptr;
  struct ScriptData *script;
  struct ScriptHTML *shtml;
  char *path;
  int i=0,j,s;


  ptr=(struct ssidata *) data;
  
  path=ptr->path;
  script=ptr->script;
  shtml=ptr->shtml;

  txt.buf=NULL;
  txt.sze=0;
 
  for (j=5;(token[j] !=0) && (isspace(token[j]));j++);
  for (i=j;(token[i] !=0) && (!isspace(token[i]));i++);
  token[i]=0;
  if (strcmp(token+j,"include") !=0) return 0;

  for (j=i+1;(token[j] !=0) && (token[j] !='=');j++);
  i=j+1;
  for (j=i;(token[j] !=0) && ((token[j]=='"') || (isspace(token[j])));j++);
  i=j;
  for (j=i;(token[j] !=0) && (token[j] !='"') && (!isspace(token[j]));j++);
  token[j]=0;
  sprintf(fname,"%s/%s",path,token+i);

  /* filename may contain tokens */

  ScriptHTMLSetText(shtml,ssifbuf,&txt);
  s=ScriptDecode(script,fname,strlen(fname));
  ScriptHTMLSetText(shtml,ptr->text.user,ptr->text.data);
  strncpy(fname,txt.buf,txt.sze);
  fname[txt.sze]=0;
  free(txt.buf);
  
  fp=fopen(fname,"r");
  if (fp !=NULL) {
    while (fgets(lbuf,255,fp) !=NULL) {
      s=ScriptDecode(script,lbuf,strlen(lbuf));
      if (s !=0) break;
    }
    fclose(fp);
    if (s !=0) fprintf(stdout,"[error processing directive %s]",fname);

  } else {
    fprintf(stdout,"[error processing directive %s]",fname);
  }
  return 0;
}


