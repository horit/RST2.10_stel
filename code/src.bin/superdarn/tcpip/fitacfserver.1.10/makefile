# Makefile for fitacfserver
# =========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -DPOSIX -D__EXTENSIONS__
OBJS = loginfo.o fork.o  fitpacket.o \
	socket.o connect.o msgmem.o fitacfserver.o 
SRC=hlpstr.h errstr.h connect.c connect.h fitacfserver.c loginfo.c loginfo.h cnxstr.h fork.c fork.h fitpacket.c fitpacket.h msgmem.c msgmem.h socket.c socket.h


OUTPUT = $(BINPATH)/fitacfserver
LIBS=-loldfit.1 -lcnx.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lrtime.1 -lopt.1 -lrcnv.1

ifeq ($(OSTYPE),solaris)
SLIB =-lm -lsocket -lnsl -lposix4
else
SLIB =-lm
endif

NET=$(shell find /usr/include/sys -name "socket.h")
ifeq ($(NET),/usr/include/sys/socket.h)
  include $(MAKEBIN) 
else 
  nonet:
	echo "TCP/IP development libraries not available."
endif

 

