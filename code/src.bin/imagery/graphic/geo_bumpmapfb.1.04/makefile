#  Makefile for geo_bumpmapfb
#  ==========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC = clip.c clip.h key.c key.h geo_bumpmapfb.c errstr.h hlpstr.h
OBJS = clip.o key.o geo_bumpmapfb.o 
OUTPUT = $(BINPATH)/geo_bumpmapfb
LIBS=-lgmap.1 -lphong.1 -lelv.1 -lsza.1 -laacgm.1 -lrmap.1 -lcontour.1 -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -lrfile.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1
SLIB= -lm
include $(MAKEBIN)

