/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/**
 * @file
 *
 * ODP packet descriptor - implementation internal
 */

#ifndef ODP_PACKET_INTERNAL_H_
#define ODP_PACKET_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <odp/align.h>
#include <odp/debug.h>
#include <odp_buffer_internal.h>
#include <odp_pool_internal.h>
#include <odp_buffer_inlines.h>
#include <odp/packet.h>
#include <odp/packet_io.h>
#include <odp/crypto.h>
#include <odp_crypto_internal.h>

#define PACKET_JUMBO_LEN       (9 * 1024)

/**
 * Packet input & protocol flags
 */
typedef union {
	/* All input flags */
	uint32_t all;

	struct {
		uint32_t parsed_l2:1; /**< L2 parsed */
		uint32_t parsed_all:1;/**< Parsing complete */

		uint32_t l2:1;        /**< known L2 protocol present */
		uint32_t l3:1;        /**< known L3 protocol present */
		uint32_t l4:1;        /**< known L4 protocol present */

		uint32_t eth:1;       /**< Ethernet */
		uint32_t jumbo:1;     /**< Jumbo frame */
		uint32_t vlan:1;      /**< VLAN hdr found */
		uint32_t vlan_qinq:1; /**< Stacked VLAN found, QinQ */

		uint32_t snap:1;      /**< SNAP */
		uint32_t arp:1;       /**< ARP */

		uint32_t ipv4:1;      /**< IPv4 */
		uint32_t ipv6:1;      /**< IPv6 */
		uint32_t ipfrag:1;    /**< IP fragment */
		uint32_t ipopt:1;     /**< IP optional headers */
		uint32_t ipsec:1;     /**< IPSec decryption may be needed */

		uint32_t udp:1;       /**< UDP */
		uint32_t tcp:1;       /**< TCP */
		uint32_t tcpopt:1;    /**< TCP options present */
		uint32_t sctp:1;      /**< SCTP */
		uint32_t icmp:1;      /**< ICMP */
	};
} input_flags_t;

_ODP_STATIC_ASSERT(sizeof(input_flags_t) == sizeof(uint32_t),
		   "INPUT_FLAGS_SIZE_ERROR");

/**
 * Packet error flags
 */
typedef union {
	/* All error flags */
	uint16_t all;

	struct {
		/* Bitfield flags for each detected error */
		uint16_t app_error:1; /**< Error bit for application use */
		uint16_t frame_len:1; /**< Frame length error */
		uint16_t snap_len:1;  /**< Snap length error */
		uint16_t l2_chksum:1; /**< L2 checksum error, checks TBD */
		uint16_t ip_err:1;    /**< IP error,  checks TBD */
		uint16_t tcp_err:1;   /**< TCP error, checks TBD */
		uint16_t udp_err:1;   /**< UDP error, checks TBD */
	};
} error_flags_t;

/**
 * Packet output flags
 */
typedef union {
	/* All output flags */
	uint16_t all;

	struct {
		/* Bitfield flags for each output option */
		uint16_t l3_chksum_set:1; /**< L3 chksum bit is valid */
		uint16_t l3_chksum:1;     /**< L3 chksum override */
		uint16_t l4_chksum_set:1; /**< L3 chksum bit is valid */
		uint16_t l4_chksum:1;     /**< L4 chksum override  */
	};
} output_flags_t;

/**
 * Internal Packet header
 */
typedef struct {
	/* common buffer header */
	odp_buffer_hdr_t buf_hdr;

	input_flags_t  input_flags;
	error_flags_t  error_flags;
	output_flags_t output_flags;

	uint16_t l2_offset; /**< offset to L2 hdr, e.g. Eth */
	uint16_t vlan_s_tag;     /**< Parsed 1st VLAN header (S-TAG) */
	uint16_t vlan_c_tag;     /**< Parsed 2nd VLAN header (C-TAG) */

	uint16_t l3_offset; /**< offset to L3 hdr, e.g. IPv4, IPv6 */
	uint16_t l3_protocol;    /**< Parsed L3 protocol */

	uint16_t l4_offset; /**< offset to L4 hdr (TCP, UDP, SCTP, also ICMP) */
	uint16_t l4_protocol;    /**< Parsed L4 protocol */

	uint16_t payload_offset; /**< offset to payload */

	uint16_t frame_len;
	uint16_t headroom;
	uint16_t tailroom;

	odp_pktio_t input;

	uint32_t has_hash:1;      /**< Flow hash present */
	uint32_t flow_hash;      /**< Flow hash value */

	odp_crypto_generic_op_result_t op_result;  /**< Result for crypto */
} odp_packet_hdr_t;

