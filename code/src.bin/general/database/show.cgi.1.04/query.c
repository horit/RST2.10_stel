/* show.c
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
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "reval.h"

#include "convert.h"



/*
 $Log: query.c,v $
 Revision 1.4  2006/11/19 00:17:52  code
 Added caseless searches.

 Revision 1.3  2005/03/11 14:34:54  barnes
 Modified query parser to propably handle logical operators, this allows
 complicated queries to be generated. The method scans the query any
 generates a string containing a logical expression which is then evaluated
 using the "eval" library.

 Revision 1.2  2004/04/27 17:17:40  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/04/15 17:22:04  barnes
 Initial revision

*/


int decode_value(char *ptr,double *value,void *data) {
  
  if ((isdigit(ptr[0])) || (ptr[0]=='.')) {
     *value=atof(ptr);
     return 0;
   }
  return 0;
}



int process_query(struct XMLDBtable *ptr,
                  struct XMLDBtable *table,char *query,int index,
                  int counter) {
  struct XMLDBelement *e;
  struct XMLDBelement *c;
  char eq[1024];
  char *optab[]={"in","ic","lt","le","gt","ge","eq","lta","gta","ltn","gtn",0};
  char *op=NULL;
  int opn=0;
  char *name=NULL;
 
  int i,n,j=0,k;
  int qflg=0;
  int flg=0;
  int s;
  double val;
 
  if (query==NULL) return 1;
  n=strlen(query);
  eq[0]=0;
 
 
  for (i=0;i<table->num;i++) {
    e=table->element[i];
    if (strncmp(e->name,query,n)==0) break;
  }
  if (i==table->num) return 1;
 
  for (i=0;i<table->num;i++) {
    n=strlen(query); 
    e=table->element[i];
    if (strncmp(e->name,query,n)!=0) continue;
    if (e->name[n] !='!') continue;
    n++;    
    while(e->name[n] !=0) {
      switch (e->name[n]) {
      case '[': 
        strcat(eq,"(");
        n++;
        break;
      case ']':
        strcat(eq,")");
        n++;
        break;
      case '#': 
        strcat(eq,"&");
        n++;
        break;
      case ':': 
        strcat(eq,"|");
        n++;
        break;
      case '0': 
        strcat(eq,"0");
        n++;
        break;
      case '1': 
        strcat(eq,"1");
        n++;
        break;

      default :  
        j=n;    
        while ((e->name[j] !=0) && (e->name[j] !='*')) j++;
        if ((j-n)==0) break;
        op=malloc(j-n+1);
        memcpy(op,e->name+n,j-n+1);
        op[j-n]=0;
        if (e->name[j]==0) break;
        n=j+1;
        j=n;
        while ((e->name[j] !=0) && (e->name[j] !=':') &&
             (e->name[j] !='#') && (e->name[j] !='[') &&
             (e->name[j] !=']') && (e->name[j] !='0') &&
             (e->name[j] !='1')) j++;
        if ((j-n)==0) break;
        name=malloc(j-n+1);
        memcpy(name,e->name+n,j-n+1);
        name[j-n]=0;
   
        for (opn=0;optab[opn] !=0;opn++) if (strcmp(op,optab[opn])==0) break;
        if (optab[opn]==NULL) break;
        free(op);
   
        /* now do the query */      
        flg=0;
        if (strcmp(name,"index")==0) {
	  char *ys;
          double yv;
          ys=XMLToRaw(e->data);
          yv=atof(ys);
          if ((opn==2) && (index<yv)) flg=1;
          if ((opn==3) && (index<=yv)) flg=1;
          if ((opn==4) && (index>yv)) flg=1;
          if ((opn==5) && (index>=yv)) flg=1;
          if ((opn==6) && (index==yv)) flg=1;
          free(ys);
        }

        if (strcmp(name,"counter")==0) {
	  char *ys;
          double yv;
          ys=XMLToRaw(e->data);
          yv=atof(ys);
          if ((opn==2) && (counter<yv)) flg=1;
          if ((opn==3) && (counter<=yv)) flg=1;
          if ((opn==4) && (counter>yv)) flg=1;
          if ((opn==5) && (counter>=yv)) flg=1;
          if ((opn==6) && (counter==yv)) flg=1;
          free(ys);
        }
        
        for (k=0;k<ptr->num;k++) {
	  c=ptr->element[k];
          if (strcmp(c->name,name) !=0) continue;
          if (opn==0) { /* simple in string */
            int x=0,y=0;
            for (x=0;x<(c->data)->sze;x++) {
              if ((e->data)->buf[y]==(c->data)->buf[x]) y++;
              else y=0;
              if (y==(e->data)->sze) break;
	    }
            if (y==(e->data)->sze) flg=1;
          } else if (opn==1) { /* caseless in string */
            int x=0,y=0;
            for (x=0;x<(c->data)->sze;x++) {
              if (tolower((e->data)->buf[y])==tolower((c->data)->buf[x])) y++;
              else y=0;
              if (y==(e->data)->sze) break;
            }
            if (y==(e->data)->sze) flg=1;  
	  } else if ((opn>1) && (opn<7)) { /* less than numeric */
            char *xs,*ys;
            double xv,yv;
            xs=XMLToRaw(c->data);
            ys=XMLToRaw(e->data);
            xv=atof(xs);
            yv=atof(ys);
            free(xs);
            free(ys);           
            if ((opn==2) && (xv<yv)) flg=1;
            if ((opn==3) && (xv<=yv)) flg=1;
            if ((opn==4) && (xv>yv)) flg=1;
            if ((opn==5) && (xv>=yv)) flg=1;
            if ((opn==6) && (xv>=yv)) flg=1;
	  } else if ((opn>6) && (opn<9)) {
            int cv;
            char *xs,*ys;
            xs=XMLToRaw(c->data);
            ys=XMLToRaw(e->data);

            for (cv=0;(ys[cv] !=0) && (xs[cv] !=0);cv++) 
              if (tolower(xs[cv]) !=tolower(ys[cv])) break;

            if (xs[cv]==0) {
              if (opn==8) flg=1;
            } else {
              if ((tolower(ys[cv])<tolower(xs[cv])) && (opn==8)) flg=1;
              if ((tolower(ys[cv])>tolower(xs[cv])) && (opn==7)) flg=1;
	    }
            free(xs);
            free(ys);
	  } else if (opn>8) {
            char *xs,*ys;
            char *tx,*ty;
            int av,bv,cv,dv,ev;

            xs=XMLToRaw(c->data);
            ys=XMLToRaw(e->data);

            tx=malloc(strlen(xs)+1);
            ty=malloc(strlen(ys)+1);
            av=0;

            cv=strlen(xs)-1;
            while (cv>=0) {
              while ((cv>=0) && (!isalnum(xs[cv]))) cv--;
              bv=cv;
              while ((cv>=0) && (isalnum(xs[cv]))) cv--;
              if (cv>=0) ev=cv+1;
              else ev=0;
              for (dv=ev;dv<=bv;dv++) {
                tx[av]=xs[dv];
                av++;
	      }
	    }
            tx[av]=0;

            av=0;
            cv=strlen(ys)-1;
            while (cv>=0) {
              while ((cv>=0) && (!isalnum(ys[cv]))) cv--;
              bv=cv;
              while ((cv>=0) && (isalnum(ys[cv]))) cv--;
              if (cv>=0) ev=cv+1;
              else ev=0;
              for (dv=ev;dv<=bv;dv++) {
                ty[av]=ys[dv];
                av++;
	      }
	    }
            ty[av]=0;
            for (cv=0;(ty[cv] !=0) && (tx[cv] !=0);cv++) 
            if (tolower(tx[cv]) !=tolower(ty[cv])) break;
  
            if (tx[cv]==0) {
              if (opn==10) flg=1;
            } else {
              if ((tolower(ty[cv])<tolower(tx[cv])) && (opn==10)) flg=1;
              if ((tolower(ty[cv])>tolower(tx[cv])) && (opn==9)) flg=1;
	    }
            free(tx);
            free(ty);
            free(xs);
            free(ys);
	  }    
        }
        free(name);
        if (flg !=0) strcat(eq,"1");
        else strcat(eq,"0");
        n=j;
      }
    }
      
  }
  s=Eval(eq,&val,decode_value,NULL,NULL,NULL);
  qflg=(int) val;
  return qflg;
}
