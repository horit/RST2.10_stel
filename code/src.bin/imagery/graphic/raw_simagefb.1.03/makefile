#  Makefile for raw_simage
#  ========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC = key.c key.h raw_simagefb.c errstr.h hlpstr.h 
OBJS = key.o raw_simagefb.o 
OUTPUT = $(BINPATH)/raw_simagefb
LIBS=-lsimage.1 -lrtime.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -ldmap.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 
SLIB= -lm
include $(MAKEBIN)

