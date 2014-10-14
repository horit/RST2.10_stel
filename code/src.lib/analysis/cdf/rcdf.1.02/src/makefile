# Makefile for rcdf
# =================
# by R.J.Barnes
#
#
INCLUDE=-I$(IPATH)/analysis -I$(CDF_PATH)/include

SRC = rcdf.c    
OBJS = rcdf.o
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT = rcdf

ifdef CDF_PATH
include $(MAKELIB)
else 
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif


 

 





