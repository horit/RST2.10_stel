#  Makefile for extract_time
#  ======================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general \
        -I$(IPATH)/analysis -I$(IPATH)/imagery
SRC=extract_time.c
OBJS =extract_time.o
OUTPUT = $(BINPATH)/extract_time
LIBS= -lsimage.1 -lopt.1 -lrtime.1 -ldmap.1 -lrcnv.1 -laacgm.1
SLIB= -lm
include $(MAKEBIN)

