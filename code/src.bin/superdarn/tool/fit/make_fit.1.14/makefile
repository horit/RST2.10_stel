# Makefile for make_fit
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_fit.o

SRC=hlpstr.h errstr.h make_fit.c

OUTPUT = $(BINPATH)/make_fit
LIBS=-loldraw.1 -loldfit.1 -lrscan.1 -lradar.1 -loldfitacf.1 -lfitacf.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
include $(MAKEBIN)
 

