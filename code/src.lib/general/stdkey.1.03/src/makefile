# Makefile for stdkey library
# ===========================
# by R.J.Barnes
#
#


OUTPUT=stdkey
DSTPATH=$(LIBPATH)
OBJS=paltable.o
LINK="1"
lib: cleanall make_palette stdkey
	rm -fv paltable.c
	mv stdkey.h $(IPATH)/general/stdkey.h

include $(MAKELIB)

paltable.o:paltable.c


make_palette: make_palette.o
	$(CC) -o make_palette make_palette.o -lm			 
	./make_palette	

cleanall:
	rm -fv *.o
	rm -fv paltable.c
	rm -fv make_palette

 

 















