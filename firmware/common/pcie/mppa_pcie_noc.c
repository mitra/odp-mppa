#include <stdio.h>
#include <string.h>
#include <mppa_noc.h>
#include <mppa_routing.h>
#include "mppa_pcie_noc.h"
#include "mppa_pcie_eth.h"
#include "mppa_pcie_buf_alloc.h"
#include "HAL/hal/hal.h"

#define DDR_BASE_ADDR			0x80000000
#define DIRECTORY_SIZE			(32 * 1024 * 1024)

#define DDR_BUFFER_BASE_ADDR		(DDR_BASE_ADDR + DIRECTORY_SIZE)

#define MPPA_PCIE_ETH_NOC_PKT_COUNT	16

/**
 *
 */
#define MPPA_PCIE_MULTIBUF_PKT_SIZE	(9*1024)
/**
 * 4 packets per multi buffer
 */
#define MPPA_PCIE_MULTIBUF_PKT_COUNT	4
#define MPPA_PCIE_MULTIBUF_SIZE		(MPPA_PCIE_MULTIBUF_PKT_COUNT * MPPA_PCIE_MULTIBUF_PKT_SIZE)


#define MPPA_PCIE_MULTIBUF_COUNT	64

/**
 * Maximum count of usable interface
 */
#define MPPA_PCIE_USABLE_DNOC_IF	4
#define MPPA_PCIE_RM_COUNT		4

#define RX_RM_START		1
#define RX_RM_COUNT		2
#define RX_RM_STACK_SIZE	(0x2000 / (sizeof(uint64_t)))

static void *g_pkt_base_addr = (void *) DDR_BUFFER_BASE_ADDR;

struct mppa_pcie_eth_dnoc_tx_cfg g_mppa_pcie_tx_cfg[BSP_NB_IOCLUSTER_MAX][BSP_DNOC_TX_PACKETSHAPER_NB_MAX] = {{{0}}};

buffer_ring_t g_buf_pool;

/**
 * Stacks for RX_RM
 */
struct rm_rx_ctx {
	uint64_t stack[RX_RM_STACK_SIZE];
	volatile uint32_t ready;
};

static struct rm_rx_ctx g_rm_ctx[MPPA_PCIE_RM_COUNT];

static void
mppa_pcie_rx_rm_func()
{
	g_rm_ctx[__k1_get_cpu_id()].ready = 1;
	while(1) {
		
	};
}

void
mppa_pcie_noc_start_rx_rm()
{
	unsigned int rm_num;
	for (rm_num = RX_RM_START; rm_num < RX_RM_START + RX_RM_COUNT; rm_num++ ){

		/* Init with scratchpad size */
		_K1_PE_STACK_ADDRESS[rm_num] = &g_rm_ctx[rm_num].stack[RX_RM_STACK_SIZE - 16];
		_K1_PE_START_ADDRESS[rm_num] = &mppa_pcie_rx_rm_func;
		_K1_PE_ARGS_ADDRESS[rm_num] = 0;

		__builtin_k1_dinval();
		__builtin_k1_wpurge();
		__builtin_k1_fence();

		printf("Powering RM %d\n", rm_num);
		__k1_poweron(rm_num);
	}

	for (rm_num = RX_RM_START; rm_num < RX_RM_START + RX_RM_COUNT; rm_num++ ){
		while(!g_rm_ctx[rm_num].ready);
		printf("RM %d ready\n", rm_num);
	}
}


int mppa_pcie_noc_init_buff_pool()
{
	mppa_pcie_noc_rx_buf_t **buf_pool;
	mppa_pcie_noc_rx_buf_t *bufs[MPPA_PCIE_MULTIBUF_COUNT];
	int i;
	uint32_t buf_left;

	buf_pool = calloc(MPPA_PCIE_MULTIBUF_COUNT, sizeof(mppa_pcie_noc_rx_buf_t *));
	if (!buf_pool) {
		printf("Failed to alloc pool descriptor\n");
		return 1;
	}

	buffer_ring_init(&g_buf_pool, buf_pool, MPPA_PCIE_MULTIBUF_COUNT);

	for (i = 0; i < MPPA_PCIE_MULTIBUF_COUNT; i++) {
		bufs[i] = (mppa_pcie_noc_rx_buf_t *) g_pkt_base_addr;
		g_pkt_base_addr += sizeof(mppa_pcie_noc_rx_buf_t);
		
		bufs[i]->buf_addr = (void *) g_pkt_base_addr;
		g_pkt_base_addr += MPPA_PCIE_MULTIBUF_SIZE;
	}

	buffer_ring_push_multi(&g_buf_pool, bufs, MPPA_PCIE_MULTIBUF_COUNT, &buf_left);
	printf("Allocation done\n");
	return 0;
}

int mppa_pcie_eth_noc_init()
{
	int i;

	for(i = 0; i < BSP_NB_DMA_IO_MAX; i++)
		mppa_noc_interrupt_line_disable(i, MPPA_NOC_INTERRUPT_LINE_DNOC_TX);

	//~ mppa_pcie_noc_init_buff_pool();

	//~ mppa_pcie_noc_start_rx_rm();

	return 0;
}

