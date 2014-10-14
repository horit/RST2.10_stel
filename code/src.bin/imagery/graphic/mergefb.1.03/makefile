#  Makefile for mergefb
#  ====================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery 
SRC=mergefb.c errstr.h hlpstr.h 
OBJS = mergefb.o
OUTPUT = $(BINPATH)/mergefb
LIBS=-lrfbuf.1 -lrxml.1 -lrcnv.1 -lopt.1
SLIB= -lm
include $(MAKEBIN)

