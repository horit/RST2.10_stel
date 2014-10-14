# Makefile for aacgmidl library
# ==============================
# by R.J.Barnes
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I${IDL_IPATH}
SRC= mltidl.c aacgmidl.c
OBJS = mltidl.o aacgmidl.o  
SLIB= -laacgm.1 
LINK=1
DSTPATH=$(LIBPATH)
OUTPUT = aacgmidl
ifdef IDL_IPATH
include $(MAKELIBIDL)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif

 





