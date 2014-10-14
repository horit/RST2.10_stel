#  Makefile for istp_plot.c
#  ========================

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(CDF_PATH)/include \
        -I$(IPATH)/analysis
OBJS = wind.o ace.o imp.o geotail.o locate.o graphic.o istp_plot.o 
SRC= wind.c wind.h ace.c ace.h imp.c imp.h geotail.c geotail.h locate.c locate.h graphic.c graphic.h istp_plot.c istp.h hlpstr.h errstr.h istp_plot.doc.xml
OUTPUT = $(BINPATH)/istp_plot
LIBS=  -lopt.1 -lfbpsrplot.1 -lrtime.1  -lgeop.1 -lrcdf.1 -lplot.1 -lrplotin.1 -lrxml.1 -lrplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -L${CDF_PATH}/lib -lcdf
SLIB= -lm 
XLIB= -lxwin.1


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



 















