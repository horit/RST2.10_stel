# Makefile for time_plot
# ======================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC = time_plot.c graphics.c fit.c oldfit.c smr.c cfit.c csd.c expr.c key.c cfit.h csd.h errstr.h expr.h fit.h graphics.h hlpstr.h key.h oldfit.h smr.h tplot.h



OBJS = time_plot.o graphics.o fit.o oldfit.o smr.o cfit.o csd.o expr.o key.o
OUTPUT = $(BINPATH)/time_plot
LIBS=-loldfit.1 -lstdkey.1 -lrpos.1 -lradar.1 -lcsd.1 -lsmr.1 -lcfit.1 -lfit.1 -lrscan.1 -lreval.1 -lrtime.1 -lfbpsrplot.1 -lplot.1 -lrplotin.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -ldmap.1 -lopt.1 -lrcnv.1 -laacgm.1 -ligrf.1
XLIB= -lxwin.1
SLIB= -lm
include $(MAKEBIN)

