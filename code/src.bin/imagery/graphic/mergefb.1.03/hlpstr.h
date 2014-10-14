/* hlpstr.h
   ========
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


char *hlpstr[]={
"mergefb - Merge two bitmaps.\n",
"mergefb --help\n",
"mergefb -ppm [options] backimage frontimage\n",
"mergefb -ppmx [options] backimage frontimage\n",
"mergefb -xml [options] backimage frontimage\n",

"--help\tprint the help message and exit.\n",
"-smooth\tApply a simple linear interpolation to smooth the images.\n",
"-tf factor\tSet the transparency factor to factor. The transparency factor controls how the foreground image is blended with the background. A ratio is calculated using the equation (1-factor)+c*factor, where c is the value of the alpha channel of the pixel in the foreground image. This ratio is then used to blend the foreground image with the background.\n",
"-add\tDo not blend the foreground image with the background do a straight addition.\n",
"-fd fade\tSet the fading factor to fade. The fading factor determines the proportion of the foreground image that is merged with a background. A value of zero effectively gives the background image and a value of one gives both images. The factor can be used to 'fade' in the foreground image for animations.\n",
"backimage\tfilename of the background image.\n",
"frontimage\tfilename of the foreground image.\n",
"-bppm\tthe background image file is a Portable PixMap (PPM) image.\n",
"-bppmx\tthe background image file is an extended Portable PixMap (PPMX) image.\n",
"-bxml\tthe background image file is an XML image representation.\n",
"-fppm\tthe foreground image file is a Portable PixMap (PPM) image.\n",
"-fppmx\tthe foreground image file is an extended Portable PixMap (PPMX) image.\n",
"-fxml\tthe foreground image file is an XML image representation.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",

NULL};
