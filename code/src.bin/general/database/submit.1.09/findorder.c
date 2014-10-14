/* findorder.c
   =========== */


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
#include <time.h>
#include "rcgi.h"
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"
#include "submit.h"
#include "convert.h"

/*
 $Log: findorder.c,v $
 Revision 1.2  2004/04/27 17:25:58  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/04/15 21:35:15  barnes
 Initial revision

*/

int findorder(int num,struct order **ptr,
               struct XMLDBtable *src,struct XMLDBtable *dst) {

  int n;
  int flg=0;
  int x,y;
  char *name;
  int type;
  struct XMLDBelement *ex=NULL,*ey=NULL;
  char *xv=NULL,*yv=NULL;
  for (n=0;n<num;n++) {
    name=ptr[n]->name;
    type=ptr[n]->type;
   
    if (type==0) {
      flg=0; /* always at end */
      break;
    }
    if (type==1) {
      flg=1; /* always first */
      break;
    }
    
    for (x=0;x<src->num;x++) {
      ex=src->element[x];
      if (strcmp(name,ex->name) !=0) continue; 
      xv=XMLToRaw(ex->data);
      
      for (y=0;y<dst->num;y++) {
        ey=dst->element[y];
      
        if (strcmp(name,ey->name) !=0) continue;
        yv=XMLToRaw(ey->data);

        if (type>3) {
          double xn,yn;
          xn=atof(xv);
          yn=atof(yv);
          if ((type==4) && (xn>=yn)) break;
          if ((type==5) && (xn<=yn)) break;
          if ((type==6) && (xn>yn)) break;
          if ((type==7) && (xn<yn)) break;
	} else if (type==2) {
          int c;
          for (c=0;(yv[c] !=0) && (xv[c] !=0);c++) 
            if (tolower(xv[c]) !=tolower(yv[c])) break;
          if (xv[c]==0) {
	    flg=1;
            break;
	  } else if (tolower(xv[c])<tolower(yv[c])) {
            flg=1;
            break;
	  } else break;
        } else if (type==3) {
          /* name */
          char *tx,*ty;
          int a,b,c,d,e;
          tx=malloc(strlen(xv)+1);
          ty=malloc(strlen(yv)+1);
          a=0;

          c=strlen(xv)-1;
          while (c>=0) {
            while ((c>=0) && (!isalnum(xv[c]))) c--;
            b=c;
            while ((c>=0) && (isalnum(xv[c]))) c--;
            if (c>=0) e=c+1;
            else e=0;
            for (d=e;d<=b;d++) {
              tx[a]=xv[d];
              a++;
	    }
	  }
          tx[a]=0;
          a=0;
          c=strlen(yv)-1;
          while (c>=0) {
            while ((c>=0) && (!isalnum(yv[c]))) c--;
            b=c;
            while ((c>=0) && (isalnum(yv[c]))) c--;
            if (c>=0) e=c+1;
            else e=0;
            for (d=e;d<=b;d++) {
              ty[a]=yv[d];
              a++;
	    }
	  }
          ty[a]=0;


          for (c=0;(ty[c] !=0) && (tx[c] !=0);c++) 
            if (tolower(tx[c]) !=tolower(ty[c])) break;
          if (tx[c]==0) {
            free(tx);
            free(ty);
	    flg=1;
            break;
	  } else if (tolower(tx[c])<tolower(ty[c])) {
            free(tx);
            free(ty);
            flg=1;
            break;
	  } else {
            free(tx);
            free(ty);
            break;
	  }

	}

        free(yv);
        yv=NULL;
	
      }
      if (yv !=NULL) free(yv);
      yv=NULL;
      free(xv);
      xv=NULL;
      if (y<dst->num) break;
    }
    if (x<src->num) break;
  }
  if (n==num) flg=1;
  return flg;
}

