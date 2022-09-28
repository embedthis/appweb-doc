#
#	Samples Makefile
#

SAMPLES		:= \
	chroot-server \
	cpp-handler \
	cpp-module \
	deploy-server \
	esp-hosted \
	esp-upload \
	login-basic \
	login-database \
	login-form \
	max-server \
	min-server \
	non-blocking-client \
	secure-server \
	simple-action \
	simple-client \
	simple-handler \
	simple-module \
	simple-server \
	spy-filter \
	ssl-server \
	thread-comms \
	threaded-client \
	tiny-server \
	typical-server \
	websockets-chat \
	websockets-echo \
	websockets-output


ifndef SHOW
.SILENT:
endif

all: build

build:
	@for sample in $(SAMPLES) ; do \
		echo make -C $$sample ; \
	done

clean:
	@for sample in $(SAMPLES) ; do \
		echo make -C $$sample clean ; \
	done
