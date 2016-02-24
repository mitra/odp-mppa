CONFIGS :=
_CONFIGS :=

$(foreach FIRM, $(FIRMWARES), $(eval $(FIRM)_CONFIGS := ))
$(foreach FIRM, $(FIRMWARES), $(eval _$(FIRM)_CONFIGS := ))

#
#
#
# K1B Definitions
#
#
#
k1b-kalray-nodeos_developer_CONF_ENV    := CC=k1-nodeos-gcc  CXX=k1-nodeos-g++ STRIP=k1-strip
k1b-kalray-nodeos_developer_CONF_OPTS   :=
k1b-kalray-nodeos_developer_CFLAGS      := -march=k1b -DK1_NODEOS
k1b-kalray-nodeos_developer_LDFLAGS     :=
k1b-kalray-nodeos_developer_PLATFORM    := mppa
k1b-kalray-nodeos_developer_INC_DIR     := k1b-nodeos
k1b-kalray-nodeos_developer_MAKE_VALID  := -j1
k1b-kalray-nodeos_developer_INSTALL     := true
CONFIGS += k1b-kalray-nodeos_developer

k1b-kalray-nodeos_explorer_CONF_ENV    := CC=k1-nodeos-gcc  CXX=k1-nodeos-g++ STRIP=k1-strip
k1b-kalray-nodeos_explorer_CONF_OPTS   :=
k1b-kalray-nodeos_explorer_CFLAGS      := -march=k1b -DK1_NODEOS -mboard=explorer -DK1B_EXPLORER
k1b-kalray-nodeos_explorer_LDFLAGS     :=
k1b-kalray-nodeos_explorer_PLATFORM    := mppa
k1b-kalray-nodeos_explorer_INC_DIR     := k1b-nodeos
k1b-kalray-nodeos_explorer_MAKE_VALID  := -j1
k1b-kalray-nodeos_explorer_INSTALL     := true
_CONFIGS += k1b-kalray-nodeos_explorer

k1b-kalray-nodeos_simu_CONF_ENV    := CC=k1-nodeos-gcc  CXX=k1-nodeos-g++ STRIP=k1-strip
k1b-kalray-nodeos_simu_CONF_OPTS   :=
k1b-kalray-nodeos_simu_CFLAGS      := -march=k1b -DK1_NODEOS -DMAGIC_SCALL
k1b-kalray-nodeos_simu_LDFLAGS     :=
k1b-kalray-nodeos_simu_PLATFORM    := mppa
k1b-kalray-nodeos_simu_INC_DIR     := k1b-nodeos
k1b-kalray-nodeos_simu_MAKE_VALID  :=
k1b-kalray-nodeos_simu_INSTALL     := true
CONFIGS += k1b-kalray-nodeos_simu

k1b-kalray-nodeos_konic80_CONF_ENV    := CC=k1-nodeos-gcc  CXX=k1-nodeos-g++ STRIP=k1-strip
k1b-kalray-nodeos_konic80_CONF_OPTS   :=
k1b-kalray-nodeos_konic80_CFLAGS      := -march=k1b -DK1_NODEOS -mboard=konic80
k1b-kalray-nodeos_konic80_LDFLAGS     :=
k1b-kalray-nodeos_konic80_PLATFORM    := mppa
k1b-kalray-nodeos_konic80_INC_DIR     := k1b-nodeos
k1b-kalray-nodeos_konic80_MAKE_VALID  := -j1
k1b-kalray-nodeos_konic80_INSTALL     := true
CONFIGS += k1b-kalray-nodeos_konic80

k1b-kalray-mos_developer_CONF_ENV    := CC=k1-gcc  CXX=k1-g++ STRIP=k1-strip
k1b-kalray-mos_developer_CONF_OPTS   :=
k1b-kalray-mos_developer_CFLAGS      := -march=k1b -mhypervisor -DK1_MOS
k1b-kalray-mos_developer_LDFLAGS     := -mhypervisor -lvbsp -lutask -Wl,--defsym=USER_STACK_SIZE=5120
k1b-kalray-mos_developer_PLATFORM    := mppa
k1b-kalray-mos_developer_INC_DIR     := k1b-elf
k1b-kalray-mos_developer_MAKE_VALID  := -j1
k1b-kalray-mos_developer_INSTALL     := true
k1b-kalray-mos_developer_INSTALL_DOC := true
CONFIGS += k1b-kalray-mos_developer

