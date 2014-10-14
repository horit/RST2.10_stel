/* rcgi.h
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
 $Log: rcgi.h,v $
 Revision 1.1  2003/03/24 23:45:19  barnes
 Initial revision

*/


#ifndef _RCGI_H
#define _RCGI_H

struct CGIarg {
  char *name;
  char *fname;
  int type;
  struct {
    struct {
      int sze;
      char *buf;
    } raw;
    struct {
      int sze;
      char *buf;
    } txt;
  } value;
};

struct CGIdata {
  int num;
  char *buf;
  int sze;
  int stp;
  int max;
  int state;
  int cnt; 
  char *bnd;

  struct {
    int (*user)(struct CGIarg *,char *,int,void *);
    void *data;
  } text;

  struct {
    int state;
    int sze;
    char *buf;
    int stp;
    int max;
  } mime;

  struct CGIarg **arg;
};

char *CGIGetString(struct CGIarg *);
char *CGIGetRawString(struct CGIarg *);
char *CGIMakeURLString(char *inp,int sze);

struct CGIdata *CGIMake();
void CGIFree(struct CGIdata *ptr);
int CGISetText(struct CGIdata *ptr,int (*text)(struct CGIarg *,char *,
					       int,void *),void *data);
int CGIFindInBuf(char *buf,int sze,char *str);
int CGIGet(struct CGIdata *ptr);
 
#endif
