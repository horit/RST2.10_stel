# Makefile for rtgrid
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -DPOSIX -D__EXTENSIONS__

OBJS = rtgrid.o loginfo.o 
SRC=hlpstr.h errstr.h rtgrid.c loginfo.c loginfo.h output.h

OUTPUT = $(BINPATH)/rtgrid
LIBS=-lfitcnx.1 -lfit.1 -lcnx.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 -lfilter.1 -lrscan.1 -lrpos.1 -lradar.1 -laacgm.1 -ligrf.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1
SLIB=-lm

ifeq ($(OSTYPE),solaris)
SLIB =-lsocket -lnsl
endif

NET=$(shell find /usr/include/sys -name "socket.h")
ifeq ($(NET),/usr/include/sys/socket.h)
  include $(MAKEBIN) 
else 
  nonet:
	echo "TCP/IP development libraries not available."
endif

 

