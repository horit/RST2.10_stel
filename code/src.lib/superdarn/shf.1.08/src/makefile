# Makefile for shf library
# =======================
# by R.J.Barnes
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/analysis   
OBJS = evallegendre.o evalpotential.o evalefield.o \
       solvepotential.o solvevelocity.o solveefield.o \
       solve.o indexlegendre.o evalvelocity.o crdshft.o \
       svdbksb.o svdcmp.o fitvector.o fitmap.o
SRC=  evallegendre.c evalpotential.c evalefield.c \
      solveefield.c solveefield.h solvepotential.c solvepotential.h \
      solvevelocity.c  solvevelocity.h nrutil.h \
      solve.c indexlegendre.c evalvelocity.c crdshft.c \
      svdbksb.c svdbksb.h svdcmp.c svdcmp.h fitvector.c \
      fitvector.h fitmap.c shfvector.h
LINK="1"
DSTPATH=$(LIBPATH)

OUTPUT = shf
include $(MAKELIB)
 

 





