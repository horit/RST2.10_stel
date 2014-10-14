# Makefile for plotkey
# ====================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC=hlpstr.h key.h errstr.h plotkey.c
OBJS = plotkey.o 
OUTPUT = $(BINPATH)/plotkey
LIBS=-lrfbuf.1 -lrxml.1 -lrcnv.1 -lopt.1
SLIB= -lm
include $(MAKEBIN)

