/* makeall.c
   =========
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
 $Log: makeall.c,v $
 Revision 1.16  2004/07/06 19:06:36  barnes
 Added missing header.

 Revision 1.15  2004/04/27 13:31:06  barnes
 Audit - Enforced warning set to "all".

 Revision 1.14  2004/04/25 16:49:33  barnes
 Added missing header.

 Revision 1.13  2002/02/22 23:08:52  barnes
 Modification to make sure that when not otherwise specified, compilation
 is in alphabetical order.

 Revision 1.12  2001/06/27 18:09:10  barnes
 Added place holder for license.

 Revision 1.11  2001/02/02 20:15:58  barnes
 Fixed missing return value.

 Revision 1.10  2001/01/30 21:15:44  barnes
 Added help message.

 Revision 1.9  2001/01/29 18:12:11  barnes
 Added Author Name

 Revision 1.8  2001/01/26 17:51:05  barnes
 Bug fix to make sure the root package name is an exact match.

 Revision 1.7  2001/01/26 15:01:50  barnes
 Modification to the way the package version numbers are decoded.

 Revision 1.6  2001/01/25 22:26:35  barnes
 Fixed bug in decoding command line options

 Revision 1.5  2001/01/25 22:24:34  barnes
 Modification to simplify the calling sequence.

 Revision 1.4  2001/01/24 22:53:45  barnes
 Modified the way the temporary filename is calculated to make it
 platform independent.

 Revision 1.3  2001/01/24 19:04:12  barnes
 Fixed bug in the abort flag.

 Revision 1.2  2001/01/24 18:46:05  barnes
 Move logging to standard output and tidied up code.

 Revision 1.1  2001/01/23 23:18:00  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef _QNX4
#include <process.h>
#endif

#include <dirent.h>
#include "hlpstr.h"
#include "errstr.h"




int vflg=1; /* compile only most recent version of code */
int num=0;
int lnum=0;
char *makelist[4096];
char *makename[4096];
char *makeroot[4096];
int makevmajor[4096];
int makevminor[4096];
char maketype[4096];
char line[256];
int vbflg=0;
char *src=NULL;



void log_info(char *text) {
  if (vbflg==1) fprintf(stderr,text);
  fprintf(stdout,text);
  fflush(stdout);
}

int do_make(char *target,char *path) {
  int status;
  char tmpname[256];
  time_t ltime;
  pid_t pid;
  
  pid=getpid();
  time(&ltime);  

  sprintf(tmpname,"log.%d.%d",pid,(int) ltime);
 
  pid=fork();
  if (pid==-1) return -1;
  if (pid==0) { /* execute the make command here */
    int s;
    freopen(tmpname,"a+",stderr);
    freopen(tmpname,"a+",stdout);
 
    if (target !=NULL) s=execlp("make","make",target,NULL); 
    else s=execlp("make","make",NULL); 

    exit(s);
  }
  do {
     if (waitpid(pid, &status, 0) == -1) {
       FILE *fp;
       /* now display log message and delete */
       fp=fopen(tmpname,"r");
       while(fgets(line,256,fp) !=NULL) {
         log_info(line);
       }
       fclose(fp);
       unlink(tmpname);
       return status;
     } 
  } while(1);
}


void add_make(char *name,char type) {

  int i,j,k;
  int major=0,minor=0;

  i=strlen(src)+1;

  /* the end of the path is the version number if any */

  for (j=strlen(name)-1;(name[j] !='.') && (j>0);j--);
  if (j>0) {
    minor=atoi(name+j);
    for (k=j-1;(name[k] !='.') && (k>0);k--);
    if (k>0) {
      name[j]=0;
      major=atoi(name+k);
      name[j]='.';
      j=k;
    } else {
      minor=0;
      j=strlen(name);
    }
  } else j=strlen(name);

  if (vflg==1) {
    for (k=0;k<num;k++) {
      if ((strlen(makeroot[k])==(j-i)) && 
          (strncmp(makeroot[k],name+i,j-i)==0)) break;
    }  
    if (k<num) {
   
      /* now compare version numbers */
      if (major<makevmajor[k]) return;
      if (minor<makevminor[k]) return;
  
      makevmajor[k]=major;
      makevminor[k]=minor;
      makename[k]=realloc(makename[k],strlen(name)+1);
      strcpy(makename[k],name);
      return;
    }
  }
  makename[num]=malloc(strlen(name)+1);
  maketype[num]=type;
  strcpy(makename[num],name);
  name[j]=0;
  makeroot[num]=malloc(strlen(name+i)+1);
  strcpy(makeroot[num],name+i);
 
  
  
  num++;

}


