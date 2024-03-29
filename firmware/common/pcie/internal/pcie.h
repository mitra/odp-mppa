#ifndef PCIE_INTERNAL__H
#define PCIE_INTERNAL__H

#include <string.h>
#include <odp/rpc/rpc.h>
#include "pcie.h"
#include "rpc-server.h"
#include "internal/rpc-server.h"
#include "ring.h"
#include "mppa_pcie_netdev.h"

#define MPPA_PCIE_USABLE_DNOC_IF	4

#define MPPA_PCIE_NOC_RX_NB 10

/**
 * PKT size
 */
#define MPPA_PCIE_MULTIBUF_PKT_SIZE	(9*1024)

/**
 * Packets per multi buffer
 */
#define MPPA_PCIE_MULTIBUF_PKT_COUNT	8
#define MPPA_PCIE_MULTIBUF_SIZE		(MPPA_PCIE_MULTIBUF_PKT_COUNT * MPPA_PCIE_MULTIBUF_PKT_SIZE)

#define MPPA_PCIE_MULTIBUF_COUNT	64

#define RX_RM_STACK_SIZE	(0x2000 / (sizeof(uint64_t)))

extern buffer_ring_t g_free_buf_pool;
extern buffer_ring_t g_full_buf_pool[MPPA_PCIE_ETH_MAX_INTERFACE_COUNT];

struct mppa_pcie_eth_dnoc_tx_cfg {
	int opened;
	unsigned int cluster;
	unsigned int mtu;
	volatile void *fifo_addr;
	unsigned int pcie_eth_if;
};

void
pcie_start_rx_rm();

void
pcie_start_tx_rm();

int pcie_setup_rx(int if_id, unsigned int rx_id, unsigned int pcie_eth_if);

static inline
int no_printf(__attribute__((unused)) const char *fmt , ...)
{
	return 0;
}

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#    define err_printf(fmt, args...) \
	printf("[ERR] %s:%d: " fmt,  __FILENAME__, __LINE__, ## args)

#if defined VERBOSE
#    define dbg_printf(fmt, args...) \
	printf("[DBG] %s:%d: " fmt,  __FILENAME__, __LINE__, ## args)
#else
#    define dbg_printf(fmt, args...)	no_printf(fmt, ## args)
#endif

#endif
