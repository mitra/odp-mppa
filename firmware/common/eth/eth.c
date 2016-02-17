#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <stdio.h>
#include <HAL/hal/hal.h>

#include <odp_rpc_internal.h>

#include "rpc-server.h"
#include "eth.h"
#include "mac.h"

eth_status_t status[N_ETH_LANE];
eth_lb_status_t lb_status;

static inline int get_eth_dma_id(unsigned cluster_id){
	unsigned offset = (cluster_id / 4) % 4;
#ifdef K1B_EXPLORER
	offset = 0;
#endif

	switch(__k1_get_cluster_id()){
#ifndef K1B_EXPLORER
	case 128:
	case 160:
		return offset + 4;
#endif
	case 192:
	case 224:
		return offset + 4;
	default:
		return -1;
	}
}

odp_rpc_cmd_ack_t  eth_open(unsigned remoteClus, odp_rpc_t *msg,
			    uint8_t *payload, unsigned fallthrough)
{
	odp_rpc_cmd_ack_t ack = { .status = 0};
	odp_rpc_cmd_eth_open_t data = { .inl_data = msg->inl_data };
	const int nocIf = get_eth_dma_id(remoteClus);
	const unsigned int eth_if = data.ifId % 4; /* 4 is actually 0 in 40G mode */

	if(nocIf < 0) {
		fprintf(stderr, "[ETH] Error: Invalid NoC interface (%d %d)\n", nocIf, remoteClus);
		goto err;
	}
	if(data.ifId > 4 || eth_if >= N_ETH_LANE) {
		fprintf(stderr, "[ETH] Error: Invalid Eth lane\n");
		goto err;
	}

	if (data.ifId == 4) {
		/* 40G port. We need to check all lanes */
		for (int i = 0; i < N_ETH_LANE; ++i) {
			if(status[i].cluster[remoteClus].opened != ETH_CLUS_STATUS_OFF) {
				fprintf(stderr, "[ETH] Error: Lane %d is already opened for cluster %d\n",
						i, remoteClus);
				goto err;
			}
		}
	} else {
		if (status[eth_if].cluster[remoteClus].opened != ETH_CLUS_STATUS_OFF) {
			fprintf(stderr, "[ETH] Error: Lane %d is already opened for cluster %d\n",
					eth_if, remoteClus);
			goto err;
		}
		if (data.jumbo) {
			fprintf(stderr,
				"[ETH] Error: Trying to enable Jumbo on 1/10G lane %d\n",
				eth_if);
			goto err;

		}
	}

	if (fallthrough && !lb_status.dual_mac) {
		fprintf(stderr, "[ETH] Error: Trying to open in fallthrough with Dual-MAC mode disabled\n");
		goto err;
	}

	int externalAddress = odp_rpc_get_cluster_id(nocIf);

	status[eth_if].cluster[remoteClus].rx_enabled = data.rx_enabled;
	status[eth_if].cluster[remoteClus].tx_enabled = data.tx_enabled;
	status[eth_if].cluster[remoteClus].jumbo = data.jumbo;

	if (ethtool_open_cluster(remoteClus, data.ifId))
		goto err;
	if (ethtool_setup_eth2clus(remoteClus, data.ifId, nocIf, externalAddress,
				   data.min_rx, data.max_rx))
		goto err;
	if (ethtool_setup_clus2eth(remoteClus, data.ifId, nocIf))
		goto err;
	if (fallthrough) {
		status[eth_if].cluster[remoteClus].policy = ETH_CLUS_POLICY_FALLTHROUGH;
	} else {
		if ( ethtool_apply_rules(remoteClus, data.ifId,
					 data.nb_rules, (pkt_rule_t*)payload))
			goto err;
	}
	if (ethtool_enable_cluster(remoteClus, data.ifId))
		goto err;
	if (ethtool_start_lane(data.ifId, data.loopback))
		goto err_enabled;

	ack.cmd.eth_open.tx_if = externalAddress;
	ack.cmd.eth_open.tx_tag = status[eth_if].cluster[remoteClus].rx_tag;
	if (data.jumbo) {
		ack.cmd.eth_open.mtu = 9000;
	} else {
		ack.cmd.eth_open.mtu = 1600;
	}

	if (!lb_status.dual_mac || fallthrough) {
		memcpy(ack.cmd.eth_open.mac, status[eth_if].mac_address[0], ETH_ALEN);
	} else {
		memcpy(ack.cmd.eth_open.mac, status[eth_if].mac_address[1], ETH_ALEN);
	}

	return ack;
 err_enabled:
	ethtool_disable_cluster(remoteClus, data.ifId);
 err:
	ethtool_close_cluster(remoteClus, data.ifId);
	ack.status = 1;
	return ack;
}

