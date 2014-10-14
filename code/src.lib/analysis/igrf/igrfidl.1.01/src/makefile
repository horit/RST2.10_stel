# Makefile for igrfidl library
# ==============================
# by R.J.Barnes
#

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I${IDL_IPATH}
SRC= igrfidl.c
OBJS = igrfidl.o  
SLIB= -ligrf.1 
LINK=1
DSTPATH=$(LIBPATH)
OUTPUT = igrfidl
ifdef IDL_IPATH
include $(MAKELIBIDL)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif
 

 





