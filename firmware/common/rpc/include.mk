SRCDIRS  += $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
_CFLAGS  += -DRPC_FIRMWARE
_LDFLAGS += -Wl,--undefined=__bas_rpc_constructor

