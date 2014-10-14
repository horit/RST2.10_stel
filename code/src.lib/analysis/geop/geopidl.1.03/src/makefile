# Makefile for gsmidl library
# ===========================
# by R.J.Barnes
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \

SRC = geopidl.c
OBJS = geopidl.o

SLIB= -L$(LIBPATH) -lgeop.1
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT = geopidl
ifdef IDL_IPATH
include $(MAKELIBIDL)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif
 





