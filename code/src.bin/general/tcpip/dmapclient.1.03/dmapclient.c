/* ipclient.c
   ==========
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "option.h"
#include "rtypes.h"
#include "dmap.h"
#include "connex.h"
#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: dmapclient.c,v $
 Revision 1.4  2005/08/08 15:12:37  barnes
 Added option to dump the command line options

 Revision 1.3  2004/06/22 18:19:55  barnes
 Changed references of integers to "int".

 Revision 1.2  2004/05/04 19:13:41  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/04/27 17:53:55  barnes
 Initial revision

*/

struct OptionData opt;

int main(int argc,char *argv[]) {
  int arg;
  int sock;
  int remote_port=0;
  char *host;
  int flag,status,size;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char *buffer=NULL;
  unsigned char stream=255;

  char **tmp; 
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  int c,n,x;

  int sval=255;
  unsigned char dflg=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"d",'x',&dflg);
  OptionAdd(&opt,"s",'i',&sval);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (argc-arg<2) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  host=argv[argc-2];
  remote_port=atoi(argv[argc-1]);
  stream=sval;

  sock=ConnexOpen(host,remote_port); 
  if (sock<0) {
    fprintf(stderr,"Could not connect to host.\n");
    exit(-1);
  }
  status=(write(sock,&stream,sizeof(char)) !=1);
  if (status !=0) {
    fprintf(stderr,"Could not set stream to host.\n");
    exit(-1);
  }
 
  do {
   status=ConnexRead(1,&sock,&buffer,&size,&flag,NULL);
   if (status==-1) break;
   if (flag !=-1) {
     fprintf(stderr,"Message size:%d\n",size);
     if (size==0) continue;
     ptr=DataMapDecodeBuffer(buffer,size);
     if (ptr==NULL) fprintf(stderr,"Not a recognized message.\n");
     else {
       fprintf(stdout,"scalars:\n");
       for (c=0;c<ptr->snum;c++) {
	 s=ptr->scl[c];
	 switch (s->type) {
	 case DATACHAR:
	   fprintf(stdout,"\tchar");
	   break;
	 case DATASHORT:
	   fprintf(stdout,"\tshort");
	   break;
	 case DATAINT:
	   fprintf(stdout,"\tint");
	   break;
	 case DATAFLOAT:
	   fprintf(stdout,"\tfloat");
	   break;
	 case DATADOUBLE:
	   fprintf(stdout,"\tdouble");
	   break;
	 case DATASTRING:
	   fprintf(stdout,"\tstring");
	   break;
	 }
	 fprintf(stdout,"\t%c%s%c",'"',s->name,'"');
	 fprintf(stdout," = ");
	 switch (s->type) {
	 case DATACHAR:
	   fprintf(stdout,"%d",*(s->data.cptr));
	   break;
	 case DATASHORT:
	   fprintf(stdout,"%d",*(s->data.sptr));
	   break;
	 case DATAINT:
	   fprintf(stdout,"%d",*(s->data.lptr));
	   break;
	 case DATAFLOAT:
	   fprintf(stdout,"%g",*(s->data.fptr));
	   break;
	 case DATADOUBLE:
	   fprintf(stdout,"%g",*(s->data.dptr));
	   break;
	 case DATASTRING:
	   tmp=(char **) s->data.vptr;
	   fprintf(stdout,"%c%s%c",'"',*tmp,'"');
	   break;
	 }
	 fprintf(stdout,"\n");
       }
       fprintf(stdout,"arrays:\n");
       for (c=0;c<ptr->anum;c++) {
	 a=ptr->arr[c];
	 switch (a->type) {
	 case DATACHAR:
	   fprintf(stdout,"\tchar");
	   break;
	 case DATASHORT:
	   fprintf(stdout,"\tshort");
	   break;
	 case DATAINT:
	   fprintf(stdout,"\tint");
	   break;
	 case DATAFLOAT:
	   fprintf(stdout,"\tfloat");
	   break;
	 case DATADOUBLE:
	   fprintf(stdout,"\tdouble");
	   break;
	 case DATASTRING:
	   fprintf(stdout,"\tstring");
	   break;
	 }
	 fprintf(stdout,"\t%c%s%c",'"',a->name,'"');
	 fprintf(stdout," ");
	 for (x=0;x<a->dim;x++) fprintf(stdout,"[%d]",a->rng[x]);
	 if (dflg) {
	   fprintf(stdout,"=");
	   n=1;
	   for (x=0;x<a->dim;x++) n=a->rng[x]*n;
	   for (x=0;x<n;x++) {
	     if (x % a->rng[a->dim-1]==0) fprintf(stdout,"\n\t\t");
	     else if (x !=0) fprintf(stdout,",\t");
	     switch (a->type) {
	     case DATACHAR:
	       fprintf(stdout,"%d",a->data.cptr[x]);
	       break;
	     case DATASHORT:
	       fprintf(stdout,"%d",a->data.sptr[x]);
	       break;
	     case DATAINT:
	       fprintf(stdout,"%d",a->data.lptr[x]);
	       break;
	     case DATAFLOAT:
	       fprintf(stdout,"%g",a->data.fptr[x]);
	       break;
	     case DATADOUBLE:
	       fprintf(stdout,"%g",a->data.dptr[x]);
	       break;
	     case DATASTRING:
	       tmp=(char **) a->data.vptr;
	       fprintf(stdout,"%c%s%c",'"',tmp[x],'"');
	       break;

	     }
	   }
	   fprintf(stdout,"\n");
	 } else fprintf(stdout,"\n");
       }
      DataMapFree(ptr);
     }
   }           
  } while(1);
  fprintf(stderr,"Connection failed.\n");
  return 0;

}
   

 










