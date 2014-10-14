# Makefile for time_image
# =======================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn

SRC=hlpstr.h oldfit.c oldfit.h csd.c csd.h fit.c fit.h smr.c smr.h errstr.h cfit.c cfit.h expr.c expr.h tplot.h key.c key.h time_image.c

OBJS = time_image.o oldfit.o fit.o smr.o cfit.o csd.o key.o expr.o
OUTPUT = $(BINPATH)/time_image
LIBS=-lstdkey.1 -lrpos.1 -lradar.1 -loldfit.1 -lcsd.1 -lsmr.1 -lcfit.1 -lfit.1 -lrscan.1 -lreval.1 -lrtime.1 -lrfbuf.1 -lrxml.1 -ldmap.1 -lopt.1 -lrcnv.1 -laacgm.1 -ligrf.1
SLIB= -lm
include $(MAKEBIN)

