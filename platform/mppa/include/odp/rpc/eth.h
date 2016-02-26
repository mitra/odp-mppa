#ifndef __ODP_RPC_ETH_H__
#define __ODP_RPC_ETH_H__

#include <odp/rpc/defines.h>

/** Length of a mac address */
#define ETH_ALEN 6

#define ODP_RPC_CMD_LIST_ETH										\
	ODP_RPC_CMD_ETH_OPEN     /**< ETH: Configure trafic forward to/from Eth in IOETH */,		\
		ODP_RPC_CMD_ETH_CLOS     /**< ETH: Free forward resources to/from Eth in IOETH */,	\
		ODP_RPC_CMD_ETH_PROMISC  /**< ETH: KSet/Clear promisc mode */,				\
		ODP_RPC_CMD_ETH_OPEN_DEF /**< ETH: Forward unmatch Rx traffic to a cluster */,		\
		ODP_RPC_CMD_ETH_CLOS_DEF /**< ETH: Stop forwarding unmatch Rx traffic to a cluster */,	\
		ODP_RPC_CMD_ETH_DUAL_MAC /**< ETH: Enable dual-mac mode (ODP + Linux) */,		\
		ODP_RPC_CMD_ETH_STATE    /**< ETH: Start or Stop trafic forwarding */

#define ODP_RPC_CMD_NAMES_ETH			\
	"ETH OPEN",				\
		"ETH CLOSE",			\
		"ETH PROMISC",			\
		"ETH OPEN FALLTHROUGH",		\
		"ETH CLOSE FALLTHROUGH",	\
		"ETH DUAL MAC",			\
		"ETH SET STATE"

#define ODP_RPC_ACK_LIST_ETH odp_rpc_ack_eth_open_t eth_open;
/**
 * Command for ODP_RPC_CMD_ETH_OPEN
 */
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
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_open_t);

/**
 * Ack inline for ODP_RPC_CMD_ETH_OPEN
 */
typedef struct {
	uint16_t tx_if;	/* IO Cluster id */
	uint8_t  tx_tag;	/* Tag of the IO Cluster rx */
	uint8_t  mac[ETH_ALEN];
	uint16_t mtu;
} odp_rpc_ack_eth_open_t;

/**
 * Command for ODP_RPC_CMD_ETH_OPEN_DEF
 */
typedef odp_rpc_cmd_eth_open_t odp_rpc_cmd_eth_open_def_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_open_def_t);

/**
 * Command for ODP_RPC_CMD_ETH_PROMIS
 */
typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
		uint8_t enabled : 1;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_promisc_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_promisc_t);

/**
 * Command for ODP_RPC_CMD_ETH_STATE
 */
typedef odp_rpc_cmd_eth_promisc_t odp_rpc_cmd_eth_state_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_state_t);

/**
 * Command for ODP_RPC_CMD_ETH_CLOS
 */
typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_clos_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_clos_t);

/**
 * Command for ODP_RPC_CMD_ETH_CLOS_DEF
 */
typedef odp_rpc_cmd_eth_clos_t odp_rpc_cmd_eth_clos_def_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_clos_def_t);

/**
 * Command for ODP_RPC_CMD_ETH_DUAL_MAC
 */
typedef union {
	struct {
		uint8_t enabled : 1;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_dual_mac_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_dual_mac_t);


/*
 *
 * Payload
 *
 */

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
typedef struct pkt_rule_entry {
	uint64_t cmp_value;
	uint16_t offset;
	uint8_t  cmp_mask;
	uint8_t  hash_mask;
} pkt_rule_entry_t;

// a set of rules is like: hashpolicy=[P0{@6#0xfc}{@12/0xf0=32}][P2{@0/0xff=123456}{@20#0xff}]
// P (for priority) is optional, [0..7]
typedef struct pkt_rule {
	pkt_rule_entry_t entries[10];
	uint8_t nb_entries : 4;
	uint8_t priority   : 4;
} pkt_rule_t;

#endif /* __ODP_RPC_ETH_H__ */
