#ifndef __FIRMWARE__IOETH__RPC__H__
#define __FIRMWARE__IOETH__RPC__H__

#ifndef RPC_FIRMWARE
#include <odp/debug.h>
#else
#define _ODP_STATIC_ASSERT(x, y)
#endif

#ifndef INVALIDATE_AREA
#define INVALIDATE_AREA(p, s) do {	__k1_dcache_invalidate_mem_area((__k1_uintptr_t)(void*)p, s);	\
	}while(0)
#endif
#ifndef INVALIDATE
#define INVALIDATE(p) INVALIDATE_AREA((p), sizeof(*p))
#endif

#ifndef BSP_NB_DMA_IO_MAX
#define BSP_NB_DMA_IO_MAX 8
#endif

#define RPC_BASE_RX 10
#define RPC_MAX_PAYLOAD 168 * 8 /* max payload in bytes */

#define ETH_ALEN 6

enum {
	PKT_RULE_OPEN_SIGN =       '[',
	PKT_RULE_CLOSE_SIGN =      ']',
	PKT_RULE_PRIO_SIGN =       'P',
	PKT_ENTRY_OPEN_SIGN =      '{',
	PKT_ENTRY_CLOSE_SIGN =     '}',
	PKT_ENTRY_OFFSET_SIGN =    '@',
	PKT_ENTRY_CMP_VALUE_SIGN = '=',
	PKT_ENTRY_CMP_MASK_SIGN =  '/',
	PKT_ENTRY_HASH_MASK_SIGN = '#',
};

// cmp_mask and hash_mask are bytemask
typedef struct {
	uint64_t cmp_value;
	uint16_t offset;
	uint8_t  cmp_mask;
	uint8_t  hash_mask;
} pkt_rule_entry_t;

// a set of rules is like: hashpolicy=[P0{@6#0xfc}{@12/0xf0=32}][P2{@0/0xff=123456}{@20#0xff}]
// P (for priority) is optional, [0..7]
typedef struct {
	pkt_rule_entry_t entries[10];
	uint8_t nb_entries : 4;
	uint8_t priority   : 4;
} pkt_rule_t;

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
	ODP_RPC_CMD_BAS_SYNC     /**< SYNC: Sync command. */,
	ODP_RPC_CMD_BAS_EXIT     /**< SYNC: Sync command. */,

	ODP_RPC_CMD_ETH_OPEN     /**< ETH: Forward Rx traffic to a cluster */,
	ODP_RPC_CMD_ETH_CLOS     /**< ETH: Stop forwarding Rx trafic to a cluster */,
	ODP_RPC_CMD_ETH_PROMISC  /**< ETH: KSet/Clear promisc mode */,

	ODP_RPC_CMD_PCIE_OPEN    /**< PCIe: Forward Rx traffic to a cluster */,
	ODP_RPC_CMD_PCIE_CLOS    /**< PCIe: Stop forwarding Rx trafic to a cluster */,

	ODP_RPC_CMD_C2C_OPEN    /**< Cluster2Cluster: Declare as ready to receive message */,
	ODP_RPC_CMD_C2C_CLOS    /**< Cluster2Cluster: Declare as not ready to receive message */,
	ODP_RPC_CMD_C2C_QUERY   /**< Cluster2Cluster: Query the amount of creadit available for tx */,

	ODP_RPC_CMD_RND_GET      /**< RND: Get a buffer with random data generated on IO cluster */,
	ODP_RPC_CMD_N_CMD        /**< Number of commands */
} odp_rpc_cmd_e;


