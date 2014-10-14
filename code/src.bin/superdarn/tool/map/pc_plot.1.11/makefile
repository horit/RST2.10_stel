# Makefile for pc_plot
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=hlpstr.h errstr.h pc_plot.c
OBJS = pc_plot.o
OUTPUT = $(BINPATH)/pc_plot

SLIB=-lm 
LIBS=-lcnvmap.1 -loldcnvmap.1 -loldgrd.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 
XLIB=-lxwin.1
include $(MAKEBIN)

