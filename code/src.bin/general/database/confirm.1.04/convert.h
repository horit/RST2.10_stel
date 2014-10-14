/* convert.h
   ========= */


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
 $Log: convert.h,v $
 Revision 1.3  2005/03/22 15:15:58  barnes
 Fixed function definition for ApplyModifier.

 Revision 1.2  2005/03/22 15:00:17  barnes
 Fixed the modifier syntax to agree with show.cgi.

 Revision 1.1  2004/07/12 22:50:43  barnes
 Initial revision

*/

char *XMLToHTML(struct XMLDBbuffer *src);

char *XMLToRaw(struct XMLDBbuffer *src);
  
char *XMLToURL(struct XMLDBbuffer *src);
 
struct XMLDBbuffer *RawToXML(char *buf,int sze);
 
char *RawToHTML(char *buf,int sze);

struct XMLDBbuffer *ApplyModifier(struct XMLDBbuffer *mod,char *name,
                                  struct XMLDBbuffer *txt);

void DoReplace(char **replace,char *name,struct XMLDBbuffer *buf,
               struct XMLDBbuffer *mod,char *sep,int type);
 
