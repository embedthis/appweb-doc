#
#	Build the documentation
#

all: serve

serve:
	expansive

build: doc

#
#	Must sync all paks to get latest code for APIs
#
doc: sync
	me doc

sync:
	pak sync


LOCAL_MAKEFILE := $(strip $(wildcard ./.local.mk))

ifneq ($(LOCAL_MAKEFILE),)
include $(LOCAL_MAKEFILE)
endif

