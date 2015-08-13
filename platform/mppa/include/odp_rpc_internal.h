#ifndef __FIRMWARE__IOETH__RPC__H__
#define __FIRMWARE__IOETH__RPC__H__

#ifndef BSP_NB_DMA_IO_MAX
#define BSP_NB_DMA_IO_MAX 8
#endif

#define RPC_BASE_RX 10
#define RPC_MAX_PAYLOAD 128 /* max payload in bytes */

typedef struct {
	uint64_t data[4];
} odp_rpc_inl_data_t;

typedef struct {
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
	ODP_RPC_CMD_BAS_INVL = 0 /**< BASE: Invalid command. Skip */,
	ODP_RPC_CMD_BAS_PING     /**< BASE: Ping command. server sends back ack = 0 */,
	ODP_RPC_CMD_ETH_OPEN     /**< ETH: Forward Rx traffic to a cluster */,
	ODP_RPC_CMD_ETH_CLOS     /**< ETH: Stop forwarding Rx trafic to a cluster */
} odp_rpc_cmd_e;


typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
		uint8_t dma_if : 8;
		uint8_t min_rx : 8;
		uint8_t max_rx : 8;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_open_t;

typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_clos_t;

typedef union {
	struct {
		uint8_t status;
	};
	struct {
		uint8_t status;
		uint16_t eth_tx_if;
		uint8_t eth_tx_tag;
	} open;
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_ack_t;

static inline int odp_rpc_get_ioeth_dma_id(unsigned eth_slot, unsigned cluster_id){
	unsigned offset = cluster_id / 4;
#if defined(K1B_EXPLORER)
	/* Only DMA4 available on explorer + eth530 */
	offset = 0;
#endif
	switch(eth_slot){
	case 0:
		/* East */
		return 160 + offset;
	case 1:
		/* West */
		return 224 + offset;
	default:
		return -1;
	}
}

static inline int odp_rpc_get_ioeth_tag_id(unsigned eth_slot, unsigned cluster_id){
	(void) eth_slot;
	unsigned offset = cluster_id % 4;
#if defined(K1B_EXPLORER)
	/* Only DMA4 available on explorer + eth530 */
	offset = cluster_id;
#endif
	return RPC_BASE_RX + offset;
}

static inline int odp_rpc_get_ioddr_dma_id(unsigned ddr_id, unsigned cluster_id){
	switch(ddr_id){
	case 0:
		/* North */
		return 128 + cluster_id % 4;
	case 1:
		/* South */
		return 192 + cluster_id % 4;
	default:
		return -1;
	}
}

static inline int odp_rpc_get_ioddr_tag_id(unsigned ddr_id, unsigned cluster_id){
	(void) ddr_id;
	return RPC_BASE_RX + (cluster_id / 4);
}

int odp_rpc_client_init(void);
int odp_rpc_client_term(void);

void odp_rpc_print_msg(const odp_rpc_t * cmd);

int odp_rpc_send_msg(uint16_t local_interface, uint16_t dest_id, uint16_t dest_tag,
		     odp_rpc_t * cmd, void * payload);

/* Calls odp_rpc_send_msg after filling the required info for reply */
int odp_rpc_do_query(uint16_t dest_id, uint16_t dest_tag,
		     odp_rpc_t * cmd, void * payload);

int odp_rpc_wait_ack(odp_rpc_t ** cmd, void ** payload);

#endif /* __FIRMWARE__IOETH__RPC__H__ */