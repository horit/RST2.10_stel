# Makefile for map_addmodel
# =========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/analysis
SRC=hlpstr.h map_addmodel.c errstr.h
OBJS = map_addmodel.o

OUTPUT =$(BINPATH)/map_addmodel

SLIB=-lm
LIBS= -lcnvmap.1 -loldcnvmap.1 -loldgrd.1 -lradar.1 -laacgm.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1  
include $(MAKEBIN)

