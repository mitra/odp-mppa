#ifndef __ODP_RPC_ETH_H__
#define __ODP_RPC_ETH_H__

#include <odp/rpc/defines.h>

/** Version of the ETH CoS */
#define ODP_RPC_ETH_VERSION 0x1

/** Length of a mac address */
#define ETH_ALEN 6

typedef enum {
	ODP_RPC_CMD_ETH_OPEN     /**< ETH: Configure trafic forward to/from Eth in IOETH */,
	ODP_RPC_CMD_ETH_CLOS     /**< ETH: Free forward resources to/from Eth in IOETH */,
	ODP_RPC_CMD_ETH_PROMISC  /**< ETH: KSet/Clear promisc mode */,
	ODP_RPC_CMD_ETH_OPEN_DEF /**< ETH: Forward unmatch Rx traffic to a cluster */,
	ODP_RPC_CMD_ETH_CLOS_DEF /**< ETH: Stop forwarding unmatch Rx traffic to a cluster */,
	ODP_RPC_CMD_ETH_DUAL_MAC /**< ETH: Enable dual-mac mode (ODP + Linux) */,
	ODP_RPC_CMD_ETH_STATE    /**< ETH: Start or Stop trafic forwarding */,
	ODP_RPC_CMD_ETH_GET_STAT /**< ETH: GEt link status and statistics */,
	ODP_RPC_CMD_ETH_N_CMD
} odp_rpc_cmd_eth_e;

#define ODP_RPC_CMD_NAMES_ETH			\
	"ETH OPEN",				\
		"ETH CLOSE",			\
		"ETH PROMISC",			\
		"ETH OPEN FALLTHROUGH",		\
		"ETH CLOSE FALLTHROUGH",	\
		"ETH DUAL MAC",			\
		"ETH SET STATE",		\
		"ETH GET STATS"

#define ODP_RPC_ACK_LIST_ETH				\
	odp_rpc_ack_eth_open_t eth_open;		\
	odp_rpc_ack_eth_get_stat_t eth_get_stat;

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

/**
 * Command for ODP_RPC_CMD_ETH_GET_STAT
 */
typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
		uint8_t link_stats : 1; /* If enable returns link stats
					 * counters in payload. If disabled
					 * just return link status */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_eth_get_stat_t;
ODP_RPC_CHECK_STRUCT_SIZE(odp_rpc_cmd_eth_get_stat_t);

/**
 * Ack inline for ODP_RPC_CMD_GET_STAT
 */
typedef struct {
	uint16_t link_status;
} odp_rpc_ack_eth_get_stat_t;

/**
 * Payload for ack of ODP_RPC_CMD_GET_STAT.
 *
 * Follows RFCs for Management Information Base
 * (MIB)for use with network management protocols in the Internet community:
 * https://tools.ietf.org/html/rfc3635
 * https://tools.ietf.org/html/rfc2863
 * https://tools.ietf.org/html/rfc2819
 */
typedef struct {
	/**
	 * The number of octets in valid MAC frames received on this interface,
	 * including the MAC header and FCS. See ifHCInOctets counter
	 * description in RFC 3635 for details.
	 */
	uint64_t in_octets;

	/**
	 * The number of packets, delivered by this sub-layer to a higher
	 * (sub-)layer, which were not addressed to a multicast or broadcast
	 * address at this sub-layer. See ifHCInUcastPkts in RFC 2863, RFC 3635.
	 */
	uint64_t in_ucast_pkts;

	/**
	 * The number of inbound packets which were chosen to be discarded
	 * even though no errors had been detected to preven their being
	 * deliverable to a higher-layer protocol.  One possible reason for
	 * discarding such a packet could be to free up buffer space.
	 * See ifInDiscards in RFC 2863.
	 */
	uint64_t in_discards;

	/**
	 * The sum for this interface of AlignmentErrors, FCSErrors, FrameTooLongs,
	 * InternalMacReceiveErrors. See ifInErrors in RFC 3635.
	 */
	uint64_t in_errors;

	/**
	 * For packet-oriented interfaces, the number of packets received via
	 * the interface which were discarded because of an unknown or
	 * unsupported protocol.  For character-oriented or fixed-length
	 * interfaces that support protocol multiplexing the number of
	 * transmission units received via the interface which were discarded
	 * because of an unknown or unsupported protocol.  For any interface
	 * that does not support protocol multiplexing, this counter will always
	 * be 0. See ifInUnknownProtos in RFC 2863, RFC 3635.
	 */
	uint64_t in_unknown_protos;

	/**
	 * The number of octets transmitted in valid MAC frames on this
	 * interface, including the MAC header and FCS.  This does include
	 * the number of octets in valid MAC Control frames transmitted on
	 * this interface. See ifHCOutOctets in RFC 3635.
	 */
	uint64_t out_octets;

	/**
	 * The total number of packets that higher-level protocols requested
	 * be transmitted, and which were not addressed to a multicast or
	 * broadcast address at this sub-layer, including those that were
	 * discarded or not sent. does not include MAC Control frames.
	 * See ifHCOutUcastPkts RFC 2863, 3635.
	 */
	uint64_t out_ucast_pkts;

	/**
	 * The number of outbound packets which were chosen to be discarded
	 * even though no errors had been detected to prevent their being
	 * transmitted.  One possible reason for discarding such a packet could
	 * be to free up buffer space.  See  OutDiscards in  RFC 2863.
	 */
	uint64_t out_discards;

	/**
	 * The sum for this interface of SQETestErrors, LateCollisions,
	 * ExcessiveCollisions, InternalMacTransmitErrors and
	 * CarrierSenseErrors. See ifOutErrors in RFC 3635.
	 */
	uint64_t out_errors;
}  odp_rpc_payload_eth_get_stat_t;



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
