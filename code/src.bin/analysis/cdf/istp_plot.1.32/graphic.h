/* graphic.h
   ======
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

/*
 $Log: graphic.h,v $
 Revision 1.1  2004/07/13 22:41:47  barnes
 Initial revision

*/


int txtbox(char *fntname,float sze,int num,char *txt,float *box,void *data);

void plot_frame(struct PlotData *plt,int pmax,double stime,
                double etime,int dflg,
                unsigned int color,float width,char *fontname,
                float fontsize);

void plot_pos(struct PlotData *plt,int p,
              struct posdata *pos,double stime,double etime,int mode,
              unsigned int color,float width,char *fontname,
              float fontsize);

void plot_plasma(struct PlotData *plt,int p,struct plasmadata *plasma,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
	      float fontsize);

void plot_imf(struct PlotData *plt,int p,struct imfdata *imf,
              double stime,double etime,int mode,int *pflg,
              unsigned int color,float width,char *fontname,
              float fontsize);

void plot_title(struct PlotData *plt,double stime,double etime,
                char *sat,char *plot,int mode,
                unsigned int color,float width,char *fontname,
                float fontsize);








