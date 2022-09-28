#
#	chroot-server Makefile
#

include ../defines.mk

all: build

build: files server

files: web/index.html
	appweb-esp -q compile

server: server.c
	cc $(CFLAGS) $(LDFLAGS) -I$(INCDIR) -o server server.c -L$(LIBDIR) $(LIBS)

run:
	./server

clean:
	rm -rf server server.o ./cache
