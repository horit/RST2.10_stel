#  Makefile for geo_globefb
#  ========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC = make_grid.c make_grid.h  clip.c clip.h geo_globefb.c errstr.h hlpstr.h
OBJS = make_grid.o clip.o geo_globefb.o 
OUTPUT = $(BINPATH)/geo_globefb
LIBS=-lsza.1 -lszamap.1 -laacgm.1 -lrmap.1 -lcontour.1 -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -lrfile.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 
SLIB= -lm
include $(MAKEBIN)

