# Makefile for rtmultiplex
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -DPOSIX -D__EXTENSIONS__
OBJS = rtmultiplex.o loginfo.o fork.o socket.o msgmem.o connect.o pipe.o
SRC=hlpstr.h errstr.h connect.c connect.h loginfo.c loginfo.h fork.c fork.h pipe.c pipe.h msgmem.c msgmem.h rtmultiplex.c socket.c socket.h
OUTPUT = $(BINPATH)/rtmultiplex
LIBS=-lrtime.1 -lcnx.1 -lopt.1 -lrcnv.1 

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

 

