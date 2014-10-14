# Makefile for map_addimf
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
         -I$(CDF_PATH)/include -I$(IPATH)/analysis
SRC=hlpstr.h ace.c ace.h map_addimf.c errstr.h istp.h locate.c locate.h wind.c wind.h
OBJS = ace.o wind.o locate.o map_addimf.o

OUTPUT = $(BINPATH)/map_addimf
LIBS=-lcnvmap.1 -loldcnvmap.1 -loldgrd.1 -lradar.1 -lrcdf.1 -lgeop.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 -L$(CDF_PATH)/lib -lcdf
SLIB=-lm

ifdef CDF_PATH
include $(MAKEBIN)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
	$(BINPATH)/rcsv -h version.info > version.h
clean:
	rm -f *.o
	rm -f $(OUTPUT)

endif

