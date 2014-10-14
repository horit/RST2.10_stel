/* xwin.h
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
 $Log: xwin.h,v $
 Revision 1.2  2004/04/26 21:34:01  barnes
 Fixed bug in defining a function prototype.

 Revision 1.1  2003/03/01 18:15:30  barnes
 Initial revision

*/


#ifndef _XWIN_H
#define _XWIN_H

struct XwinDisplay {
  Display *display;
  int screen_num;
  Visual *default_visual;
  int default_depth; 
  int wdt,hgt;
}; 


struct XwinWindow {
  int wdt,hgt;
  char *progname;    
  struct XwinDisplay *dp;
  Window win;
  XTextProperty windowName,iconName;
  XSizeHints *size_hints;
  XWMHints *wm_hints;
  XClassHint *class_hints;

  char *buffer;
  XImage *xi;
  GC gc;

  struct  {
    Atom proto;
    Atom delete;
  } atom;

};

int XwinCloseDisplay(struct XwinDisplay *ptr);
int XwinFreeWindow(struct XwinWindow *ptr);
struct XwinDisplay *XwinOpenDisplay(char *display_name,int *flg);
struct XwinWindow *XwinMakeWindow(int x,int y,int wdt,int hgt,int border,
                           struct XwinDisplay *display,
                           char *window_name,char *icon_name,
                           char *res_name,char *res_class,int argc,
                           char *argv[],
                           int *flg); 

int XwinFrameBufferToXimage(struct FrameBuffer *ptr,XImage *xi);
int XwinFrameBufferWindow(struct FrameBuffer *ptr,struct XwinWindow *win);

int XwinShowWindow(struct XwinWindow *win);
int XwinDisplayEvent(struct XwinDisplay *display,int wnum,
                   struct XwinWindow **win,int mode,struct timeval *tmout);

#endif
 

