/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 *
 * ODP packet descriptor
 */

#ifndef ODP_PLAT_PACKET_H_
#define ODP_PLAT_PACKET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <odp/std_types.h>
#include <odp/plat/event_types.h>
#include <odp/plat/packet_io_types.h>
#include <odp/plat/packet_types.h>
#include <odp/plat/buffer_types.h>
#include <odp/plat/pool_types.h>

/** @ingroup odp_packet
 *  @{
 */

static inline odp_packet_t odp_packet_from_event(odp_event_t ev)
{
	return (odp_packet_t)ev;
}

static inline odp_event_t odp_packet_to_event(odp_packet_t pkt)
{
	return (odp_event_t)pkt;
}

static inline int odp_packet_is_segmented(odp_packet_t pkt)
{
	(void)pkt;
	return 0;
}

static inline int odp_packet_num_segs(odp_packet_t pkt)
{
	(void)pkt;
	return 1;
}

static inline odp_packet_seg_t odp_packet_first_seg(odp_packet_t pkt)
{
	return (odp_packet_seg_t)pkt;
}

static inline odp_packet_seg_t odp_packet_last_seg(odp_packet_t pkt)
{
	return (odp_packet_seg_t)pkt;
}

static inline odp_packet_seg_t
odp_packet_next_seg(odp_packet_t pkt,
		    odp_packet_seg_t seg)
{
	(void)pkt;
	(void)seg;
	return ODP_PACKET_SEG_INVALID;
}

/**
 * @}
 */

#include <odp/api/packet.h>

#ifdef __cplusplus
}
#endif

#endif
