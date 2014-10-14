# Makefile for buffer_server
# ==========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base
SRC=hlpstr.h errstr.h buffer_server.c config.h
OBJS=buffer_server.o
OUTPUT = $(BINPATH)/buffer_server
LIBS=-lopt.1
ifeq ($(OSTYPE),solaris)
SLIB =-lsocket -lnsl
endif

include $(MAKEBIN)
 

 

