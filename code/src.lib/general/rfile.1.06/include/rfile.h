/* rfile.h
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

/*
 $Log: rfile.h,v $
 Revision 1.3  2005/04/04 18:51:48  barnes
 Removed redundant function prototype.

 Revision 1.2  2005/04/04 18:50:39  barnes
 Added missing function prototypes.

 Revision 1.1  2003/03/20 23:43:39  barnes
 Initial revision

*/


#ifndef _RFILE_H
#define _RFILE_H

#define LMAX 4096

#define FIELD_TXT 0
#define FIELD_CHAR 1
#define FIELD_DOUBLE 2
#define FIELD_FLOAT 3

#define FILE_STEP 64

struct RfileIndex {
  double st_time,ed_time;
  int num;
  struct {
    double *start;
    double *end;
  } time;
  int *offset;
};


struct RfileData {
  int type;
  union {
    char *tval;
    char cval;
    float fval;
    double dval;
    int32 ival;
    int16 sval;
    int32 lval;
  } data;
};


typedef int (*RfileDecoder)(char *,char *,char *,double,double,int,int,int,
                            struct RfileData *,void *ptr);


int RfileRead(FILE *fp,int num,RfileDecoder *decode,void **ptr);
struct RfileIndex *RfileLoadIndex(FILE *fp);

int RfileSeek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,double sc,
              struct RfileIndex *inx,
              double *aval);



int RfileReadRaw(FILE *fp,int num,RfileDecoder *decode,void **ptr);



#endif





