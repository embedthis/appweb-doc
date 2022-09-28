#
#	websockets-chat Makefile
#

include ../defines.mk

all: build

build: files

files: web/index.html
	appweb-esp -q compile

run:
	appweb -v

clean:
	rm -rf ./cache
