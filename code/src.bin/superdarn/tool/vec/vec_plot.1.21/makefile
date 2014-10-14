# Makefile for vec_plot
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn  \
	-I$(IPATH)/analysis
OBJS = plot_circle.o plot_radial.o plot_time.o text_box.o vec_plot.o
SRC=text_box.c text_box.h vec_plot.c hlpstr.h plot_time.c plot_time.h errstr.h plot_radial.c plot_radial.h plot_circle.c plot_circle.h

OUTPUT = $(BINPATH)/vec_plot
LIBS=-laacgm.1 -ligrf.1 -lrmap.1 -lpoly.1 -lrpos.1 -lradar.1 -lrtime.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 
XLIB= -lxwin.1
SLIB=-lm 
include $(MAKEBIN) 

 

