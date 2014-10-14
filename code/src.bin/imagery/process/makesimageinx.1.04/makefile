#  Makefile for makesimageinx
#  ==========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general \
        -I$(IPATH)/analysis -I$(IPATH)/imagery
SRC=makesimageinx.c hlpstr.h
OBJS =makesimageinx.o
OUTPUT = $(BINPATH)/makesimageinx
LIBS= -lsimage.1 -lrtime.1 -ldmap.1 -lopt.1 -lrcnv.1 -laacgm.1
SLIB= -lm
include $(MAKEBIN)

