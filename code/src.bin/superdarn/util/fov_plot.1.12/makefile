# Makefile for fov_plot
# =====================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery -I$(IPATH)/superdarn 
 
SRC=text_box.c text_box.h hlpstr.h fov_plot.c errstr.h clip.c clip.h make_grid.c make_grid.h make_fov.c make_fov.h plot_time_label.c plot_time_label.h
 
OBJS = text_box.o plot_time_label.o make_grid.o clip.o make_fov.o fov_plot.o  
OUTPUT = $(BINPATH)/fov_plot
LIBS=-lrpos.1 -lradar.1 -lsza.1 -lszamap.1 -laacgm.1 -ligrf.1 -lrmap.1 -lcontour.1 -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -lrfile.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1
XLIB=-lxwin.1
SLIB= -lm
include $(MAKEBIN)

