# Makefile for map_ascii
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
	-I$(IPATH)/superdarn 

SRC=hlpstr.h errstr.h map_ascii.c make_pgrid.c make_pgrid.h
OBJS = map_ascii.o make_pgrid.o
OUTPUT = $(BINPATH)/map_ascii

SLIB=-lm
LIBS=  -lcnvmap.1 -loldcnvmap.1 -lshf.1 -loldgrd.1 -laacgm.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1  
include $(MAKEBIN)

