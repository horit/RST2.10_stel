# Makefile for eval
# =================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general \
        -I$(IPATH)/analysis -I$(IPATH)/imagery
SRC=hlpstr.h errstr.h eval.c
OBJS =eval.o
OUTPUT = $(BINPATH)/eval
LIBS= -lreval.1 -lopt.1 
SLIB= -lm
include $(MAKEBIN)

