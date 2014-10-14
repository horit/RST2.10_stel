#  Makefile for geo_simage
#  =======================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC = text_box.c text_box.h plot_time_label.c plot_time_label.h plot_time.c plot_time.h  make_grid.c make_grid.h key.c key.h clip.c clip.h geo_simage.c map.c map.c map.h errstr.h hlpstr.h 
OBJS = text_box.o plot_time_label.o plot_time.o make_grid.o key.o clip.o geo_simage.o map.o 
OUTPUT = $(BINPATH)/geo_simage
LIBS=-lsimage.1 -lszamap.1 -lsza.1 -laacgm.1 -lrmap.1 -lcontour.1 -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -ldmap.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 
XLIB=-lxwin.1
SLIB= -lm
include $(MAKEBIN)