odp_rpc_cmd_ack_t  eth_close(unsigned remoteClus, odp_rpc_t *msg)
{
	odp_rpc_cmd_ack_t ack = { .status = 0 };
	odp_rpc_cmd_eth_clos_t data = { .inl_data = msg->inl_data };
	const unsigned int eth_if = data.ifId % 4; /* 4 is actually 0 in 40G mode */

	if (data.ifId == 4) {
		if(status[eth_if].cluster[remoteClus].opened != ETH_CLUS_STATUS_40G) {
			ack.status = -1;
			return ack;
		}
	} else {
		if(status[eth_if].cluster[remoteClus].opened != ETH_CLUS_STATUS_ON) {
			ack.status = -1;
			return ack;
		}
	}

	ethtool_disable_cluster(remoteClus, data.ifId);
	ethtool_close_cluster(remoteClus, data.ifId);

	if (data.ifId == 4) {
		for (int i = 0; i < N_ETH_LANE; ++i) {
			_eth_cluster_status_init(&status[i].cluster[remoteClus]);
		}
	} else {
		_eth_cluster_status_init(&status[eth_if].cluster[remoteClus]);
	}

	return ack;
}

odp_rpc_cmd_ack_t  eth_dual_mac(unsigned remoteClus __attribute__((unused)),
				odp_rpc_t *msg)
{
	odp_rpc_cmd_ack_t ack = { .status = 0 };
	odp_rpc_cmd_eth_dual_mac_t data = { .inl_data = msg->inl_data };
	if (ethtool_set_dual_mac(data.enabled))
		ack.status = -1;
	return ack;
}

static void eth_init(void)
{
	for (int eth_if = 0; eth_if < N_ETH_LANE; ++eth_if) {
		_eth_status_init(&status[eth_if]);
		ethtool_init_lane(eth_if);
		mppa_ethernet_generate_mac(160 + 64 *( (__k1_get_cluster_id() - 128) / 64), eth_if,
					   status[eth_if].mac_address[0]);
		memcpy(status[eth_if].mac_address[1], status[eth_if].mac_address[0], ETH_ALEN);
		status[eth_if].mac_address[1][ETH_ALEN - 1] |= 1;
	}
}

static int eth_rpc_handler(unsigned remoteClus, odp_rpc_t *msg, uint8_t *payload)
{
	odp_rpc_cmd_ack_t ack = ODP_RPC_CMD_ACK_INITIALIZER;

	switch (msg->pkt_type){
	case ODP_RPC_CMD_ETH_OPEN:
		ack = eth_open(remoteClus, msg, payload, 0);
		break;
	case ODP_RPC_CMD_ETH_CLOS:
	case ODP_RPC_CMD_ETH_CLOS_DEF:
		ack = eth_close(remoteClus, msg);
		break;
	case ODP_RPC_CMD_ETH_OPEN_DEF:
		ack = eth_open(remoteClus, msg, payload, 1);
		break;
	case ODP_RPC_CMD_ETH_DUAL_MAC:
		ack = eth_dual_mac(remoteClus, msg);
		break;
	default:
		return -1;
	}
	odp_rpc_server_ack(msg, ack);
	return 0;
}

void  __attribute__ ((constructor)) __eth_rpc_constructor()
{
#if defined(MAGIC_SCALL)
	return;
#endif

	eth_init();
	if(__n_rpc_handlers < MAX_RPC_HANDLERS) {
		__rpc_handlers[__n_rpc_handlers++] = eth_rpc_handler;
	} else {
		fprintf(stderr, "Failed to register ETH RPC handlers\n");
		exit(EXIT_FAILURE);
	}
}
