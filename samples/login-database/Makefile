#
#	esp-hosted Makefile
#

include ../defines.mk

all: build

build: files

files: controllers/user.c
	appweb-esp -q compile

run:
	appweb -v

clean:
	rm -rf ./cache
