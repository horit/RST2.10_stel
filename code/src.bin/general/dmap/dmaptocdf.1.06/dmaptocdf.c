/* dmaptocdf.c
   ===========
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
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "cdf.h"
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "hlpstr.h"
#include "errstr.h"

/*
 $Log: dmaptocdf.c,v $
 Revision 1.7  2005/08/08 14:48:43  barnes
 Added option to dump the command line options

 Revision 1.6  2004/07/14 17:52:13  barnes
 Added error message.

 Revision 1.5  2004/07/14 17:32:37  barnes
 Added help message.

 Revision 1.4  2004/06/22 18:17:49  barnes
 Changed references of integers to "int".

 Revision 1.3  2004/06/22 18:10:39  barnes
 Changed references of Long to Int.

 Revision 1.2  2004/05/04 19:03:56  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/04/27 17:44:38  barnes
 Initial revision

*/


int snum=0;
struct DataMapScalar **sptr=NULL;
char **cdfsname;

int anum=0;
struct DataMapArray **aptr=NULL;
char **cdfaname;

struct OptionData opt;
int arg=0;

int loadmap(FILE *fp) {
  char buf[256];
  int off=0;
  int c,x,y,t,dim;

  do {
    off=0;
    do {
      c=fgetc(fp);
      if (c==-1) break;
      if (c==';') break;
      buf[off]=c;
      off++;
      if (off>255) c=-1;
    } while (c !=-1);
    if (c==-1) break;
    buf[off]=0;

    x=0;
    while((!isalnum(buf[x])) && (x<off)) x++;
    y=x;
    while((isalnum(buf[y])) && (y<off)) y++;
    t=0;
    if (strncmp(buf+x,"char",4)==0) t=DATACHAR;
    if (strncmp(buf+x,"short",5)==0) t=DATASHORT;
    if (strncmp(buf+x,"int",4)==0) t=DATAINT;
    if (strncmp(buf+x,"float",5)==0) t=DATAFLOAT;
    if (strncmp(buf+x,"double",6)==0) t=DATADOUBLE;
    if (strncmp(buf+x,"string",6)==0) t=DATASTRING;
    x=y;
    while ((!isalnum(buf[x])) && (x<off)) x++;
    y=x;
    while ((isalnum(buf[y])) && (y<off)) y++;
    buf[y]=0;
    dim=atoi(buf+x);
    x=y;
    while ((buf[x] !='"') && (x<off)) x++;
    y=x+1;
    while ((buf[y] !='"') && (y<off)) y++;
    buf[y]=0;
    if (dim==0) {
      if (sptr==NULL) sptr=malloc(sizeof(struct DataMapScalar *));
      else {
        struct DataMapScalar **tmp;
        tmp=realloc(sptr,(snum+1)*sizeof(struct DataMapScalar *));
        if (tmp==NULL) break;
        sptr=tmp;
      }
      sptr[snum]=DataMapMakeScalar(buf+x+1,t,NULL);
      if (cdfsname==NULL) cdfsname=malloc(sizeof(char *));
      else {
        char **tmp;
        tmp=realloc(cdfsname,(snum+1)*sizeof(char *));
        if (tmp==NULL) break;
        cdfsname=tmp;
      }  
      snum++;
    } else {
      if (aptr==NULL) aptr=malloc(sizeof(struct DataMapArray *));
      else {
        struct DataMapArray **tmp;
        tmp=realloc(aptr,(anum+1)*sizeof(struct DataMapArray *));
        if (tmp==NULL) break;
        aptr=tmp;
      }
      if (cdfaname==NULL) cdfaname=malloc(sizeof(char *));
      else {
        char **tmp;
        tmp=realloc(cdfaname,(snum+1)*sizeof(char *));
        if (tmp==NULL) break;
        cdfaname=tmp;
      }  
      aptr[anum]=DataMapMakeArray(buf+x+1,t,dim,NULL,NULL);
      anum++;
    }
 
    x=y+1;
    while ((buf[x] !='=') && (x<off)) x++;
    while ((!isalnum(buf[x])) && (x<off)) x++;
    y=x;
    while ((buf[y] !=';') && (y<off)) y++;
    buf[y]=0;
    if (dim==0) {
      cdfsname[snum-1]=malloc(y-x+1);
      strcpy(cdfsname[snum-1],buf+x);
    } else {
      cdfaname[anum-1]=malloc(y-x+1);
      strcpy(cdfaname[anum-1],buf+x);
    }
  } while (c !=-1);
  return 0;
}



