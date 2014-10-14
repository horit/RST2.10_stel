# Makefile for xwin_lib
# =====================
# by R.J.Barnes
#
#


INCLUDE=-I$(IPATH)/base -I$(XPATH)/include

SRC=xwin.c
OBJS = xwin.o
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT=xwin
ifdef XPATH
include $(MAKELIB)
else 
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif




