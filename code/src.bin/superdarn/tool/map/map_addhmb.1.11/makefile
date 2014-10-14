# Makefile for map_addhmb
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn 
SRC=hlpstr.h map_addhmb.c errstr.h
OBJS = map_addhmb.o
OUTPUT = $(BINPATH)/map_addhmb

SLIB=-lm
LIBS= -lcnvmap.1 -loldcnvmap.1 -loldgrd.1 -lradar.1 -laacgm.1 -lrtime.1 -ldmap.1 -lrfile.1 -lopt.1 -lrcnv.1
include $(MAKEBIN)

