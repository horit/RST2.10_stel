# Makefile for fitacfclientscan
# =============================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitacfclientscan.o downloadscan.o
SRC=hlpstr.h fitacfclientscan.c errstr.h downloadscan.c downloadscan.h
OUTPUT = $(BINPATH)/fitacfclientscan
LIBS=-lfitcnx.1 -lfit.1 -lrscan.1 -lcnx.1 -ldmap.1 -lrtime.1 -lrcnv.1 -lopt.1
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

 

