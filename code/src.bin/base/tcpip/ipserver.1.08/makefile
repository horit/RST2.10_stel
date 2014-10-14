# Makefile for ipserver
# =====================
# Author: R.J.Barnes
#
 
INCLUDE=-I$(IPATH)/base -DPOSIX -D__EXTENSIONS__
OBJS = log_info.o fork.o socket.o connect.o msg_mem.o ipserver.o 
SRC=hlpstr.h errstr.h msg_mem.c msg_mem.h connect.c connect.h log_info.c log_info.h fork.c fork.h ipserver.c socket.c socket.h
OUTPUT = $(BINPATH)/ipserver
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
 

