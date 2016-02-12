SRCDIRS  += $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
_CFLAGS  += -DRPC_FIRMWARE
_LDFLAGS += -Wl,--undefined=__bas_rpc_constructor -Wl,--defsym,_K1_PE_STACK_ADDRESS=__rm4_stack_start \
	-Wl,--allow-multiple-definition -Wl,--undefined=__boot_rm4

