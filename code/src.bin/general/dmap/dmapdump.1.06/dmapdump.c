/* dmapdump.c
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"

/*
 $Log: dmapdump.c,v $
 Revision 1.7  2005/08/08 14:48:43  barnes
 Added option to dump the command line options

 Revision 1.6  2004/07/30 18:04:28  barnes
 Fixed bug in reporting the array indices in the wrong order.

 Revision 1.5  2004/07/14 17:30:06  barnes
 Added help message.

 Revision 1.4  2004/06/22 18:14:06  barnes
 Changed reporting of integers to "int".

 Revision 1.3  2004/06/22 18:10:02  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 19:03:14  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/04/27 17:44:22  barnes
 Initial revision

*/


struct OptionData opt;
int arg=0;

int main(int argc,char *argv[]) {
  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  char **tmp;
  unsigned char dflg=0;
  unsigned char help=0;
  unsigned char option=0;


  FILE *fp;
  int c;
  int x,n;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"d",'x',&dflg);

  if (argc>1) {
    arg=OptionProcess(1,argc,argv,&opt,NULL); 
    if (help==1) {
      OptionPrintInfo(stdout,hlpstr);
      exit(0);
    }
    if (option==1) {
      OptionDump(stdout,&opt);
      exit(0);
    }

    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }  
  } else fp=stdin; 

    
  while ((ptr=DataMapFread(fp)) !=NULL) {
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
      for (x=0;x<a->dim;x++) fprintf(stdout,"[%d]",a->rng[a->dim-1-x]);
      if (dflg) {
        fprintf(stdout,"=");
        n=1;
        for (x=0;x<a->dim;x++) n=a->rng[x]*n;
        for (x=0;x<n;x++) {
	  if (x % a->rng[0]==0) fprintf(stdout,"\n\t\t");
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
  fclose(fp);
  return 0;  


}
