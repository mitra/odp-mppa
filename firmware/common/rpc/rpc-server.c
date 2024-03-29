#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <HAL/hal/hal.h>
#include <odp/rpc/rpc.h>
#include <odp/rpc/api.h>

#include <mppa_bsp.h>
#include <mppa_routing.h>
#include <mppa_noc.h>
#include "rpc-server.h"
#include "internal/rpc-server.h"
#include "internal/cache.h"

#define RPC_PKT_SIZE (sizeof(odp_rpc_t) + RPC_MAX_PAYLOAD)

odp_rpc_handler_t __rpc_handlers[ODP_RPC_N_CLASS];
int __n_rpc_handlers;
static uint64_t __rpc_ev_masks[BSP_NB_DMA_IO_MAX][4];
static uint64_t __rpc_fair_masks[BSP_NB_DMA_IO_MAX][4];

static struct {
	char    recv_buf[RPC_PKT_SIZE];
} g_clus_priv[RPC_MAX_CLIENTS]
#ifndef K1B_EXPLORER
 __attribute__ ((aligned(64), section(".upper_internal_memory")))
#endif
;

static inline int rxToMsg(unsigned ifId, unsigned tag,
			   odp_rpc_t **msg, uint8_t **payload)
{
	int remoteClus;
#if defined(K1B_EXPLORER)
	(void)ifId;
	remoteClus = (tag - RPC_BASE_RX);
#else
	int locIfId = ifId - 4;
	remoteClus = 4 * locIfId + ((tag - RPC_BASE_RX)) / 4 * 16 + ((tag - RPC_BASE_RX) % 4);
#endif

	odp_rpc_t *cmd = (void*)g_clus_priv[remoteClus].recv_buf;
	*msg = cmd;
	INVALIDATE(cmd);

	if(payload && cmd->data_len > 0) {
		*payload = (uint8_t*)(cmd + 1);
		INVALIDATE_AREA(*payload, cmd->data_len);
	}
#ifdef VERBOSE
	odp_rpc_print_msg(cmd);
#endif

	return remoteClus;
}

static int cluster_init_dnoc_rx(int clus_id)
{
	mppa_noc_ret_t ret;
	int ifId;
	int rxId;

	ifId = get_rpc_local_dma_id(clus_id);
	rxId = get_rpc_tag_id(clus_id);
	__rpc_ev_masks[ifId][rxId / 64] |= 1ULL << (rxId % 64);

	/* DNoC */
	ret = mppa_noc_dnoc_rx_alloc(ifId, rxId);
	if (ret != MPPA_NOC_RET_SUCCESS)
		return 1;

	mppa_noc_dnoc_rx_configuration_t conf = {
		.buffer_base = (uintptr_t)g_clus_priv[clus_id].recv_buf,
		.buffer_size = RPC_PKT_SIZE,
		.current_offset = 0,
		.item_counter = 0,
		.item_reload = 0,
		.reload_mode = MPPA_NOC_RX_RELOAD_MODE_INCR_DATA_NOTIF,
		.activation = MPPA_NOC_ACTIVATED,
		.counter_id = 0
	};

	ret = mppa_noc_dnoc_rx_configure(ifId, rxId, conf);
	if (ret != MPPA_NOC_RET_SUCCESS)
		return 1;

	return 0;
}

static int get_if_rx_id(unsigned interface_id)
{
	int i;

	mppa_noc_dnoc_rx_bitmask_t bitmask = mppa_noc_dnoc_rx_get_events_bitmask(interface_id);
	for (i = 0; i < 3; ++i) {
		bitmask.bitmask[i] &= __rpc_fair_masks[interface_id][i];
		if (bitmask.bitmask[i]) {
			int rx_id = __k1_ctzdl(bitmask.bitmask[i]) + i * 8 * sizeof(bitmask.bitmask[i]);
			int ev_counter = mppa_noc_dnoc_rx_lac_event_counter(interface_id, rx_id);

			__rpc_fair_masks[interface_id][i] ^= (1ULL << rx_id);
			assert(ev_counter > 0);
			return rx_id;
		}
	}
	for (i = 0; i < 3; ++i)
		__rpc_fair_masks[interface_id][i] = __rpc_ev_masks[interface_id][i];
	return -1;
}

static int dma_id;

static
int odp_rpc_server_poll_msg(odp_rpc_t **msg, uint8_t **payload)
{
	const int base_if = 4;
	int idx;

	for (idx = 0; idx < BSP_NB_DMA_IO; ++idx) {
		int if_id = dma_id + base_if;
		int tag = get_if_rx_id(if_id);

		dma_id++;
		if(dma_id == BSP_NB_DMA_IO)
			dma_id = 0;
		if(tag < 0)
			continue;

		/* Received a message */
		return rxToMsg(if_id, tag, msg, payload);
	}
	return -1;
}

int odp_rpc_server_ack(odp_rpc_t * msg, odp_rpc_ack_t ack,
		       const uint8_t *payload, uint16_t payload_len)
{
	msg->ack = 1;
	msg->data_len = payload_len;
	msg->inl_data = ack.inl_data;

	unsigned interface = get_rpc_local_dma_id(msg->dma_id);
	return odp_rpc_send_msg(interface, msg->dma_id, msg->dnoc_tag, msg, payload);
}

static
int odp_rpc_server_handle(odp_rpc_t ** unhandled_msg)
{
	int remoteClus;
	odp_rpc_t *msg;
	uint8_t *payload = NULL;

	remoteClus = odp_rpc_server_poll_msg(&msg, &payload);
	if(remoteClus >= 0) {
		*unhandled_msg = msg;

		if (msg->pkt_class >= ODP_RPC_N_CLASS ||
		    __rpc_handlers[msg->pkt_class] == NULL) {
			/* Unhandled message type */
			*unhandled_msg = msg;
			return -ODP_RPC_ERR_BAD_COS;
		}
		return __rpc_handlers[msg->pkt_class](remoteClus, msg, payload);
	}

	/* No message */
	return -ODP_RPC_ERR_NONE;
}

static int bas_rpc_handler(unsigned remoteClus, odp_rpc_t *msg, uint8_t *payload)
{
	odp_rpc_ack_t ack = ODP_RPC_CMD_ACK_INITIALIZER;

	if (msg->pkt_class != ODP_RPC_CLASS_BAS)
		return -ODP_RPC_ERR_INTERNAL_ERROR;
	if (msg->cos_version != ODP_RPC_BAS_VERSION)
		return -ODP_RPC_ERR_VERSION_MISMATCH;

	(void)remoteClus;
	(void)payload;

	switch (msg->pkt_subtype){
	case ODP_RPC_CMD_BAS_PING:
		ack.status = 0;
		break;
	default:
		return -ODP_RPC_ERR_BAD_SUBTYPE;
	}
	odp_rpc_server_ack(msg, ack, NULL, 0);
	return -ODP_RPC_ERR_NONE;
}

void  __attribute__ ((constructor)) __bas_rpc_constructor()
{
	__rpc_handlers[ODP_RPC_CLASS_BAS] = bas_rpc_handler;
}


/** Boot ack */
static char rpc_server_stack[8192] __attribute__ ((aligned(64), section(".upper_internal_memory")));
int odp_rpc_server_thread()
{
	if (__k1_get_cluster_id() != 160 &&
	    __k1_get_cluster_id() != 224) {
		return -1;
	}

	while (1) {
		odp_rpc_t *msg;
		int ret;
		ret = odp_rpc_server_handle(&msg);
		if (ret < 0) {
			unsigned interface = get_rpc_local_dma_id(msg->dma_id);

			/* Error in handling. Send a RPC command with error flag */
			msg->ack = 1;
			msg->rpc_err = -ret;
			msg->data_len = 0;
			odp_rpc_send_msg(interface, msg->dma_id, msg->dnoc_tag, msg, NULL);
		}
	}
	return 0;
}

int odp_rpc_server_start(void)
{
	int i;

	for (i = 0; i < RPC_MAX_CLIENTS; ++i) {
		int ret = cluster_init_dnoc_rx(i);
		if (ret)
			return ret;
	}

#ifdef VERBOSE
	printf("[RPC] Server started...\n");
#endif
	g_rpc_init = 1;

	if (__k1_get_cluster_id() == 128 ||
	    __k1_get_cluster_id() == 192) {
		/* Boot only if we are in an IODDR */
		_K1_PE_STACK_ADDRESS[4] = rpc_server_stack + sizeof(rpc_server_stack) - 16;
		_K1_PE_START_ADDRESS[4] = &odp_rpc_server_thread;
		_K1_PE_ARGS_ADDRESS[4] = 0;

		__builtin_k1_dinval();
		__builtin_k1_wpurge();
		__builtin_k1_fence();
		__k1_poweron(4);
	}

	return 0;
}
