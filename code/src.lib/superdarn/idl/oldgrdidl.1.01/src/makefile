# Makefile for oldgrdidl library
# ===========================
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC=oldgrdidl.c
OBJS = oldgrdidl.o
DSTPATH=$(LIBPATH)
OUTPUT = oldgrdidl
SLIB= -loldgrd.1 -lrfile.1 -lrtime.1 -lrcnv.1
LINK="1"
ifdef IDL_IPATH
include $(MAKELIBIDL)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif


