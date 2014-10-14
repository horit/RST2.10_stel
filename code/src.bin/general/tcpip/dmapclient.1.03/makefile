# Makefile for dmapclient
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general
OBJS = dmapclient.o
OUTPUT = $(BINPATH)/dmapclient
SRC=hlpstr.h errstr.h dmapclient.c
OUTPUT = $(BINPATH)/dmapclient
LIBS=-lcnx.1 -ldmap.1 -lopt.1 -lrcnv.1

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

 