typedef struct {
	const uint8_t *parseptr;
	uint32_t offset;

	uint16_t l3_len;
	uint16_t l4_len;

} odp_packet_parsing_ctx_t;

typedef struct odp_packet_hdr_stride {
	uint8_t pad[ODP_CACHE_LINE_SIZE_ROUNDUP(sizeof(odp_packet_hdr_t))];
} odp_packet_hdr_stride;

/**
 * Return the packet header
 */
static inline odp_packet_hdr_t *odp_packet_hdr(odp_packet_t pkt)
{
	return (odp_packet_hdr_t *)odp_buf_to_hdr((odp_buffer_t)pkt);
}

static inline void copy_packet_parser_metadata(odp_packet_hdr_t *src_hdr,
					       odp_packet_hdr_t *dst_hdr)
{
	dst_hdr->input_flags    = src_hdr->input_flags;
	dst_hdr->error_flags    = src_hdr->error_flags;
	dst_hdr->output_flags   = src_hdr->output_flags;

	dst_hdr->l2_offset      = src_hdr->l2_offset;
	dst_hdr->l3_offset      = src_hdr->l3_offset;
	dst_hdr->l4_offset      = src_hdr->l4_offset;
	dst_hdr->payload_offset = src_hdr->payload_offset;

	dst_hdr->l4_protocol    = src_hdr->l4_protocol;
}

static inline void *packet_map(odp_packet_hdr_t *pkt_hdr,
			       uint32_t offset, uint32_t *seglen)
{
	if (offset > pkt_hdr->frame_len)
		return NULL;

	return buffer_map(&pkt_hdr->buf_hdr,
			  pkt_hdr->headroom + offset, seglen,
			  pkt_hdr->headroom + pkt_hdr->frame_len);
}

static inline void push_head(odp_packet_hdr_t *pkt_hdr, size_t len)
{
	pkt_hdr->headroom  -= len;
	pkt_hdr->frame_len += len;
}

static inline void pull_head(odp_packet_hdr_t *pkt_hdr, size_t len)
{
	pkt_hdr->headroom  += len;
	pkt_hdr->frame_len -= len;
}

static inline void push_tail(odp_packet_hdr_t *pkt_hdr, size_t len)
{
	pkt_hdr->tailroom  -= len;
	pkt_hdr->frame_len += len;
}

static inline void pull_tail(odp_packet_hdr_t *pkt_hdr, size_t len)
{
	pkt_hdr->tailroom  += len;
	pkt_hdr->frame_len -= len;
}

static inline void packet_set_len(odp_packet_t pkt, uint32_t len)
{
	odp_packet_hdr(pkt)->frame_len = len;
}

static inline int packet_parse_l2_not_done(odp_packet_hdr_t *pkt_hdr)
{
	return !pkt_hdr->input_flags.parsed_l2;
}

static inline int packet_parse_not_complete(odp_packet_hdr_t *pkt_hdr)
{
	return !pkt_hdr->input_flags.parsed_all;
}

static inline void packet_free_multi(odp_packet_t pkt_tbl[], unsigned len)
{
	odp_buffer_free_multi((odp_buffer_t *)pkt_tbl, len);
}

/* Forward declarations */
int _odp_packet_copy_to_packet(odp_packet_t srcpkt, uint32_t srcoffset,
			       odp_packet_t dstpkt, uint32_t dstoffset,
			       uint32_t len);

void _odp_packet_copy_md_to_packet(odp_packet_t srcpkt, odp_packet_t dstpkt);

odp_packet_t _odp_packet_alloc(odp_pool_t pool_hdl);

void packet_init(pool_entry_t *pool, odp_packet_hdr_t *pkt_hdr,
		 size_t size, int parse);

/* Fill in parser metadata for L2 */
void packet_parse_l2(odp_packet_hdr_t *pkt_hdr);

/* Perform full packet parse */
int packet_parse_full(odp_packet_hdr_t *pkt_hdr);

/* Reset parser metadata for a new parse */
void packet_parse_reset(odp_packet_hdr_t *pkt_hdr);

int _odp_parse_common(odp_packet_hdr_t *pkt_hdr, const uint8_t *parseptr);

int _odp_cls_parse(odp_packet_hdr_t *pkt_hdr, const uint8_t *parseptr);


#ifdef __cplusplus
}
#endif

#endif
