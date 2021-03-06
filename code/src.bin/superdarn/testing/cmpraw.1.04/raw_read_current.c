/* raw_read_current.c
   ==================
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
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "lmt.h"
#include "raw.h"
#include "raw_read.h"

/*
 $Log: raw_read_current.c,v $
 Revision 1.2  2004/06/22 18:47:13  barnes
 Replaced references of integers with "int".

 Revision 1.1  2004/05/10 21:04:37  barnes
 Initial revision

*/

int32 dcmpr(unsigned char *word) {
  /* decompress value */
  int count;
  int32 value;
 
  
  count=word[0] & 0x0f;
  value=(word[1] & 0x7f)<<8 | (word[0] & 0xf0); 
  if (count==0) value=value<<1;
  else {
     value=value | 0x8000;
     value=value<<count;
  }
  if ((word[1] & 0x80) !=0) value=-value; 
  return (value);  
}

int raw_read_current(struct rawfp *fp,struct rawdata *raw_data) { 

   /* read raw data block from file */


   int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
   int i;
   int16 range;
   int j;
   int16 prev_range;
   int xcf_data;
   unsigned int stat;
   int16 num_byte;
   int32 rec_num=0;
   unsigned char *inbuf;
   unsigned char *inbuf_ptr;

   xcf_data = 0;
   prev_range = -1;

   inbuf=malloc(sizeof(struct rawdata));

   /* zero out the raw data buffer */
   memset(raw_data,0,sizeof(struct rawdata));
   fp->rlen=0;
   do {
     if (ConvertReadShort(fp->rawfp,&num_byte) !=0) {
       free(inbuf);
       return -1;
     }
     fp->rlen+=num_byte;
     fp->ptr+=num_byte;
     num_byte = num_byte - 2;
     stat = read(fp->rawfp,inbuf,num_byte);
     if(stat != num_byte) {
       free(inbuf);
       return -1; 
     }
     inbuf_ptr=inbuf;
     ConvertToInt(inbuf_ptr,&rec_num);
   } while (rec_num==0); 
   inbuf_ptr = inbuf_ptr + 12;  /* skip rec_num + rawwrite */  
   num_byte = num_byte - 12;
  
   /* zero out the raw data buffer */

   /* copy radar_parms */   
   ConvertBlock(inbuf_ptr,radar_parms_pat);
   memcpy((void *) &(raw_data->PARMS),inbuf_ptr,sizeof(struct radar_parms)); 
   inbuf_ptr = inbuf_ptr + sizeof(struct radar_parms);
   num_byte = num_byte - sizeof(struct radar_parms);

   /* copy the pulse pattern */
  
   for (i=0;i<raw_data->PARMS.MPPUL;i++) {
      ConvertToShort(inbuf_ptr,&raw_data->PULSE_PATTERN[i]);
      inbuf_ptr+=sizeof(int16);
      num_byte-=sizeof(int16);
   }

   /* copy the lag table */

    for(j=0;j < 2; ++j)
      for(i=0; i < raw_data->PARMS.MPLGS; ++i) {
         ConvertToShort(inbuf_ptr,&raw_data->LAG_TABLE[j][i]);
         inbuf_ptr = inbuf_ptr + sizeof(int16);
         num_byte = num_byte - sizeof(int16);
      } 
	  
   /* copy comment buffer */
   memcpy(raw_data->COMBF,inbuf_ptr,ORIG_COMBF_SIZE);
   inbuf_ptr = inbuf_ptr + ORIG_COMBF_SIZE;
   num_byte = num_byte - ORIG_COMBF_SIZE;

   /* decompress and copy the lag-0 powers */
   for(i=0 ; i < raw_data->PARMS.NRANG ; ++i) {
      raw_data->pwr0[i] = dcmpr(inbuf_ptr);
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);
   }

   /* decompress and copy acfs */
   while ( num_byte > 0 ) {
      ConvertToShort(inbuf_ptr,&range);
      --range;
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);

      if((range <= prev_range) && (raw_data->PARMS.XCF))
		   xcf_data = 1;

      for(i = 0; i < raw_data->PARMS.MPLGS ; ++i) {   
         for(j=0 ; j < 2; ++j) {
	    if (xcf_data) raw_data->xcfd[range][i][j] = dcmpr(inbuf_ptr);
	    else raw_data->acfd[range][i][j] = dcmpr(inbuf_ptr);
            inbuf_ptr = inbuf_ptr + sizeof(int16);
            num_byte = num_byte - sizeof(int16);
         }
      }
		
      prev_range = range;

   } 
  fp->ctime=TimeYMDHMSToEpoch(raw_data->PARMS.YEAR,
			raw_data->PARMS.MONTH,
			raw_data->PARMS.DAY,
			raw_data->PARMS.HOUR,
			raw_data->PARMS.MINUT,
			raw_data->PARMS.SEC);
  free(inbuf);  
  return 0;
}

