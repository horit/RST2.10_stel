/* make_palette.c
   ==============
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
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

/*
 $Log: make_palette.c,v $
 Revision 1.3  2004/04/26 21:15:09  barnes
 Audit - Enforced warning set to "all".

 Revision 1.2  2003/08/05 20:55:57  barnes
 Changed array names.

 Revision 1.1  2003/05/01 16:53:19  barnes
 Initial revision

*/

int out_reg[256][3];

int reg[320][3];
int sgn_inx[256];
int mag_inx[256];

int reg_map[320];

int root_mag[256][3];
int root_sgn[256][3];

int mag[256][3];
int sgn[256][3];

int sgn_map[256];
int mag_map[256];

int std_max=5;
int col_std[5][3]={
   {0x00,0x00,0x00},
   {0xff,0xff,0xff},
   {0x80,0x80,0x80},
   {0x60,0x60,0x60},
   {0x60,0x60,0x80}
};



void print_hdr(FILE *fp,char *name,int max) {
  fprintf(fp,"extern unsigned int %sMax;\n",name);
  fprintf(fp,"extern unsigned char %sR[2][%d];\n",name,max);
  fprintf(fp,"extern unsigned char %sG[2][%d];\n",name,max);
  fprintf(fp,"extern unsigned char %sB[2][%d];\n",name,max);
}

void print_chdr(FILE *fp,char *name,int max) {
  fprintf(fp,"extern unsigned int %sMax;\n",name);
  fprintf(fp,"extern unsigned char %s[2][%d];\n",name,max);
}


void print_reg_hdr(FILE *fp,char *name,int max) {
  fprintf(fp,"extern unsigned int %sMax;\n",name);
  fprintf(fp,"extern unsigned char %sR[%d];\n",name,max);
  fprintf(fp,"extern unsigned char %sG[%d];\n",name,max);
  fprintf(fp,"extern unsigned char %sB[%d];\n",name,max);

}

void print_map(FILE *fp,char *name,int max) {
  int i;
  fprintf(fp,"unsigned int %sMax=%d;\n",name,max);
  fprintf(fp,"unsigned char %sR[2][%d]={\n",name,max);
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",mag[i][0]);
  }
  fprintf(fp,"},\n");
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",sgn[i][0]);
  }
  fprintf(fp,"}\n};\n");

  fprintf(fp,"unsigned char %sG[2][%d]={\n",name,max);
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",mag[i][1]);
  }
  fprintf(fp,"},\n");
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",sgn[i][1]);
  }
  fprintf(fp,"}\n};\n");

  fprintf(fp,"unsigned char %sB[2][%d]={\n",name,max);
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",mag[i][2]);
  }
  fprintf(fp,"},\n");
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",sgn[i][2]);
  }
  fprintf(fp,"}\n};\n");
}

void print_cmap(FILE *fp,char *name,int max) {
  int i;
  fprintf(fp,"unsigned int %sMax=%d;\n",name,max);
  fprintf(fp,"unsigned char %s[2][%d]={\n",name,max);
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",mag_map[i]);
  }
  fprintf(fp,"},\n");
  fprintf(fp,"  {");
  for (i=0;i<max;i++) {
    if ((i % 9==0) && (i>0))  fprintf(fp,",\n   ");
    else if (i>0) fprintf(fp,", ");
    fprintf(fp,"0x%.2x",sgn_map[i]);
  }
  fprintf(fp,"}\n};\n");
}

void make_reg() {
  int j=0,i;
  for (i=0;i<64;i++) { /* cyan-white */
    reg[j][0]=255-255*pow(cos(i/64.0*0.5*PI),2);
    reg[j][1]=255;
    reg[j][2]=255;
    j++;
  }
  for (i=0;i<64;i++) { /* blue-cyan */
    reg[j][0]=0;
    reg[j][1]=255-255*pow(cos(i/64.0*0.5*PI),2);
    reg[j][2]=255;
    j++;
  }
  for (i=0;i<64;i++) { /* cyan-green */
    reg[j][0]=0;
    reg[j][1]=255;
    reg[j][2]=255*pow(cos(i/64.0*0.5*PI),1);
    j++;
  }
  for (i=0;i<64;i++) { /* green-yellow */
    reg[j][0]=255-255*pow(cos(i/64.0*0.5*PI),4);
    reg[j][1]=255;
    reg[j][2]=0;
    j++;
  }
  for (i=0;i<64;i++) { /* yellow-red */
    reg[j][0]=255;
    reg[j][1]=255*pow(cos(i/64.0*0.5*PI),2);
    reg[j][2]=0;
    j++;
  }
  for (i=0;i<j;i++) {
    reg[i][0]=reg[i][0];
    reg[i][1]=reg[i][1];
    reg[i][2]=reg[i][2];
  }

}