typedef union {
	struct {
		uint8_t ifId : 3;        /**< 0-3, 4 for 40G */
		uint8_t dma_if : 8;      /**< External address of the local DMA */
		uint8_t min_rx : 8;      /**< Minimum Rx Tag for Eth2Clus */
		uint8_t max_rx : 8;      /**< Maximum Rx tag for Eth2Clus */
		uint8_t loopback : 1;    /**< Put interface in loopback mode (no MAC) */
		uint8_t rx_enabled : 1;  /**< Enable packet reception (Eth2Clus). */
		uint8_t tx_enabled : 1;  /**< Enable packet transmission (Clus2Eth) */
		uint8_t jumbo : 1;       /**< Enable Jumbo frame support */
		uint8_t nb_rules : 4;    /**< Number of rule to the has policy.
								  *   Rules are provided in the payload */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_open_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_eth_open_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_ETH_OPEN_T__SIZE_ERROR");

typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
		uint8_t enabled : 1;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_promisc_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_eth_promisc_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_ETH_PROMISC_T__SIZE_ERROR");

typedef union {
	struct {
		uint16_t pkt_size;
		uint8_t pcie_eth_if_id; /* PCIe eth interface number */
		uint8_t min_rx;
		uint8_t max_rx;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_pcie_open_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_pcie_open_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_PCIE_OPEN_T__SIZE_ERROR");


typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_clos_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_eth_clos_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_ETH_CLOS_T__SIZE_ERROR");

typedef odp_rpc_cmd_eth_clos_t odp_rpc_cmd_pcie_clos_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_pcie_clos_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_PCIE_CLOS_T__SIZE_ERROR");

typedef union {
	struct {
		uint8_t rnd_data[31]; /* Filled with data in response packet */
		uint8_t rnd_len;  /* lenght of random data to send back */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_rnd_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_rnd_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_RND_T__SIZE_ERROR");

typedef union {
	struct {
		uint8_t cluster_id : 8;
		uint8_t min_rx     : 8;
		uint8_t max_rx     : 8;
		uint8_t rx_enabled : 1;
		uint8_t tx_enabled : 1;
		uint8_t cnoc_rx    : 8;
		uint16_t mtu       :16;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_c2c_open_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_c2c_open_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_C2C_OPEN_T__SIZE_ERROR");

typedef union {
	struct {
		uint8_t cluster_id : 8;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_c2c_clos_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_c2c_clos_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_C2C_CLOS_T__SIZE_ERROR");

typedef odp_rpc_cmd_c2c_clos_t odp_rpc_cmd_c2c_query_t;
/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_c2c_query_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_C2C_QUERY_T__SIZE_ERROR");

typedef union {
	struct {
		uint8_t status;
		union {
			uint8_t foo;                    /* Dummy entry for init */
			struct {
				uint16_t tx_if;	/* IO Cluster id */
				uint8_t  tx_tag;	/* Tag of the IO Cluster rx */
				uint8_t  mac[ETH_ALEN];
				uint16_t mtu;
			} eth_open;
			struct {
				uint16_t tx_if;	/* IO Cluster id */
				uint8_t  min_tx_tag;	/* Tag of the first IO Cluster rx */
				uint8_t  max_tx_tag;	/* Tag of the last IO Cluster rx */
				uint8_t  mac[ETH_ALEN];
				uint16_t mtu;
			} pcie_open;
			struct {
				uint8_t closed  : 1;
				uint8_t eacces  : 1;
				uint8_t min_rx  : 8;
				uint8_t max_rx  : 8;
				uint8_t cnoc_rx : 8;
				uint16_t mtu    : 16;
			} c2c_query;
		} cmd;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_ack_t;

#define ODP_RPC_CMD_ACK_INITIALIZER { .inl_data = { .data = { 0 }}, .cmd = { 0 }, .status = -1}

/** @internal Compile time assert */
_ODP_STATIC_ASSERT(sizeof(odp_rpc_cmd_ack_t) == sizeof(odp_rpc_inl_data_t), "ODP_RPC_CMD_ACK_T__SIZE_ERROR");

static inline int odp_rpc_densify_cluster_id(unsigned cluster_id){
	if(cluster_id == 128)
		cluster_id = 16;
	else if(cluster_id == 192)
		cluster_id = 19;
	return cluster_id;
}

static inline int odp_rpc_get_io_dma_id(unsigned io_id, unsigned cluster_id){
	int dense_id = odp_rpc_densify_cluster_id(cluster_id);
	int dma_offset = dense_id / 4;
#if defined(K1B_EXPLORER)
	dma_offset = 0;
#endif

	switch(io_id){
	case 0:
		/* North */
		return 160 + dma_offset;
	case 1:
		/* South */
		return 224 + dma_offset;
	default:
		return -1;
	}
}

static inline int odp_rpc_get_io_tag_id(unsigned cluster_id){
	int dense_id = odp_rpc_densify_cluster_id(cluster_id);
	int tag_offset = dense_id % 4;

#if defined(K1B_EXPLORER)
	/* Only DMA4 available on explorer + eth530 */
	tag_offset = dense_id;
#endif
	return RPC_BASE_RX + tag_offset;
}

extern int g_rpc_init;

int odp_rpc_client_init(void);
int odp_rpc_client_term(void);
int odp_rpc_client_get_default_server(void);

void odp_rpc_print_msg(const odp_rpc_t * cmd);

int odp_rpc_send_msg(uint16_t local_interface, uint16_t dest_id, uint16_t dest_tag,
		     odp_rpc_t * cmd, void * payload);

/* Calls odp_rpc_send_msg after filling the required info for reply */
int odp_rpc_do_query(uint16_t dest_id, uint16_t dest_tag,
		     odp_rpc_t * cmd, void * payload);

/*
 * Time out in cycles.
 * Retval: -1 = Error, 0 = Timeout, 1 = OK
 */
int odp_rpc_wait_ack(odp_rpc_t ** cmd, void ** payload, uint64_t timeout);

#define RPC_TIMEOUT_1S ((uint64_t)__bsp_frequency)

#endif /* __FIRMWARE__IOETH__RPC__H__ */
