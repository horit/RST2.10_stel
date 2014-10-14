/* make_grid.c
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
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"

/*
 $Log: make_grid.c,v $
 Revision 1.15  2004/10/08 19:10:03  barnes
 Fixed bug in handling extended files.

 Revision 1.14  2004/10/08 16:39:50  barnes
 Changed griddata structure.

 Revision 1.13  2004/06/21 22:51:38  barnes
 Added missing header.

 Revision 1.12  2004/05/10 21:32:56  barnes
 Removed redundant code.

 Revision 1.11  2003/02/25 22:40:21  barnes
 Modification to use the base libraries.

 Revision 1.10  2002/07/19 18:12:32  barnes
 Fixed bug in one comparison.

 Revision 1.9  2002/07/18 17:40:53  barnes
 Fixed bug in dealing with stereo files.

 Revision 1.8  2002/02/06 23:02:56  barnes
 Modification to deal with channel letter in the header.

 Revision 1.7  2001/06/27 21:06:53  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:11:57  barnes
 Added Author Name

 Revision 1.5  2000/04/04 18:46:30  barnes
 Fixed nasty bug that meant when combining files containing more
 than one radar, some vectors were duplicated.

 Revision 1.4  1999/10/12 21:07:18  barnes
 Added support for the extended grid format.

 Revision 1.3  1999/10/06 19:20:10  barnes
 Added missing headers.

 Revision 1.2  1999/10/04 19:32:36  barnes
 Modifications to use the new grid format.

 Revision 1.1  1998/07/30 14:45:17  barnes
 Initial revision


 */


int make_grid(struct GridData *out,
	      struct GridData *in,int fnum,int rflg,int *dflg) {

  int i,j,k,l=0;
  int tpnt=0;
  struct GridGVec *data[20];
  int cnt[20];
  out->stnum=0;
  out->vcnum=0;
  out->xtd=0;


  l=0;
  for (i=0;i<fnum;i++) 
    if ( (dflg[i] !=0) && 
         (in[i].st_time<out->ed_time) && 
	 (in[i].ed_time>out->st_time)) { 
      if (in[i].xtd) out->xtd=1;
     for (j=0;j<in[i].stnum;j++) {    
       for (k=0;(k<out->stnum) && (in[i].sdata[j].st_id !=
                                     out->sdata[k].st_id) && 
                                    (in[i].sdata[j].chn != 
                                     out->sdata[k].chn);k++);
       if (k<out->stnum) {
	 if (rflg !=0) {
            out->sdata[k].st_id=in[i].sdata[j].st_id;
            out->sdata[k].chn=in[i].sdata[j].chn;

            out->sdata[k].npnt=in[i].sdata[j].npnt;
            out->sdata[k].freq0=in[i].sdata[j].freq0;
            out->sdata[k].major_revision=in[i].sdata[j].major_revision;
            out->sdata[k].minor_revision=in[i].sdata[j].minor_revision;
            out->sdata[k].prog_id=in[i].sdata[j].prog_id;
            out->sdata[k].noise.mean=in[i].sdata[j].noise.mean;
            out->sdata[k].noise.sd=in[i].sdata[j].noise.sd;
            out->sdata[k].gsct=in[i].sdata[j].gsct;
            out->sdata[k].vel.min=in[i].sdata[j].vel.min;
            out->sdata[k].vel.max=in[i].sdata[j].vel.max;
            out->sdata[k].pwr.min=in[i].sdata[j].pwr.min;
            out->sdata[k].pwr.max=in[i].sdata[j].pwr.max;
            out->sdata[k].wdt.min=in[i].sdata[j].wdt.min;
            out->sdata[k].wdt.max=in[i].sdata[j].wdt.max;
            out->sdata[k].verr.min=in[i].sdata[j].verr.min;
            out->sdata[k].verr.max=in[i].sdata[j].verr.max;
            data[k]=in[i].data;
            cnt[k]=in[i].vcnum;
         }
         continue;
      }
       if (out->sdata !=NULL) out->sdata=realloc(out->sdata,
                                      sizeof(struct GridSVec)*(l+1));
       else out->sdata=malloc(sizeof(struct GridSVec));

     
       out->sdata[l].st_id=in[i].sdata[j].st_id;
       out->sdata[l].chn=in[i].sdata[j].chn;
       out->sdata[l].npnt=in[i].sdata[j].npnt;
       out->sdata[l].freq0=in[i].sdata[j].freq0;
       out->sdata[l].major_revision=in[i].sdata[j].major_revision;
       out->sdata[l].minor_revision=in[i].sdata[j].minor_revision;
       out->sdata[l].prog_id=in[i].sdata[j].prog_id;
       out->sdata[l].noise.mean=in[i].sdata[j].noise.mean;
       out->sdata[l].noise.sd=in[i].sdata[j].noise.sd;
       out->sdata[l].gsct=in[i].sdata[j].gsct;
       out->sdata[l].vel.min=in[i].sdata[j].vel.min;
       out->sdata[l].vel.max=in[i].sdata[j].vel.max;
       out->sdata[l].pwr.min=in[i].sdata[j].pwr.min;
       out->sdata[l].pwr.max=in[i].sdata[j].pwr.max;
       out->sdata[l].wdt.min=in[i].sdata[j].wdt.min;
       out->sdata[l].wdt.max=in[i].sdata[j].wdt.max;
       out->sdata[l].verr.min=in[i].sdata[j].verr.min;
       out->sdata[l].verr.max=in[i].sdata[j].verr.max;


       data[l]=in[i].data;     
       cnt[l]=in[i].vcnum;
 
       l++;
     }
    }
   
    tpnt=0;
    for (i=0;i<l;i++) tpnt+=out->sdata[i].npnt;
    out->stnum=l;
    out->vcnum=tpnt;
    if (out->data !=NULL) 
      out->data=realloc(out->data,sizeof(struct GridGVec)*tpnt);    
    else out->data=malloc(sizeof(struct GridGVec)*tpnt);

    k=0;
    for (i=0;i<l;i++) {
      for (j=0;j<cnt[i];j++) {
        if (data[i][j].st_id !=out->sdata[i].st_id) continue;
        if (data[i][j].chn !=out->sdata[i].chn) continue;
        memcpy(&out->data[k],&data[i][j],sizeof(struct GridGVec));
        k++;
      }
    } 

  return l;
}
    
    





