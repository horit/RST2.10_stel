# Makefile for rtcsd
# ==================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -DPOSIX -D__EXTENSIONS__

OBJS=loginfo.o buffer.o rtcsd.o compress.o
SRC=hlpstr.h errstr.h rtcsd.c loginfo.c loginfo.h compress.c compress.h default.h buffer.c buffer.h
OUTPUT = $(BINPATH)/rtcsd
LIBS=-lfitcnx.1 -lcnx.1 -lcsd.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lrtime.1 -lopt.1 -lrcnv.1  
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

 

