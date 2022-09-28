#
#	simple-handler Makefile
#

include ../defines.mk

all: build

build: libmod_simple

libmod_simple:
	cc $(CFLAGS) $(MODFLAGS) $(FLAGS) -I$(INCDIR) -o libmod_simple$(MODEXT) simpleHandler.c -L$(LIBDIR) $(LIBS)

run:
	appweb

clean:
	rm -f libmod_simple.* *.o
