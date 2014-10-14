#  Makefile for apply_bumpfb
#  =========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC=apply_bumpfb.c errstr.h hlpstr.h
OBJS = apply_bumpfb.o
OUTPUT = $(BINPATH)/apply_bumpfb
LIBS=-lrfbuf.1 -lrxml.1 -lrcnv.1 -lopt.1
SLIB= -lm
include $(MAKEBIN)

