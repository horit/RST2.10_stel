# Makefile for extract_grid
# =========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
OBJS = extract_grid.o
SRC=hlpstr.h errstr.h extract_grid.c
OUTPUT = $(BINPATH)/extract_grid
LIBS=-lgrd.1 -loldgrd.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 
SLIB=-lm
include $(MAKEBIN) 

 

