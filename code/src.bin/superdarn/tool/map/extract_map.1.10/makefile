# Makefile for extract_map
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn
SRC=hlpstr.h errstr.h extract_map.c
OBJS = extract_map.o
OUTPUT = $(BINPATH)/extract_map
SLIB=-lm
LIBS= -lcnvmap.1 -loldcnvmap.1 -loldgrd.1 -lgrd.1 -lradar.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1  
include $(MAKEBIN)

