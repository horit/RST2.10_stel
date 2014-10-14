# Makefile for fovp
# =================
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery -I$(IPATH)/superdarn 

SRC=hlpstr.h fovp.c errstr.h clip.c clip.h make_fov.c make_fov.h
 
OBJS =clip.o make_fov.o fovp.o  
OUTPUT = $(BINPATH)/fovp
LIBS=-lrpos.1 -lradar.1 -lsza.1 -lszamap.1 -laacgm.1 -ligrf.1 -lrmap.1 -lcontour.1 -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lfbpsrplot.1 -lplot.1 -lrfile.1 -lrtime.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1
XLIB=-lxwin.1
SLIB= -lm
include $(MAKEBIN)