void make_root() {
  int i;
  for (i=0;i<64;i++) { /* green-yellow, blue-cyan */
    root_sgn[i][0]=reg[192+i][0];
    root_sgn[i][1]=reg[192+i][1];
    root_sgn[i][2]=reg[192+i][2];
    root_mag[i][0]=reg[64+i][0];
    root_mag[i][1]=reg[64+i][1];
    root_mag[i][2]=reg[64+i][2];
    sgn_inx[i]=192+i;  
    mag_inx[i]=64+i;
  }
  for (i=0;i<64;i++) { /* yellow-red, cyan-green */ 
    root_sgn[i+64][0]=reg[256+i][0];
    root_sgn[i+64][1]=reg[256+i][1];
    root_sgn[i+64][2]=reg[256+i][2];
    root_mag[i+64][0]=reg[128+i][0];
    root_mag[i+64][1]=reg[128+i][1];
    root_mag[i+64][2]=reg[128+i][2];
    sgn_inx[i+64]=256+i;  
    mag_inx[i+64]=128+i;
  }   
  for (i=0;i<64;i++) { /* blue-cyan, green yellow */ 
    root_sgn[i+128][0]=reg[64+i][0];
    root_sgn[i+128][1]=reg[64+i][1];
    root_sgn[i+128][2]=reg[64+i][2];
    root_mag[i+128][0]=reg[192+i][0];
    root_mag[i+128][1]=reg[192+i][1];
    root_mag[i+128][2]=reg[192+i][2];
    sgn_inx[i+128]=64+i;  
    mag_inx[i+128]=192+i;

  } 
  for (i=0;i<64;i++) { /* cyan-white, yellow to red */
    root_sgn[i+192][0]=reg[i][0];
    root_sgn[i+192][1]=reg[i][1];
    root_sgn[i+192][2]=reg[i][2];
    root_mag[i+192][0]=reg[256+i][0];
    root_mag[i+192][1]=reg[256+i][1];
    root_mag[i+192][2]=reg[256+i][2];
    sgn_inx[i+192]=i;  
    mag_inx[i+192]=256+i;
  }     
}      

void make_map(int max) {
  int i,j,k;
  
  for (i=0;i<max;i++) {
    j=255*1.0*i/(max-1);
    for (k=0;k<3;k++) {
      mag[i][k]=root_mag[j][k];    
      sgn[i][k]=root_sgn[j][k];
    }
  }
  
  if (max !=256) {
    for (i=0;i<max;i++) {
      for (k=0;k<3;k++) {
        if ((i % 2)==0) {
          mag[i][k]=mag[i][k]-0x0f;    
          sgn[i][k]=sgn[i][k]-0x0f;
	} else {
          mag[i][k]=mag[i][k]+0x0f;    
          sgn[i][k]=sgn[i][k]+0x0f;
	}
        if (mag[i][k]>255) mag[i][k]=255;
        if (sgn[i][k]>255) sgn[i][k]=255;
        if (mag[i][k]<0) mag[i][k]=0;
        if (sgn[i][k]<0) sgn[i][k]=0;

      }
    }
  }
  
}

void make_cmap(int max) {
  int i,j;
  for (i=0;i<max;i++) {
    j=255*1.0*i/(max-1);
    sgn_map[i]=reg_map[sgn_inx[j]];    
    mag_map[i]=reg_map[mag_inx[j]];
  }
}


void make_out_reg(int max,int start,int end) {
  float scale;
  int p,c;

  for (p=0;p<max;p++) {
    out_reg[p][0]=0;
    out_reg[p][1]=0;
    out_reg[p][2]=0;
  }

  for (c=0;c<320;c++) {
    scale=(float) c/320;
    p=start+scale*(end-start);
    reg_map[c]=p;
    out_reg[p][0]=reg[c][0];
    out_reg[p][1]=reg[c][1];
    out_reg[p][2]=reg[c][2];
  }

  for (p=0;p<std_max;p++) {
    out_reg[p][0]=col_std[p][0];
    out_reg[p][1]=col_std[p][1];
    out_reg[p][2]=col_std[p][2];
  }
}

