# Makefile for make_cfit
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_cfit.o
SRC=hlpstr.h errstr.h make_cfit.c
OUTPUT = $(BINPATH)/make_cfit
LIBS=-loldfit.1 -lcfit.1 -lfit.1 -lrscan.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm
include $(MAKEBIN)
 

