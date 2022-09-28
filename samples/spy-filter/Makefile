#
#	spy-filter Makefile
#

include ../defines.mk

all: build

build: libmod_spy

libmod_spy:
	cc $(CFLAGS) $(MODFLAGS) $(FLAGS) -I$(INCDIR) -o libmod_spy$(MODEXT) spyFilter.c -L$(LIBDIR) $(LIBS)

run:
	appweb

clean:
	rm -f libmod_spy.* *.o
