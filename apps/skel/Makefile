APP_DIR   := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

####################################
# Configure your project name here #
####################################
TEST_NAME := TemplateProject

####################################
# Select one of these platforms    #
####################################
ODP_ARCH := k1b-kalray-mos
#ODP_ARCH := k1b-kalray-nodeos
#ODP_ARCH := k1b-kalray-mos_simu
#ODP_ARCH := k1b-kalray-nodeos_simu

ODP_TOOLCHAIN_DIR ?= $(K1_TOOLCHAIN_DIR)
include $(ODP_TOOLCHAIN_DIR)/share/odp/build/apps/Makefile.apps

####################################
# Configure your flags here        #
####################################
cflags += -I$(APP_DIR)
ldflags +=

####################################
# Create cluster binaries and pick #
# the source and header it uses    #
####################################
odp_example-srcs := $(APP_DIR)/example.c
odp_example-nameinmpk := odp_example.kelf
cluster-bin += odp_example

####################################
#  GENERIC RULES. DO NOT EDIT !!   #
####################################
$(TEST_NAME)-objs += $(cluster-bin)
mppa-bin += $(TEST_NAME)
$(TEST_NAME)-multibinary := $(TEST_NAME).mpk
mppa-tests += $(TEST_NAME)
$(TEST_NAME)-scripts := run.sh

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray
