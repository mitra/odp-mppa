#ifndef __ODP_RPC_RPC_H__
#define __ODP_RPC_RPC_H__

#include <odp/rpc/defines.h>
#include <odp/rpc/helpers.h>
#include <odp/rpc/api.h>

typedef struct {
	uint64_t data[4];
} odp_rpc_inl_data_t;

/* Command modules */
#include <odp/rpc/bas.h>
#include <odp/rpc/eth.h>
#include <odp/rpc/pcie.h>
#include <odp/rpc/c2c.h>
#include <odp/rpc/rnd.h>

typedef struct odp_rpc {
	uint16_t pkt_type;
	uint16_t data_len;       /* Packet is data len * 8B long. data_len < RPC_MAX_PAYLOAD / 8 */
	uint8_t  dma_id;         /* Source cluster ID */
	uint8_t  dnoc_tag;       /* Source Rx tag for reply */
	union {
		struct {
			uint8_t ack : 1;
		};
		uint16_t flags;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_t;

typedef enum {
	/* Import commands from modules */
	ODP_RPC_CMD_LIST_BAS,
	ODP_RPC_CMD_LIST_ETH,
	ODP_RPC_CMD_LIST_PCIE,
	ODP_RPC_CMD_LIST_C2C,
	ODP_RPC_CMD_LIST_RND,
	ODP_RPC_CMD_N_CMD        /**< Number of commands */
} odp_rpc_cmd_e;

typedef union {
	struct {
		uint8_t status;
		union {
			uint8_t foo;                    /* Dummy entry for init */
			ODP_RPC_ACK_LIST_BAS
			ODP_RPC_ACK_LIST_ETH
			ODP_RPC_ACK_LIST_PCIE
			ODP_RPC_ACK_LIST_C2C
			ODP_RPC_ACK_LIST_RND
		} cmd;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_ack_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_ack_t);

#define ODP_RPC_CMD_ACK_INITIALIZER { .inl_data = { .data = { 0 }}, .cmd = { 0 }, .status = -1}

/** RPC client status */
extern int g_rpc_init;

#endif /* __ODP_RPC_RPC_H__ */
