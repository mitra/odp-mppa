FIRMDIR    := $(shell basename $(SRCDIR))
BUILDDIR   := $(shell pwd)
MAKEFILE   := $(lastword $(MAKEFILE_LIST))
TOP_SRCDIR := $(realpath $(dir $(MAKEFILE))/..)
SRCFILES   += $(shell find $(SRCDIRS) -name "[^.]*.c")
ASMFILES   += $(shell find $(SRCDIRS) -name "[^.]*.S")
HDRFILES   += $(shell find $(SRCDIRS) -name "[^.]*.h")
OBJFILES   += $(patsubst %.c, $(BUILDDIR)/%.o, $(SRCFILES))
ASMOBJFILES+= $(patsubst %.S, $(BUILDDIR)/%.o, $(ASMFILES))

ODP_TOOLCHAIN_DIR ?= $(K1_TOOLCHAIN_DIR)
include $(ODP_TOOLCHAIN_DIR)/share/odp/build/mk/platforms.inc
ODP_FIRMWARE_CONFIG ?= k1b-kalray_konic80

# Inherit flags from platforms.inc
CC            = $(firmware_$(ODP_FIRMWARE_CONFIG)_CC)
PLAT_CFLAGS   = $(firmware_$(ODP_FIRMWARE_CONFIG)_CFLAGS)
PLAT_LDFLAGS  = $(firmware_$(ODP_FIRMWARE_CONFIG)_CFLAGS)


ODP_INCDIR  = $(TOP_SRCDIR)/platform/mppa/include
ODP_SRCDIR  = $(TOP_SRCDIR)/platform/mppa/
_CFLAGS    += -I$(ODP_INCDIR) -I$(TOP_SRCDIR)/include
_CFLAGS    += $(CFLAGS) -g3 -Wall -Wextra -Werror -std=gnu99 $(patsubst %, -I%, $(SRCDIRS))
_LDFLAGS   += $(LDFLAGS)

ifdef DEBUG
_CFLAGS    += -O0
else
_CFLAGS    += -O2
endif
ifdef VERBOSE
_CFLAGS += -DVERBOSE
endif

ifndef INCLUDING_FIRMWARE

ifdef BUILD_LIBRARY
LIB      := $(BUILDDIR)/lib$(FIRMDIR).a
INSTBIN  := $(INSTDIR)/lib$(FIRMDIR).a
else
BIN      := $(BUILDDIR)/$(FIRMDIR).kelf
INSTBIN  := $(INSTDIR)/$(FIRMDIR).kelf
endif
all: $(BIN) $(LIB)

$(BIN): $(OBJFILES) $(ASMOBJFILES)
	$(CC) -o $@ $^ $(PLAT_CFLAGS) $(_CFLAGS) $(PLAT_LDFLAGS) $(_LDFLAGS)
$(LIB): $(OBJFILES) $(ASMOBJFILES)
	ar rc $@ $^

$(OBJFILES): $(BUILDDIR)/%.o: %.c $(HDRFILES)
	@mkdir -p $$(dirname $@) || true
	$(CC) -o $@ -c $< $(PLAT_CFLAGS) $(_CFLAGS)

$(ASMOBJFILES): $(BUILDDIR)/%.o: %.S $(HDRFILES)
	@mkdir -p $$(dirname $@) || true
	$(CC) -o $@ -c $< $(PLAT_CFLAGS) $(_CFLAGS)

install: $(INSTBIN) $(INSTLIB)
$(INSTBIN): $(BIN)
	if [ "$(INSTDIR)" == "" ]; then echo "ERROR: INSTDIR not set" && exit 1; fi
	install -D $< $@
$(INSTLIB): $(LIB)
	if [ "$(INSTDIR)" == "" ]; then echo "ERROR: INSTDIR not set" && exit 1; fi
	install -D $< $@

valid:
long:

clean:
	rm -f $(OBJFILES) $(ASMOBJFILES) $(BIN)
endif
