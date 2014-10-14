# Makefile for rawacftohok
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = rawacftoadjust.o
SRC=hlpstr.h errstr.h rawacftoadjust.c
OUTPUT = $(BINPATH)/rawacftoadjust
LIBS= -loldraw.1 -lraw.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
include $(MAKEBIN)
 

