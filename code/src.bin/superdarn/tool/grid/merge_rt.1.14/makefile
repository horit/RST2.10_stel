# Makefile for merge_rt
# =====================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 

OBJS = merge_rt.o log_info.o
SRC=hlpstr.h errstr.h log_info.c log_info.h merge_rt.c
OUTPUT = $(BINPATH)/merge_rt
LIBS= -lgrd.1 -loldgrd.1 -lradar.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 
SLIB= -lm
include $(MAKEBIN)

 

