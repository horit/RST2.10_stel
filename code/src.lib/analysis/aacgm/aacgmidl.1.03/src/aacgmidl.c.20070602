/* aacgmidl.c
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
#include <sys/types.h>
#include <unistd.h>
#include "idl_export.h"
#include "rtypes.h"
#include "aacgm.h"

/*
 $Log: aacgmidl.c,v $
 Revision 1.3  2004/08/09 20:45:15  barnes
 Implemented new function to read co-efficients from an open file.

 Revision 1.2  2004/06/16 21:44:04  barnes
 Added missing headers.

 Revision 1.1  2004/06/15 21:47:40  barnes
 Initial revision

*/

int AACGMIDLConvert(int argc,void *argv[]) {
  
  int s=0;
  float *inpos;
  float *in_lat;
  float *in_lon;
  float *height;
  float *outpos;
  float *out_latf;
  float *out_lonf;
  float *out_rf;

  double out_lat,out_lon,radius;

  int16 *mgflag;
  int16 *model;


  if (argc < 3) return -1;
  
  inpos  = (float *) argv[0];
  outpos = (float *) argv[1];
  mgflag = (int16 *) argv[2];


  if (argc >= 4) model  = (int16 *) argv[3];
  else model = NULL;
  if (inpos == NULL || outpos == NULL || mgflag == NULL) return -1;
 
  in_lat  = inpos;
  in_lon = (inpos+1);
  height  = (inpos+2);

  out_latf  = outpos;
  out_lonf = (outpos+1);
  out_rf    = (outpos+2);



  if (model !=NULL) AACGMInit(*model);
    
  s=AACGMConvert( (double) *in_lat,(double) *in_lon,(double) *height,
              &out_lat,&out_lon,&radius,
              (int) *mgflag);
  *out_latf=(float) out_lat;
  *out_lonf=(float) out_lon;
  *out_rf=(float) radius;
  if (s !=0) return -1;
  return 0;

}





int32 AACGMIDLLoadCoef(int argc,char *argv[]) {
  IDL_FILE_STAT stat;
  FILE *fp;
  int unit; 
  int s;
  
  unit = *( (int32 *) argv[0]);


  /* Make sure that the "STDIO" keyword was used to
   * open the file in IDL.
   */

  s=IDL_FileEnsureStatus(IDL_MSG_RET,unit,IDL_EFS_STDIO);
  if (s==FALSE) return -1;

  /* Get information about the file */

  IDL_FileStat(unit,&stat);
 
  /* Find the file pointer */

  fp=stat.fptr; 
  
  if (fp==NULL) return -1;

  s=AACGMLoadCoefFP(fp);
  return s;
}