int find_makefile(char *path) {
  struct dirent *direntp=NULL;
  DIR *dirp;
  int s; 
  struct stat dstat;
  int state=0;
  char spath[4096];

  dirp=opendir(path);
  if (dirp==NULL) return 0;
  
  while ((direntp=readdir(dirp)) !=NULL) {
     if (direntp->d_name[0]=='.') continue;
     if (strcmp(direntp->d_name,"makefile")==0) add_make(path,'b');
     if (strcmp(direntp->d_name,"src")==0) state++;
     if (strcmp(direntp->d_name,"include")==0) state++;

     if (state==2) { /* possible library source directory */
       struct dirent *direntpl=NULL;
       DIR *dirpl;
       sprintf(spath,"%s/src",path);
       dirpl=opendir(spath);
       if (dirpl==NULL) break;
       while ((direntpl=readdir(dirpl)) !=NULL) {
         
         if (direntpl->d_name[0]=='.') continue;
	 if (strcmp(direntpl->d_name,"makefile")==0) add_make(path,'l');
       }
       closedir(dirpl);
       break;
     }

     if (state !=0) continue;

      
     sprintf(spath,"%s/%s",path,direntp->d_name);
     s=stat(spath,&dstat); 
    
     if ((s==0) && (S_ISDIR(dstat.st_mode))) {
       /* recurse here */
       find_makefile(spath);
     }
  }
  closedir(dirp);
  return 0;
}



void load_list(FILE *fp) {
  int i;
  char line[1024];
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i]==' ') && (line[i] !=0);i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

    memmove(line,line+i,strlen(line)+1-i);
    for (i=0;(line[i] !=0) && (line[i] !='\n') && (line[i] !=' ');i++); 
    line[i]=0; 
    if (strlen(line)==0) continue;
    makelist[lnum]=malloc(strlen(line)+1);
    strcpy(makelist[lnum],line);
    lnum++;

  } 
}

int dsort(const void *a,const void *b) {
  char **x;
  char **y;
  x=(char **) a;
  y=(char **) b;
  return strcmp(*x,*y);
}
 
void print_info(FILE *fp,char *str[]) {
  int i;
  for (i=0;str[i] !=NULL;i++) fprintf(fp,str[i]);
}



int main(int argc,char *argv[]) {
 
  char logstr[1024];
  char path[4096];

  char sep[256];
  int i,j,status;
  int aflg=1;



  for (i=0;i<80;i++) sep[i]='=';
  sep[i]='\n';
  sep[i+1]=0;



  for (i=1;i<argc;i++) {
    if (strcmp(argv[i],"-a")==0) vflg=0;
    else if (strcmp(argv[i],"-q")==0) aflg=0;
    else if (strcmp(argv[i],"-vb")==0) vbflg=1;
    else if (strcmp(argv[i],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    } else break;
  }

  if (argc<(1+i)) {
    print_info(stderr,errstr);
    exit(1);
  }

  src=argv[i];
  if (argc>(i+1)) {
    if (strcmp(argv[i+1],"stdin")==0) load_list(stdin);
    else {
      for (j=0;j<(argc-i-1);j++) makelist[j]=argv[j+i+1];
      lnum=j;      
    }
  }
 

 
  find_makefile(src);

  log_info("makeall\n");
  

  sprintf(logstr,"Package Source Directory:%s\n",src);
  log_info(logstr);

  if (aflg !=0) sprintf(logstr,"Abort on failure:yes\n");
  else sprintf(logstr,"Abort on failure:no\n");

  log_info(logstr);

  

  if (vflg !=0) sprintf(logstr,
                "Compile most recent version of code only:yes\n");
  else sprintf(logstr,"Compile most recent version of code only:no\n");

  log_info(logstr);
  log_info(sep);

  if (lnum !=0) {
    sprintf(logstr,"Compilation Order List:\n");
    log_info(logstr);
    for  (i=0;i<lnum;i++) {
      sprintf(logstr,"%d:%s\n",i+1,makelist[i]);
      log_info(logstr);
    }
  }
 
  if (lnum==0) {
    lnum=num;
    for (i=0;i<num;i++) makelist[i]=makeroot[i];

    /* sort the make list into alphabetical order */
       qsort(makelist,num,sizeof(char *),dsort);
     
  

  }

 
  log_info("Located Source Code:\n");
 
  for (i=0;i<lnum;i++) {
    for (j=0;j<num;j++) if (strcmp(makelist[i],makeroot[j])==0) break;
    if (j==num) continue;
    if (maketype[j]=='b') sprintf(logstr,"%d.Located Binary:%s\n",
                                  i+1,makename[j]);
    else sprintf(logstr,"%d.Located Library:%s\n",i+1,makename[j]);
    log_info(logstr);
  }
  
  
  for (i=0;i<lnum;i++) {
     for (j=0;j<num;j++) if (strcmp(makelist[i],makeroot[j])==0) break;
    if (j==num) continue;
    log_info(sep);
    if (maketype[j]=='b') sprintf(logstr,"Compiling Binary:%s\n",makename[j]);
    else sprintf(logstr,"Compiling Library:%s\n",makename[j]);
    log_info(logstr);
    log_info(sep);
    
    

    if (maketype[j]=='b') sprintf(path,"%s",makename[j]);
    else sprintf(path,"%s/src",makename[j]);
    chdir(path);

    log_info("make clean\n");
    status=do_make("clean",path);
    if ((aflg !=0) && (status !=0)) break;
    log_info("make\n");
    status=do_make(NULL,path);
    if ((aflg !=0) && (status !=0)) break;
    
  }
  if (i<lnum) log_info("Compilation Aborted.\n");
 
   


  return 0;
}
 


  




