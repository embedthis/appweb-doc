#
#	defines.mk -- Definitions to build samples with GCC make
#

PROFILE := debug
OS      := $(shell uname | sed 's/CYGWIN.*/windows/;s/Darwin/macosx/' | tr '[A-Z]' '[a-z]')
TOP		:= $(realpath $(PWD)/../..)

ifeq ($(ARCH),)
    ifeq ($(OS),windows)
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            ARCH?=x64
        else
            ARCH?=x86
        endif
    else
        ARCH:= $(shell uname -m | sed 's/i.86/x86/;s/x86_64/x64/;s/arm.*/arm/;s/mips.*/mips/')
    endif
endif

LIBDIR 	:= $(TOP)/build/$(OS)-$(ARCH)-$(PROFILE)/bin
INCDIR  := $(TOP)/build/$(OS)-$(ARCH)-$(PROFILE)/inc
LIBS	:= -lappweb -lesp -lhttp -lmpr -lpcre -ldl -lpthread -lm
CFLAGS	:= 

ifeq ($(OS),macosx)
	LDFLAGS	:= -Wl,-rpath,@executable_path/ -Wl,-rpath,@loader_path/ -Wl,-rpath,$(LIBDIR)/
	MODFLAGS := -dynamiclib -install_name @rpath/libmod_simple.dylib -compatibility_version 1.0 -current_version 1.0
	MODEXT := .dylib
endif
ifeq ($(OS),linux)
	LDFLAGS	:= 
	MODEXT := .so
endif
