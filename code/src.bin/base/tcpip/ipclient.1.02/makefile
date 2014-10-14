# Makefile for ipclient
# =====================
# Author: R.J.Barnes
#
INCLUDE=-I$(IPATH)/base
OBJS = ipclient.o
SRC=ipclient.c
OUTPUT = $(BINPATH)/ipclient
SRC=hlpstr.h errstr.h ipclient.c
OUTPUT = $(BINPATH)/ipclient
LIBS=-lcnx.1 -lopt.1

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

 