int main(int argc,char *argv[]) {
  struct DataMap *ptr;
  struct DataMapScalar *sx,*sy;
  struct DataMapArray *ax,*ay;

  unsigned char vbflg=0;
  unsigned char help=0;
  unsigned char option=0;


  FILE *fp;
  FILE *mapfp;
  int n,c,x;
  int block=0;
 
   
  CDFid id;
  CDFstatus status;

  int sze;


  int varid;   
  int nume;
  char *buf=NULL;
  char **strptr;

  int intervals[CDF_MAX_DIMS];
  int counts[CDF_MAX_DIMS];
  int indices[CDF_MAX_DIMS];

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vbflg);

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
  } else {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  /* load the map */

  mapfp=fopen(argv[arg+1],"r");
  loadmap(mapfp);
  fclose(mapfp);

  status=CDFopen(argv[arg+2],&id);
  if (status !=CDF_OK) {
    fprintf(stderr,"Error opening CDF file.\n");
    exit(-1);
  }

  block=0;
  while ((ptr=DataMapFread(fp)) !=NULL) {

    for (c=0;c<ptr->snum;c++) {
      sx=ptr->scl[c];
      for (n=0;n<snum;n++) {
        sy=sptr[n];
        if (strcmp(sx->name,sy->name) !=0) continue;
        if (sx->type !=sy->type) continue;
        break;
      }
      if (n !=snum) { /* mapped variable */
       status=CDFlib(GET_,rVAR_NUMBER_,cdfsname[n],&varid,NULL_);
        if (status !=CDF_OK) {
          fprintf(stderr,"Error accessing CDF file.\n");
          exit(-1);
        }
      
        switch (sx->type) {
        case DATACHAR:
          status=CDFlib(SELECT_,rVAR_,varid,
                        SELECT_,rVARs_RECNUMBER_,block,
                        PUT_,rVAR_DATA_,sx->data.vptr,NULL_);
          break;
        case DATASHORT:
          status=CDFlib(SELECT_,rVAR_,varid,
                        SELECT_,rVARs_RECNUMBER_,block,
                        PUT_,rVAR_DATA_,sx->data.vptr,NULL_);
        
          break;
        case DATAINT:
          status=CDFlib(SELECT_,rVAR_,varid,
                        SELECT_,rVARs_RECNUMBER_,block,
                        PUT_,rVAR_DATA_,sx->data.vptr,NULL_);

         
          break;
        case DATAFLOAT:
          status=CDFlib(SELECT_,rVAR_,varid,
                     SELECT_,rVARs_RECNUMBER_,block,
                     PUT_,rVAR_DATA_,sx->data.vptr,NULL_);

          break;
        case DATADOUBLE:
           status=CDFlib(SELECT_,rVAR_,varid,
                     SELECT_,rVARs_RECNUMBER_,block,
                     PUT_,rVAR_DATA_,sx->data.vptr,NULL_);

          break;
        case DATASTRING:
          status=CDFlib(SELECT_,rVAR_,varid,GET_,rVAR_NUMELEMS_,&nume,NULL_);
          if (status !=CDF_OK) break;
          buf=malloc(nume);
          if (buf==NULL) break;
          memset(buf,0,nume);
          if (*((char **) sx->data.vptr) !=NULL) 
            strcpy(buf,*((char **) sx->data.vptr));


         
          status=CDFlib(SELECT_,rVAR_,varid,
                     SELECT_,rVARs_RECNUMBER_,block,
                     PUT_,rVAR_DATA_,buf,NULL_);

	  free(buf);
          break;
	}
        if (status !=CDF_OK) {
          fprintf(stderr,"Error writing CDF file (%d).\n",(int) status);
          exit(-1);
        }
       
      }
    }
    
    for (c=0;c<ptr->anum;c++) {
      ax=ptr->arr[c];
      for (n=0;n<anum;n++) {
        ay=aptr[n];
      
        if (strcmp(ax->name,ay->name) !=0) continue;
        if (ax->type !=ay->type) continue;
        if (ax->dim !=ay->dim) continue;
        break;
      }
      if (n !=anum) { 
      
        status=CDFlib(GET_,zVAR_NUMBER_,cdfaname[n],&varid,NULL_);
        if (status !=CDF_OK) {
          fprintf(stderr,"Error accessing CDF file.\n");
          exit(-1);
        }

        sze=1;
        for (x=0;x<ax->dim;x++) {
          indices[x]=0;
          intervals[x]=1;
          counts[x]=ax->rng[x];
          sze=sze*ax->rng[x];
	}   
        if (sze==0) continue;

	if (ax->type==DATASTRING) {
	  status=CDFlib(SELECT_,zVAR_,varid,GET_,zVAR_NUMELEMS_,&nume,NULL_);
          if (status !=CDF_OK) break;
          buf=malloc(nume*sze);
          if (buf==NULL) break;
          memset(buf,0,sze*nume);
	  strptr=(char **) ax->data.vptr;
          for (x=0;x<sze;x++) strncpy(buf+x*nume,strptr[x],nume);
	}


        switch (ax->type) { 
        case DATACHAR:
	  status=CDFlib(SELECT_,
                              zVAR_,varid,zVAR_RECNUMBER_,block,
                              zVAR_RECCOUNT_,1,
			      zVAR_RECINTERVAL_,1,
                              zVAR_DIMINDICES_,indices,
                              zVAR_DIMCOUNTS_,counts,
                              zVAR_DIMINTERVALS_,intervals, 
                              PUT_,zVAR_HYPERDATA_,ax->data.vptr,NULL_);
          break;
        case DATASHORT:
          status=CDFlib(SELECT_,
                              zVAR_,varid,zVAR_RECNUMBER_,block,
                              zVAR_RECCOUNT_,1,
			      zVAR_RECINTERVAL_,1,
                              zVAR_DIMINDICES_,indices,
                              zVAR_DIMCOUNTS_,counts,
                              zVAR_DIMINTERVALS_,intervals, 
                              PUT_,zVAR_HYPERDATA_,ax->data.vptr,NULL_);
    
           break;
        case DATAINT:
	   status=CDFlib(SELECT_,
                              zVAR_,varid,zVAR_RECNUMBER_,block,
                              zVAR_RECCOUNT_,1,
			      zVAR_RECINTERVAL_,1,
                              zVAR_DIMINDICES_,indices,
                              zVAR_DIMCOUNTS_,counts,
                              zVAR_DIMINTERVALS_,intervals, 
                              PUT_,zVAR_HYPERDATA_,ax->data.vptr,NULL_);       
           break;
        case DATAFLOAT:
                status=CDFlib(SELECT_,
                              zVAR_,varid,zVAR_RECNUMBER_,block,
                              zVAR_RECCOUNT_,1,
			      zVAR_RECINTERVAL_,1,
                              zVAR_DIMINDICES_,indices,
                              zVAR_DIMCOUNTS_,counts,
                              zVAR_DIMINTERVALS_,intervals, 
                              PUT_,zVAR_HYPERDATA_,ax->data.vptr,NULL_);
 	 
           break;
        case DATADOUBLE:
	    status=CDFlib(SELECT_,
                              zVAR_,varid,zVAR_RECNUMBER_,block,
                              zVAR_RECCOUNT_,1,
			      zVAR_RECINTERVAL_,1,
                              zVAR_DIMINDICES_,indices,
                              zVAR_DIMCOUNTS_,counts,
                              zVAR_DIMINTERVALS_,intervals, 
                              PUT_,zVAR_HYPERDATA_,ax->data.vptr,NULL_);
           break;
        case DATASTRING:
	  status=CDFlib(SELECT_,
			zVAR_,varid,zVAR_RECNUMBER_,block,
			zVAR_RECCOUNT_,1,
			zVAR_RECINTERVAL_,1,
			zVAR_DIMINDICES_,indices,
			zVAR_DIMCOUNTS_,counts,
			zVAR_DIMINTERVALS_,intervals,
			PUT_,zVAR_HYPERDATA_,buf,NULL_);

	   free(buf);


	   break;
	}
       

        if ((status !=CDF_OK) && (status !=VIRTUAL_RECORD_DATA)) {
          fprintf(stderr,"Error writing CDF file (%d).\n",(int) status);
          exit(-1);
        }


 
      }
    }
    

    DataMapFree(ptr);
    block++;
  }
  CDFclose(id);
  fclose(fp);
  return 0;
}
