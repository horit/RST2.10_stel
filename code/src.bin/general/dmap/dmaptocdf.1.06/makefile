# Makefile for dmaptocdf
# ======================
# Author: R.J.Barnes
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(CDF_PATH)/include
SRC=hlpstr.h errstr.h dmaptocdf.c
OBJS=dmaptocdf.o
OUTPUT = $(BINPATH)/dmaptocdf
LIBS= -lopt.1 -ldmap.1 -lrcnv.1 -L$(CDF_PATH)/lib -lcdf
SLIB= -lm

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
