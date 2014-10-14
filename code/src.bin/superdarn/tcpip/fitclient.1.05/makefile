# Makefile for fitclient
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitclient.o


SRC=hlpstr.h errstr.h fitclient.c

OUTPUT = $(BINPATH)/fitclient
LIBS=-loldfitcnx.1 -lcnx.1 -lopt.1 -lrcnv.1

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

 

