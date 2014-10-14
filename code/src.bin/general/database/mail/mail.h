/* change.h
   ======== */


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
 $Log$
*/


struct map {
  int type;
  char *search;
  char *replace;
  struct XMLDBbuffer *sep;
  struct XMLDBbuffer *mod;
  struct XMLDBbuffer *def;
};


struct post {
  int type;
  char *name;
  char *tag;
};


struct config {
  char *top;
  char *store;
  char *path;
  char *query;
  char *mail;
  char *mailc;

  struct {
    char *path;
    char delim;
  } tree;

  struct {
    int num;
    struct post **ptr;
  } post;


  struct {
    int num;
    char **mail;
  } mlist;


  struct {
    struct {
      int type;
      struct XMLDBbuffer *buf;
    } mail;

    struct {
      int type;
      struct XMLDBbuffer *buf;
    } mailmail;

  } sfname;

  struct {
    int num;
    struct map **ptr;
  } map;

  char *ok;
  char *fail;

  struct {
    char *name;
    int num;
    char **tag;
  } display;
 
};



