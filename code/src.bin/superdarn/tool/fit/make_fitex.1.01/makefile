# Makefile for make_fitex
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_fitex.o

SRC=hlpstr.h errstr.h make_fitex.c
OUTPUT = $(BINPATH)/make_fitex
LIBS=-loldraw.1 -loldfit.1 -lfitacfex.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrmath.1 -lrcnv.1 
SLIB=-lm
include $(MAKEBIN)
 