void print_reg(FILE *fp,char *name,int max) {
  int p,i;
  char *rg={"RGB"};
  fprintf(fp,"unsigned int %sMax=%d;\n",name,max);
  for (i=0;i<3;i++) {
    fprintf(fp,"unsigned char %s%c[%d]={\n  ",name,rg[i],max);
    for (p=0;p<max;p++) {
      if ((p % 9 ==0) && (p>0)) fprintf(fp,",\n  ");
      else if (p>0) fprintf(fp,",");
      fprintf(fp,"0x%.2x",out_reg[p][i]); 
    }
    fprintf(fp,"};\n");
  }
}

int main() {
  FILE *fpA;
  FILE *fpB;
  FILE *fpC;
  int c;  
  fpA=fopen("paltable.c","w");
  fpB=fopen("stdkey.h","w");
  fprintf(stderr,"making spectra.\n");
  make_reg();
  fprintf(stderr,"making color bars.\n");
  make_root();
  make_out_reg(256,17,256);
  fprintf(stderr,"printing palette table.\n");
  print_reg(fpA,"KeyPalReg",256);
  print_reg_hdr(fpB,"KeyPalReg",256);
  fprintf(stderr,"printing rgb tables.\n");
  make_map(256);
  print_map(fpA,"KeyLinear",256);
  print_hdr(fpB,"KeyLinear",256);

  fpC=fopen("linear-mag.key","w");
  fprintf(fpC,"# linear magnitude\n");
  fprintf(fpC,"%d\n",256);
  for (c=0;c<256;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",mag[c][0],mag[c][1],mag[c][2]);
  fclose(fpC);

  fpC=fopen("linear-sgn.key","w");
  fprintf(fpC,"# linear signed\n");
  fprintf(fpC,"%d\n",256);
  for (c=0;c<256;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",sgn[c][0],sgn[c][1],sgn[c][2]);
  fclose(fpC);


  make_map(20);
  print_map(fpA,"KeyTwentyBlk",20);
  print_hdr(fpB,"KeyTwentyBlk",20); 

  fpC=fopen("twentyblk-mag.key","w");
  fprintf(fpC,"# twentyblk magnitude\n");
  fprintf(fpC,"%d\n",20);
  for (c=0;c<20;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",mag[c][0],mag[c][1],mag[c][2]);
  fclose(fpC);

  fpC=fopen("twentyblk-sgn.key","w");
  fprintf(fpC,"# twentyblk signed\n");
  fprintf(fpC,"%d\n",20);
  for (c=0;c<20;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",sgn[c][0],sgn[c][1],sgn[c][2]);
  fclose(fpC);

  make_map(10);
  print_map(fpA,"KeyTenBlk",10);
  print_hdr(fpB,"KeyTenBlk",10); 

  fpC=fopen("tenblk-mag.key","w");
  fprintf(fpC,"# tenblk magnitude\n");
  fprintf(fpC,"%d\n",10);
  for (c=0;c<10;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",mag[c][0],mag[c][1],mag[c][2]);
  fclose(fpC);

  fpC=fopen("tenblk-sgn.key","w");
  fprintf(fpC,"# tenblk signed\n");
  fprintf(fpC,"%d\n",10);
  for (c=0;c<10;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",sgn[c][0],sgn[c][1],sgn[c][2]);
  fclose(fpC);

  make_map(5);
  print_map(fpA,"KeyFiveBlk",5);
  print_hdr(fpB,"KeyFiveBlk",5);

  fpC=fopen("fiveblk-mag.key","w");
  fprintf(fpC,"# fiveblk magnitude\n");
  fprintf(fpC,"%d\n",256);
  for (c=0;c<5;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",mag[c][0],mag[c][1],mag[c][2]);
  fclose(fpC);

  fpC=fopen("fiveblk-sgn.key","w");
  fprintf(fpC,"# fiveblk signed\n");
  fprintf(fpC,"%d\n",256);
  for (c=0;c<5;c++) 
    fprintf(fpC,"%.2x%.2x%.2x\n",sgn[c][0],sgn[c][1],sgn[c][2]);
  fclose(fpC);

  make_cmap(256);
  print_cmap(fpA,"KeyLinearC",256);
  print_chdr(fpB,"KeyLinearC",256);
  
  make_cmap(20);
  print_cmap(fpA,"KeyTwentyblkC",20);
  print_chdr(fpB,"KeyTwentyBlkC",20);
  
  make_cmap(10);
  print_cmap(fpA,"KeyTenBlkC",10);
  print_chdr(fpB,"KeyTenBlkC",10); 
  make_cmap(5);
  print_cmap(fpA,"KeyFiveBlkC",5);
  print_chdr(fpB,"KeyFiveBlkC",5);

  fclose(fpA);
  fclose(fpB);
  return 0;
}
  




