# Makefile for rawidl library
# ===========================
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC=rawidl.c
OBJS = rawidl.o
DSTPATH=$(LIBPATH)
OUTPUT = rawidl
SLIB= -lraw.1 -ldmap.1 -lrtime.1 -lrcnv.1
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



