# Makefile for rtfittofitacf
# ==========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -DPOSIX -D__EXTENSIONS__
OBJS = rtfittofitacf.o loginfo.o fork.o fitpacket.o \
	socket.o connect.o msgmem.o 

SRC=hlpstr.h errstr.h connect.c connect.h loginfo.c loginfo.h cnxstr.h fork.c fork.h rtfittofitacf.c fitpacket.c fitpacket.h msgmem.c msgmem.h socket.c socket.h
OUTPUT = $(BINPATH)/rtfittofitacf
LIBS=-loldfitcnx.1 -lcnx.1 -lradar.1 -ldmap.1 -lrtime.1 -lopt.1 -lrcnv.1 

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

 