k1b-kalray-mos_explorer_CONF_ENV    := CC=k1-gcc  CXX=k1-g++ STRIP=k1-strip
k1b-kalray-mos_explorer_CONF_OPTS   :=
k1b-kalray-mos_explorer_CFLAGS      := -march=k1b -mhypervisor -DK1_MOS -mboard=explorer -DK1B_EXPLORER
k1b-kalray-mos_explorer_LDFLAGS     := -mhypervisor -lvbsp -lutask -Wl,--defsym=USER_STACK_SIZE=5120
k1b-kalray-mos_explorer_PLATFORM    := mppa
k1b-kalray-mos_explorer_INC_DIR     := k1b-elf
k1b-kalray-mos_explorer_MAKE_VALID  := -j1
k1b-kalray-mos_explorer_INSTALL     := true
_CONFIGS += k1b-kalray-mos_explorer

k1b-kalray-mos_konic80_CONF_ENV    := CC=k1-gcc  CXX=k1-g++ STRIP=k1-strip
k1b-kalray-mos_konic80_CONF_OPTS   :=
k1b-kalray-mos_konic80_CFLAGS      := -march=k1b -mhypervisor -DK1_MOS -mboard=konic80
k1b-kalray-mos_konic80_LDFLAGS     := -mhypervisor -lvbsp -lutask -Wl,--defsym=USER_STACK_SIZE=5120
k1b-kalray-mos_konic80_PLATFORM    := mppa
k1b-kalray-mos_konic80_INC_DIR     := k1b-elf
k1b-kalray-mos_konic80_MAKE_VALID  := -j1
k1b-kalray-mos_konic80_INSTALL     := true
CONFIGS += k1b-kalray-mos_konic80

k1b-kalray-mos_simu_CONF_ENV    := CC=k1-gcc  CXX=k1-g++ STRIP=k1-strip
k1b-kalray-mos_simu_CONF_OPTS   :=
k1b-kalray-mos_simu_CFLAGS      := -march=k1b -mhypervisor -DK1_MOS -DMAGIC_SCALL
k1b-kalray-mos_simu_LDFLAGS     := -mhypervisor -lvbsp -lutask -Wl,--defsym=USER_STACK_SIZE=5120
k1b-kalray-mos_simu_PLATFORM    := mppa
k1b-kalray-mos_simu_INC_DIR     := k1b-elf
k1b-kalray-mos_simu_MAKE_VALID  :=
k1b-kalray-mos_simu_INSTALL     := true
CONFIGS += k1b-kalray-mos_simu

# Buid long for all BUT x86_64
LONG_CONFIGS := $(CONFIGS)
#
#
#
# x86 Definitions
#
#
#
x86_64-unknown-linux-gnu_CONF_ENV    :=
x86_64-unknown-linux-gnu_CONF_OPTS   :=
x86_64-unknown-linux-gnu_CFLAGS      :=
x86_64-unknown-linux-gnu_LDFLAGS     :=
x86_64-unknown-linux-gnu_PLATFORM    := linux-generic
x86_64-unknown-linux-gnu_INC_DIR     :=
x86_64-unknown-linux-gnu_MAKE_VALID  :=
x86_64-unknown-linux-gnu_INSTALL     := false
CONFIGS += x86_64-unknown-linux-gnu

#
#
#
# Firmware Definitions
#
#
#

#
# IOPCIE
#
k1b-kalray-ioddr_developer_CC          := k1-gcc
k1b-kalray-ioddr_developer_CFLAGS      := -march=k1b -mos=bare -mcore=k1bio -mcluster=ioddr -mprocessing=single
k1b-kalray-ioddr_developer_LDFLAGS     :=
k1b-kalray-ioddr_developer_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/developer/k1b
iopcie_CONFIGS += k1b-kalray-ioddr_developer

k1b-kalray-ioddr_konic80_CC          := k1-gcc
k1b-kalray-ioddr_konic80_CFLAGS      := -march=k1b -mos=bare -mcore=k1bio -mcluster=ioddr -mprocessing=single -mboard=konic80
k1b-kalray-ioddr_konic80_LDFLAGS     :=
k1b-kalray-ioddr_konic80_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/konic80/k1b
iopcie_CONFIGS += k1b-kalray-ioddr_konic80

k1b-kalray-ioddr_simu_CC          := k1-gcc
k1b-kalray-ioddr_simu_CFLAGS      := -march=k1b -mos=bare -mcore=k1bio -mcluster=ioddr -mprocessing=single -DMAGIC_SCALL
k1b-kalray-ioddr_simu_LDFLAGS     :=
k1b-kalray-ioddr_simu_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/simu/k1b
iopcie_CONFIGS += k1b-kalray-ioddr_simu

