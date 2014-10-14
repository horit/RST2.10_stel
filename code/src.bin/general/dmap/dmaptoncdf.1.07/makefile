# Makefile for dmaptoncdf
# =======================
# Author: R.J.Barnes
# Author: R.J.Barnes

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(NETCDF_PATH)/include
SRC=hlpstr.h errstr.h dmaptoncdf.c
OBJS =dmaptoncdf.o
OUTPUT = $(BINPATH)/dmaptoncdf
LIBS= -lopt.1 -ldmap.1 -lrcnv.1
SLIB= -lm -L$(NETCDF_PATH)/lib -lnetcdf

ifdef NETCDF_PATH
include $(MAKEBIN)
else
version: version.info $(SRC)
	$(BINPATH)/rcsv -t version.info
	$(BINPATH)/rcsv -h version.info > version.h
clean:
	rm -f *.o
	rm -f $(OUTPUT)
endif

