DIR := $(dir $(lastword $(MAKEFILE_LIST)))
TEST_NAME := $(lastword $(subst /, ,$(DIR)))

include $(DIR)/../Makefile 
$(TEST_NAME)-srcs := $(DIR)/ring_com.c
cluster-bin += $(TEST_NAME)

$(TEST_NAME)_multibin-objs := $(IOPCIE_ELF) $(TEST_NAME)
mppa-bin += $(TEST_NAME)_multibin
$(TEST_NAME)-multibinary := $(TEST_NAME)_multibin.mpk

$(TEST_NAME)-sim-cmd := $(TEST_NAME)_multibin.mpk $(foreach i,$(shell seq 16),-c $(TEST_NAME))
#~ mppa-tests += $(TEST_NAME)
include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray
