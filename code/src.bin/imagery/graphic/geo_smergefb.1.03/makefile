#  Makefile for geo_smergefb
#  =========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC=geo_smergefb.c image_table.c image_table.h clip.c clip.h  key.c key.h map.c map.h errstr.h hlpstr.h 
OBJS = geo_smergefb.o image_table.o clip.o key.o map.o 
OUTPUT = $(BINPATH)/geo_smergefb
LIBS=-lsimage.1 -lsza.1 -laacgm.1 -lrmap.1 -lcontour.1 -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -ldmap.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 
SLIB= -lm
include $(MAKEBIN)

