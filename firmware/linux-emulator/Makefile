SRCDIR   := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
SRCDIRS  := $(SRCDIR)
SRCFILES += $(SRCDIR)/../../platform/mppa/odp_rpc.c
_LDFLAGS = -lmppapower -lmppanoc -lmpparouting

include $(SRCDIR)/../Makefile
