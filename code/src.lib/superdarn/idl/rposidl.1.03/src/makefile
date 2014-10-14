# Makefile for rposidl library
# ============================
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC=rposidl.c
OBJS = rposidl.o
DSTPATH=$(LIBPATH)

OUTPUT = rposidl
SLIB= -lrpos.1 -laacgm.1 -ligrf.1 -lradar.1 -lrtime.1 -lrcnv.1
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