static int mppa_pcie_eth_setup_tx(unsigned int iface_id, unsigned int *tx_id, unsigned int cluster_id, unsigned int rx_id)
{
	mppa_noc_ret_t nret;
	mppa_routing_ret_t rret;
	mppa_dnoc_header_t header;
	mppa_dnoc_channel_config_t config;

	/* Configure the TX for PCIe */
	nret = mppa_noc_dnoc_tx_alloc_auto(iface_id, tx_id, MPPA_NOC_NON_BLOCKING);
	if (nret) {
		printf("Tx alloc failed\n");
		return 1;
	}

	MPPA_NOC_DNOC_TX_CONFIG_INITIALIZER_DEFAULT(config, 0);

	rret = mppa_routing_get_dnoc_unicast_route(__k1_get_cluster_id() + iface_id, cluster_id, &config, &header);
	if (rret) {
		printf("Routing failed\n");
		return 1;
	}

	header._.tag = rx_id;
	header._.valid = 1;

	nret = mppa_noc_dnoc_tx_configure(iface_id, *tx_id, header, config);
	if (nret) {
		printf("Tx configure failed\n");
		return 1;
	}

	return 0;
}

static int mppa_pcie_eth_setup_rx(int if_id, unsigned int *rx_id)
{
	unsigned int buf_size = MPPA_PCIE_ETH_DEFAULT_MTU * MPPA_PCIE_ETH_NOC_PKT_COUNT;
	mppa_noc_ret_t ret;
	mppa_noc_dnoc_rx_configuration_t conf = MPPA_NOC_DNOC_RX_CONFIGURATION_INIT;

	ret = mppa_noc_dnoc_rx_alloc_auto(if_id, rx_id, MPPA_NOC_NON_BLOCKING);
	if(ret) {
		fprintf(stderr, "[PCIe] Error: Failed to find an available Rx on if %d\n", if_id);
		return 1;
	}

	conf.buffer_base = DDR_BASE_ADDR;
	conf.buffer_size = buf_size;
	g_pkt_base_addr += buf_size;

	conf.reload_mode = MPPA_NOC_RX_RELOAD_MODE_INCR_DATA_NOTIF;
	conf.activation = MPPA_NOC_ACTIVATED;

	ret = mppa_noc_dnoc_rx_configure(if_id, *rx_id, conf);
	if (ret)
		return 1;

	return 0;
}

odp_rpc_cmd_ack_t mppa_pcie_eth_open(unsigned remoteClus, odp_rpc_t * msg)
{
	odp_rpc_cmd_pcie_open_t open_cmd = {.inl_data = msg->inl_data};
	odp_rpc_cmd_ack_t ack = ODP_RPC_CMD_ACK_INITIALIZER;
	struct mppa_pcie_eth_dnoc_tx_cfg *tx_cfg;
	int if_id = remoteClus % MPPA_PCIE_USABLE_DNOC_IF;
	unsigned int tx_id, rx_id;

	printf("Received request to open PCIe\n");
	int ret = mppa_pcie_eth_setup_tx(if_id, &tx_id, remoteClus, open_cmd.min_rx);
	if (ret) {
		fprintf(stderr, "[PCIe] Error: Failed to setup tx on if %d\n", if_id);
		return ack;
	}

	ret = mppa_pcie_eth_setup_rx(if_id, &rx_id);
	if (ret)
		return ack;

	tx_cfg = &g_mppa_pcie_tx_cfg[if_id][tx_id];
	tx_cfg->opened = 1; 
	tx_cfg->cluster = remoteClus;
	tx_cfg->rx_id = rx_id;
	tx_cfg->fifo_addr = &mppa_dnoc[if_id]->tx_ports[tx_id].push_data;
	tx_cfg->pcie_eth_if = open_cmd.pcie_eth_if_id; 
	tx_cfg->mtu = open_cmd.pkt_size;

	ret = mppa_pcie_eth_add_forward(open_cmd.pcie_eth_if_id, &g_mppa_pcie_tx_cfg[if_id][tx_id]);
	if (ret)
		return ack;

	ack.cmd.pcie_open.tx_tag = rx_id;
	ack.cmd.pcie_open.tx_if = __k1_get_cluster_id() + if_id;
	/* FIXME, we send the same MTU as the one received */
	ack.cmd.pcie_open.mtu = open_cmd.pkt_size;
	memcpy(ack.cmd.pcie_open.mac, g_pcie_eth_control.configs[open_cmd.pcie_eth_if_id].mac_addr, MAC_ADDR_LEN);
	ack.status = 0;

	return ack;
}

odp_rpc_cmd_ack_t mppa_pcie_eth_close(__attribute__((unused)) unsigned remoteClus, __attribute__((unused)) odp_rpc_t * msg)
{
	odp_rpc_cmd_ack_t ack = ODP_RPC_CMD_ACK_INITIALIZER;
	ack.status = 0;

	return ack;
}

static int pcie_rpc_handler(unsigned remoteClus, odp_rpc_t *msg, uint8_t *payload)
{
	odp_rpc_cmd_ack_t ack = ODP_RPC_CMD_ACK_INITIALIZER;

	(void)payload;
	switch (msg->pkt_type){
	case ODP_RPC_CMD_PCIE_OPEN:
		ack = mppa_pcie_eth_open(remoteClus, msg);
		break;
	case ODP_RPC_CMD_PCIE_CLOS:
		ack = mppa_pcie_eth_close(remoteClus, msg);
		break;
	default:
		return -1;
	}
	odp_rpc_server_ack(msg, ack);
	return 0;
}

void  __attribute__ ((constructor)) __pcie_rpc_constructor()
{
	if(__n_rpc_handlers < MAX_RPC_HANDLERS) {
		__rpc_handlers[__n_rpc_handlers++] = pcie_rpc_handler;
	} else {
		fprintf(stderr, "Failed to register PCIE RPC handlers\n");
		exit(EXIT_FAILURE);
	}
}