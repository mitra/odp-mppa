SRCDIRS  += $(shell dirname $$(echo $(MAKEFILE_LIST) | awk '{ print $$NF}'))
_CFLAGS  += -I$(TOP_SRCDIR)/include
HDRFILES += $(ODP_INCDIR)/odp_rpc_internal.h
SRCFILES += $(ODP_SRCDIR)/odp_rpc.c