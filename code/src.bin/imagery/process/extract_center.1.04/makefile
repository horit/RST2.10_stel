#  Makefile for extract_center
#  ===========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general \
        -I$(IPATH)/analysis -I$(IPATH)/imagery
SRC=extract_center.c
OBJS =extract_center.o
OUTPUT = $(BINPATH)/extract_center
LIBS= -lsimage.1 -lopt.1 -lrtime.1 -ldmap.1 -lrcnv.1 -laacgm.1
SLIB= -lm
include $(MAKEBIN)

