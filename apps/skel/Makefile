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
$(realpath .)/firmware-ab01/firmware.kelf-nameinmpk := firmware-ab01.kelf
$(realpath .)/firmware-konic80/firmware.kelf-nameinmpk := firmware-konic80.kelf
$(TEST_NAME)-objs := $(cluster-bin) $(realpath .)/firmware-ab01/firmware.kelf $(realpath .)/firmware-konic80/firmware.kelf
mppa-bin += $(TEST_NAME)

$(TEST_NAME)-multibinary := $(TEST_NAME).mpk
mppa-tests += $(TEST_NAME)
$(TEST_NAME)-scripts := run.sh

pre-build-hooks := build-firmware
clean-hooks := clean-firmware

include $(K1_TOOLCHAIN_DIR)/share/make/Makefile.kalray

build-firmware:
	+mkdir -p ./firmware-ab01 && cd ./firmware-ab01 && \
	$(MAKE) -f $(APP_DIR)/firmware/Makefile CC="$(k1b-kalray-iounified_CC)" CFLAGS="$(k1b-kalray-iounified_CFLAGS)" \
	 LDFLAGS="$(k1b-kalray-iounified_LDFLAGS)" all
	+mkdir -p ./firmware-konic80 && cd ./firmware-konic80 && \
	$(MAKE) -f $(APP_DIR)/firmware/Makefile CC="$(k1b-kalray-iounified_konic80_CC)" CFLAGS="$(k1b-kalray-iounified_konic80_CFLAGS)" \
	 LDFLAGS="$(k1b-kalray-iounified_konic80_LDFLAGS)" all 
clean-firmware:
	if [ -d ./firmware-ab01 ]; then \
	$(MAKE) -f $(APP_DIR)/firmware/Makefile CC="$(k1b-kalray-iounified_CC)" CFLAGS="$(k1b-kalray-iounified_CFLAGS)" \
	 LDFLAGS="$(k1b-kalray-iounified_LDFLAGS)" clean; else true; fi
	if [ -d ./firmware-konic80 ]; then \
	$(MAKE) -f $(APP_DIR)/firmware/Makefile CC="$(k1b-kalray-iounified_konic80_CC)" CFLAGS="$(k1b-kalray-iounified_konic80_CFLAGS)" \
	 LDFLAGS="$(k1b-kalray-iounified_konic80_LDFLAGS)" clean; else true; fi

