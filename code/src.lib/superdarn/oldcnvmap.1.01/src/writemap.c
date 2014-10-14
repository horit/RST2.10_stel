/* writemap.c
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
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "oldgridwrite.h"
#include "cnvmap.h"

/*
 $Log: writemap.c,v $
 Revision 1.1  2004/10/08 20:50:52  barnes
 Initial revision

 Revision 1.1  2004/08/20 13:35:34  barnes
 Initial revision

*/

#define NBLOCKS 12


int OldCnvMapEncodeIMF(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"Bx","By","Bz",0};
  char *unit[]={"nT","nT","nT",0};
  char *type[]={"float","float","float",0};

  fprintf(fp,"%d %d\n",1,3);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  fprintf(fp,"%#10g %#10g %#10g\n",map->Bx,map->By,map->Bz);
  return 0;
}

int OldCnvMapEncodeMinfo(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"doping_level","imf_flag","imf_delay",
                "mod_dir","mod_mag",0};
  char *unit[]={"count","count","minute","name","name",0};
  char *type[]={"int","int","int","text","text",0};


  fprintf(fp,"%d %d\n",1,5);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");



  fprintf(fp,"%10d %10d %10d ",
	  map->doping_level,map->imf_flag,
	  map->imf_delay);
   if (strlen(map->imf_model[0])==0) fprintf(fp,"%10s ","Null");
   else fprintf(fp,"%10s ",map->imf_model[0]);
   if (strlen(map->imf_model[1])==0) fprintf(fp,"%10s\n","Null");
   else fprintf(fp,"%10s\n",map->imf_model[1]);
   return 0;
}

int OldCnvMapEncodeFinfo(FILE *fp,struct CnvMapData *map) {
  int i;
  
  char *name[]={"lon_shft","lat_shft","hemisphere","order","latmin",
                "error_wt","model_wt",0};
    
  char *unit[]={"degrees","degrees","count","count","degrees","count","count",
		0};

  char *type[]={"float","float","int","int","float","int","int",0};

  fprintf(fp,"%d %d\n",1,7);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");


  fprintf(fp,"%#10g %#10g ",map->lon_shft,map->lat_shft);
  fprintf(fp,"%10d ",map->hemisphere);
   
  fprintf(fp,"%10d %#10g %10d %10d\n",
              map->fit_order,map->latmin,map->error_wt,map->model_wt);
  return 0;
}


int OldCnvMapEncodeMLT(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"mlt_start","mlt_end","mlt_avg",0};
  char *unit[]={"dec_hour","dec_hour","dec_hour",0};
  char *type[]={"float","float","float",0};

  fprintf(fp,"%d %d\n",1,3);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  fprintf(fp,"%#10g %#10g %#10g\n",map->mlt.start,map->mlt.end,
	  map->mlt.av);
 return 0;
}



int OldCnvMapEncodeStat(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"chi_sqr","chi_sqr_dat","rms_err",0};

  char *unit[]={"kv","kv","kv",0};
  char *type[]={"float","float","float",0};

  fprintf(fp,"%d %d\n",1,3);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  fprintf(fp,"%#10g %#10g %#10g\n",map->chi_sqr,map->chi_sqr_dat,
                               map->rms_err);	
 
  return 0;
}

int OldCnvMapEncodePot(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"pot","pot_err",0};

  char *unit[]={"kv","kv",0};
  char *type[]={"float","float",0};

  fprintf(fp,"%d %d\n",3,2);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");
	
  fprintf(fp,"%#10g %#10g\n",map->pot_drop,map->pot_drop_err);
  fprintf(fp,"%#10g %#10g\n",map->pot_max,map->pot_max_err);
  fprintf(fp,"%#10g %#10g\n",map->pot_min,map->pot_min_err);
  return 0;
}




int OldCnvMapEncodeCoef(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"N","N+1","N+2","N+3",0};
  char *unit[]={"none","none","none","none",0};
  char *type[]={"float","float","float","float",0};
 
  fprintf(fp,"%d %d\n",map->num_coef,4);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<map->num_coef;i++) 
    fprintf(fp,"%#10g %#10g %#10g %#10g\n",
	    map->coef[i*4],map->coef[i*4+1],
            map->coef[i*4+2],map->coef[i*4+3]);

  return 0;
}

int OldCnvMapEncodeModel(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"gmlong","gmlat","kvect","vlos",0};
  char *unit[]={"degrees","degrees","degrees","m/sec",0};
  char *type[]={"float","float","float","float",0};
 
  fprintf(fp,"%d %d\n",map->num_model,4);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  for (i=0;i<map->num_model;i++) 
    fprintf(fp,"%#10g %#10g %#10g %#10g\n",
	        map->model[i].mlon,map->model[i].mlat,
	        map->model[i].azm,map->model[i].vel.median);
  return 0;
}

int OldCnvMapEncodeBnd(FILE *fp,struct CnvMapData *map) {

  int i;  
  char *name[]={"bnd_lon","bnd_lat",0};
  char *unit[]={"degrees","degrees",0};
  char *type[]={"float","float",0};

  fprintf(fp,"%d %d\n",map->num_bnd,2);
 
  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");
  
  for (i=0;i<map->num_bnd;i++) fprintf(fp,"%#10g %#10g\n",
                                       map->bnd_lon[i],map->bnd_lat[i]);
  return 0; 
}



int OldCnvMapEncodeVersion(FILE *fp,struct CnvMapData *map) {
  int i;
  char *name[]={"source","major_rev","minor_rev",0};

  char *unit[]={"name","count","count",0};
  char *type[]={"text","int","int",0};

  fprintf(fp,"%d %d\n",1,3);

  for (i=0;name[i] !=0;i++) fprintf(fp,"%10s ",name[i]);
  fprintf(fp,"\n");
  
  for (i=0;unit[i] !=0;i++) fprintf(fp,"%10s ",unit[i]);   
  fprintf(fp,"\n");
  
  for (i=0;type[i] !=0;i++) fprintf(fp,"%10s ",type[i]);
  fprintf(fp,"\n");

  if (strlen(map->source) !=0) 
    fprintf(fp,"%10s %10d %10d\n",map->source,map->major_rev,
                               map->minor_rev);	
  else fprintf(fp,"%10s %10d %10d\n","Null",map->major_rev,
                               map->minor_rev);	
  return 0;
}


int OldCnvMapFwrite(FILE *fp,struct CnvMapData *map,struct GridData *grd) {

  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  double sec;
 
  TimeEpochToYMDHMS(map->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
  ssc=(int) sec;
  TimeEpochToYMDHMS(map->ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
  esc=(int) sec;

  fprintf(fp,"%d %d %d %d %d %d ",syr,smo,sdy,shr,smt,ssc);
  fprintf(fp,"%d %d %d %d %d %d\n",eyr,emo,edy,ehr,emt,esc);
  fprintf(fp,"%d\n",NBLOCKS);


  /* write model and data indices */
  OldCnvMapEncodeVersion(fp,map);

  OldCnvMapEncodeIMF(fp,map);
  OldCnvMapEncodeMinfo(fp,map); 
  OldCnvMapEncodeFinfo(fp,map);

  OldCnvMapEncodeCoef(fp,map);
  OldCnvMapEncodeStat(fp,map); 
  OldCnvMapEncodePot(fp,map); 

  OldCnvMapEncodeMLT(fp,map);
 
 
  OldCnvMapEncodeModel(fp,map);
  OldCnvMapEncodeBnd(fp,map);

  OldGridEncodeOne(fp,grd);
  if (grd->xtd==0) OldGridEncodeTwo(fp,grd);
  else OldGridEncodeThree(fp,grd);

  return 0;
}