k1b-kalray-ioddr_explorer_CC          := k1-gcc
k1b-kalray-ioddr_explorer_CFLAGS      := -march=k1b -mos=bare -mcore=k1bio -mcluster=ioddr \
									  	 -mprocessing=single -mboard=explorer -DK1B_EXPLORER
k1b-kalray-ioddr_explorer_LDFLAGS     :=
k1b-kalray-ioddr_explorer_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/explorer/k1b
_iopcie_CONFIGS += k1b-kalray-ioddr_explorer

#
# IOUNIFIED
#
k1b-kalray-iounified_developer_CC          := k1-gcc
k1b-kalray-iounified_developer_CFLAGS      := -march=k1b -DK1_BARE -mcore=k1bio -mcluster=ioddr -mos=bare
k1b-kalray-iounified_developer_LDFLAGS     :=
k1b-kalray-iounified_developer_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/developer/k1b
iounified_CONFIGS += k1b-kalray-iounified_developer

k1b-kalray-iounified_simu_CC          := k1-gcc
k1b-kalray-iounified_simu_CFLAGS      := -march=k1b -DK1_BARE -mcore=k1bio -mcluster=ioddr -mos=bare -DMAGIC_SCALL
k1b-kalray-iounified_simu_LDFLAGS     :=
k1b-kalray-iounified_simu_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/simu/k1b
iounified_CONFIGS += k1b-kalray-iounified_simu

k1b-kalray-iounified_konic80_CC          := k1-gcc
k1b-kalray-iounified_konic80_CFLAGS      := -march=k1b -DK1_BARE -mcore=k1bio -mcluster=ioddr -mos=bare \
										 	-mboard=konic80
k1b-kalray-iounified_konic80_LDFLAGS     :=
k1b-kalray-iounified_konic80_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/konic80/k1b
iounified_CONFIGS += k1b-kalray-iounified_konic80


#
# IOETH
#
k1b-kalray-ioeth_developer_CC          := k1-gcc
k1b-kalray-ioeth_developer_CFLAGS      := -march=k1b -DK1_BARE -mcore=k1bio -mcluster=ioddr -mos=bare
k1b-kalray-ioeth_developer_LDFLAGS     :=
k1b-kalray-ioeth_developer_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/developer/k1b
ioeth_CONFIGS += k1b-kalray-ioeth_developer

k1b-kalray-ioeth_simu_CC          := k1-gcc
k1b-kalray-ioeth_simu_CFLAGS      := -march=k1b -DK1_BARE -mcore=k1bio -mcluster=ioddr -mos=bare -DMAGIC_SCALL
k1b-kalray-ioeth_simu_LDFLAGS     :=
k1b-kalray-ioeth_simu_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/simu/k1b
ioeth_CONFIGS += k1b-kalray-ioeth_simu

k1b-kalray-ioeth_konic80_CC          := k1-gcc
k1b-kalray-ioeth_konic80_CFLAGS      := -march=k1b -DK1_BARE -mcore=k1bio -mcluster=ioddr -mos=bare \
										 	-mboard=konic80
k1b-kalray-ioeth_konic80_LDFLAGS     :=
k1b-kalray-ioeth_konic80_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/konic80/k1b
ioeth_CONFIGS += k1b-kalray-ioeth_konic80

#
# Netdev loopback
#
k1b-kalray-netdev-loopback_CC          := k1-gcc
k1b-kalray-netdev-loopback_CFLAGS      := -march=k1b -mos=bare -mcore=k1bio -mcluster=ioddr -mprocessing=single
k1b-kalray-netdev-loopback_LDFLAGS     :=
k1b-kalray-netdev-loopback_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/developer/k1b
_netdev-loopback_CONFIGS += k1b-kalray-netdev-loopback

k1b-kalray-netdev-loopback_konic80_CC          := k1-gcc
k1b-kalray-netdev-loopback_konic80_CFLAGS      := -march=k1b -mos=bare -mcore=k1bio -mcluster=ioddr -mprocessing=single
k1b-kalray-netdev-loopback_konic80_LDFLAGS     :=
k1b-kalray-netdev-loopback_konic80_INSTDIR      = $(K1ST_DIR)/share/odp/firmware/konic80/k1b
_netdev-loopback_CONFIGS += k1b-kalray-netdev-loopback_konic80
