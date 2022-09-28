#
#	esp-hosted Makefile
#

include ../defines.mk

all: build

build: files

files: controllers/post.c
	appweb-esp -q compile

run:
	appweb -v

clean:
	rm -rf ./cache
