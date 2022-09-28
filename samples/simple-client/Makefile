#
#	simple-client Makefile
#

include ../defines.mk

all: build

build: client

client: client.c
	cc $(CFLAGS) $(LDFLAGS) -I$(INCDIR) -o client client.c -L$(LIBDIR) $(LIBS)

run:
	./client

clean:
	rm -rf client client.o 
