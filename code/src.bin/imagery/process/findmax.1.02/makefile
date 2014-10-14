#  Makefile for findmax
#  ====================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general \
        -I$(IPATH)/analysis -I$(IPATH)/imagery
SRC=findmax.c hlpstr.h
OBJS =findmax.o
OUTPUT = $(BINPATH)/findmax
LIBS= -lsimage.1 -lopt.1 -lrtime.1 -ldmap.1 -lrcnv.1 -laacgm.1
SLIB= -lm
include $(MAKEBIN)

