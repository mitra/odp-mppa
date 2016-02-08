#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <HAL/hal/hal.h>

#include <odp_rpc_internal.h>
#include <mppa_eth_core.h>
#include <mppa_eth_loadbalancer_core.h>
#include <mppa_eth_phy.h>
#include <mppa_eth_mac.h>
#include <mppa_routing.h>
#include <mppa_noc.h>
#include <mppa_eth_io_utils.h>
#include <mppa_eth_qsfp_utils.h>
#include "rpc-server.h"
#include "eth.h"

enum mppa_eth_mac_ethernet_mode_e mac_get_default_mode(unsigned lane_id)
{
	(void)lane_id;
	switch (__bsp_flavour) {
	case BSP_ETH_530:
	case BSP_EXPLORER:
		return MPPA_ETH_MAC_ETHMODE_1G;
		break;
	case BSP_KONIC80:
		return MPPA_ETH_MAC_ETHMODE_40G;
		break;
	case BSP_DEVELOPER:
		if (__k1_get_cluster_id() >= 192) {
			/* IO(DDR|ETH)1 */
			if(lane_id == 0 || lane_id == 1)
				return MPPA_ETH_MAC_ETHMODE_10G_BASE_R;
			if(lane_id == 2 || lane_id == 3)
				return MPPA_ETH_MAC_ETHMODE_1G;
		} else {
			/* IO(DDR|ETH)0 => EXB03 */
			return MPPA_ETH_MAC_ETHMODE_40G;
		}
		break;
	default:
		return -1;
	}
	return -1;
}

int ethtool_setup_eth2clus(unsigned remoteClus, int if_id,
			   int nocIf, int externalAddress,
			   int min_rx, int max_rx)
{
	int ret;

	mppa_dnoc_header_t header = { 0 };
	mppa_dnoc_channel_config_t config = { 0 };
	unsigned nocTx;
	int eth_if = if_id % 4;

	if (!status[eth_if].cluster[remoteClus].rx_enabled)
		return 0;

	ret = mppa_routing_get_dnoc_unicast_route(externalAddress,
						  remoteClus, &config, &header);
	if (ret != MPPA_ROUTING_RET_SUCCESS) {
		fprintf(stderr, "[ETH] Error: Failed to route to cluster %d\n", remoteClus);
		return -1;
	}

	ret = mppa_noc_dnoc_tx_alloc_auto(nocIf, &nocTx, MPPA_NOC_BLOCKING);
	if (ret != MPPA_NOC_RET_SUCCESS) {
		fprintf(stderr, "[ETH] Error: Failed to find an available Tx on DMA %d\n", nocIf);
		return -1;
	}

	config._.loopback_multicast = 0;
	config._.cfg_pe_en = 1;
	config._.cfg_user_en = 1;
	config._.write_pe_en = 1;
	config._.write_user_en = 1;
	config._.decounter_id = 0;
	config._.decounted = 0;
	config._.payload_min = 1;
	config._.payload_max = 32;
	config._.bw_current_credit = 0xff;
	config._.bw_max_credit     = 0xff;
	config._.bw_fast_delay     = 0x00;
	config._.bw_slow_delay     = 0x00;

	header._.tag = min_rx;
	header._.valid = 1;
	header._.multicast = 0;

	ret = mppa_noc_dnoc_tx_configure(nocIf, nocTx, header, config);
	if (ret != MPPA_NOC_RET_SUCCESS) {
		fprintf(stderr, "[ETH] Error: Failed to configure Tx\n");
		mppa_noc_dnoc_tx_free(nocIf, nocTx);
		return -1;
	}

	status[eth_if].cluster[remoteClus].nocIf = nocIf;
	status[eth_if].cluster[remoteClus].txId = nocTx;
	status[eth_if].cluster[remoteClus].min_rx = min_rx;
	status[eth_if].cluster[remoteClus].max_rx = max_rx;


	volatile mppa_dnoc_min_max_task_id_t *context =
		&mppa_dnoc[nocIf]->tx_chan_route[nocTx].
		min_max_task_id[ETH_DEFAULT_CTX];

	context->_.current_task_id = min_rx;
	context->_.min_task_id = min_rx;
	context->_.max_task_id = max_rx;
	context->_.min_max_task_id_en = 1;

	if (status[eth_if].cluster[remoteClus].jumbo) {
		mppabeth_lb_cfg_jumbo_mode((void *)&(mppa_ethernet[0]->lb),
					   eth_if, MPPABETHLB_JUMBO_ALLOWED);
	} else {
		mppabeth_lb_cfg_jumbo_mode((void *)&(mppa_ethernet[0]->lb),
					   eth_if, MPPABETHLB_JUMBO_DISABLED);
	}

	return 0;
}


int ethtool_setup_clus2eth(unsigned remoteClus, int if_id, int nocIf)
{
	int ret;
	unsigned rx_port;
	int eth_if = if_id % 4;

	if (!status[eth_if].cluster[remoteClus].tx_enabled)
		return 0;

	mppa_dnoc[nocIf]->rx_global.rx_ctrl._.alert_level = -1;
	mppa_dnoc[nocIf]->rx_global.rx_ctrl._.payload_slice = 2;
	ret = mppa_noc_dnoc_rx_alloc_auto(nocIf, &rx_port, MPPA_NOC_NON_BLOCKING);
	if(ret) {
		fprintf(stderr, "[ETH] Error: Failed to find an available Rx on DMA %d\n", nocIf);
		return -1;
	}

	mppa_dnoc_queue_event_it_target_t it_targets = {
		.reg = 0
	};
	int fifo_id = remoteClus;

	/* If we are using 40G */
	if (if_id == 4) {
		/* Jumbo frames */
		fifo_id = (remoteClus % 4) * 4;
		mppa_ethernet[0]->tx.fifo_if[nocIf - ETH_BASE_TX].lane[eth_if].
			eth_fifo[fifo_id].eth_fifo_ctrl._.jumbo_mode = 1;
	}

	mppa_ethernet[0]->tx.fifo_if[nocIf - ETH_BASE_TX].lane[eth_if].
		eth_fifo[fifo_id].eth_fifo_ctrl._.drop_en = 1;
	mppa_noc_dnoc_rx_configuration_t conf = {
		.buffer_base = (unsigned long)(void*)
		&mppa_ethernet[0]->tx.fifo_if[nocIf - ETH_BASE_TX].lane[eth_if].
		eth_fifo[fifo_id].push_data,
		.buffer_size = 8,
		.current_offset = 0,
		.event_counter = 0,
		.item_counter = 1,
		.item_reload = 1,
		.reload_mode = MPPA_NOC_RX_RELOAD_MODE_DECR_NOTIF_RELOAD,
		.activation = MPPA_NOC_ACTIVATED | MPPA_NOC_FIFO_MODE,
		.counter_id = 0,
		.event_it_targets = &it_targets,
	};

	ret = mppa_noc_dnoc_rx_configure(nocIf, rx_port, conf);
	if(ret) {
		fprintf(stderr, "[ETH] Error: Failed to configure Rx\n");
		mppa_noc_dnoc_rx_free(nocIf, rx_port);
		return -1;
	}

	status[eth_if].cluster[remoteClus].nocIf = nocIf;
	status[eth_if].cluster[remoteClus].rx_tag = rx_port;
	return 0;
}

int ethtool_init_lane(unsigned if_id, int loopback)
{
	int ret;
	int eth_if = if_id % 4;

	switch (status[eth_if].initialized) {
	case ETH_LANE_OFF:
		if (loopback) {
#ifdef VERBOSE
			printf("[ETH] Initializing lane %d in loopback\n", eth_if);
#endif
			mppabeth_mac_enable_loopback_bypass((void *)&(mppa_ethernet[0]->mac));
			for (int i = 0; i < N_ETH_LANE; ++i)
				status[i].initialized = (if_id == 4) ? ETH_LANE_LOOPBACK_40G :  ETH_LANE_LOOPBACK;

		} else {
			enum mppa_eth_mac_ethernet_mode_e link_speed =
				mac_get_default_mode(eth_if);
			if (!link_speed == MPPA_ETH_MAC_ETHMODE_40G && if_id == 4) {
				fprintf(stderr,
					"[ETH] Error: Cannot open 40G link\n");
				return -1;
			} else if (link_speed == MPPA_ETH_MAC_ETHMODE_40G && if_id < 4) {
				/* Link could do 40G but we use only one lane */
				link_speed = MPPA_ETH_MAC_ETHMODE_10G_BASE_R;
			}
#ifdef VERBOSE
			printf("[ETH] Initializing global MAC @ %d\n", link_speed);
#endif
			mppabeth_mac_cfg_mode((void*) &(mppa_ethernet[0]->mac), link_speed);

			/* Init MAC */
#ifdef VERBOSE
			printf("[ETH] Initializing MAC for lane %d\n", eth_if);
#endif
			ret = mppa_eth_utils_init_mac(eth_if, link_speed);
			if (ret == BAD_VENDOR) {
				fprintf(stderr,
					"[ETH] Warning: QSFP coonector is not supported\n");
			}else if(ret < 0) {
				fprintf(stderr,
					"[ETH] Error: Failed to initialize lane %d (%d)\n",
					eth_if, ret);
				return -1;
			}

			mppabeth_mac_enable_rx_check_sfd((void*)
							 &(mppa_ethernet[0]->mac));
			mppabeth_mac_enable_rx_fcs_deletion((void*)
							    &(mppa_ethernet[0]->mac));
			mppabeth_mac_enable_tx_fcs_insertion((void*)
				&(mppa_ethernet[0]->mac));
			mppabeth_mac_enable_tx_add_padding((void*)
				&(mppa_ethernet[0]->mac));
			mppabeth_mac_enable_rx_check_preambule((void*)
				&(mppa_ethernet[0]->mac));
#ifdef VERBOSE
			printf("[ETH] Starting MAC for lane %d\n", eth_if);
#endif
			mppa_eth_utils_start_lane(eth_if, link_speed);

#ifdef VERBOSE
			printf("[ETH] Waiting for link %d\n", eth_if);
#endif
			/* Wait for link to come up */
			unsigned long long start = __k1_read_dsu_timestamp();
			int up = 0;
			while (__k1_read_dsu_timestamp() - start < 3ULL * __bsp_frequency) {
				if (!mppa_eth_utils_mac_poll_state(eth_if, link_speed)) {
					up = 1;
					break;
				}
			}
#ifdef VERBOSE
			printf("Link %d %s\n", eth_if, up ? "up" : "down/polling");
#endif
			if (!up)
				return -1;
			if (if_id == 4) {
				for (int i = 9; i < N_ETH_LANE; ++i)
					status[i].initialized = ETH_LANE_ON_40G;
			} else
				status[eth_if].initialized = ETH_LANE_ON;
		}
		break;
	case ETH_LANE_ON:
		if (loopback) {
			fprintf(stderr,
				"[ETH] Error: One lane was enabled. Cannot set lane %d in loopback\n",
				eth_if);
			return -1;
		} else if (if_id == 4) {
			fprintf(stderr,
				"[ETH] Error: One lane was enabled in 1 or 10G. Cannot set lane %d in 40G\n",
				eth_if);
			return -1;
		}
		break;
	case ETH_LANE_ON_40G:
		if (loopback) {
			fprintf(stderr,
				"[ETH] Error: One lane was enabled. Cannot set lane %d in loopback\n",
				eth_if);
			return -1;
		} else if (if_id < 4) {
			fprintf(stderr,
				"[ETH] Error: Interface was enabled in 40G. Cannot set lane %d in 1 or 10G mode\n",
				eth_if);
			return -1;
		}
		break;
	case ETH_LANE_LOOPBACK:
		if (!loopback) {
			fprintf(stderr,
				"[ETH] Error: Eth is in 10G loopback. Cannot enable lane %d\n",
				eth_if);
			return -1;
		} else if(if_id == 4){
			fprintf(stderr,
				"[ETH] Error: One lane was enabled in 10G loopback . Cannot set lane %d in 40G loopback\n",
				eth_if);
			return -1;
		}
		break;
	case ETH_LANE_LOOPBACK_40G:
		if (!loopback) {
			fprintf(stderr,
					"[ETH] Error: Eth is in 40G loopback. Cannot enable lane %d\n",
					eth_if);
			return -1;
		} else if (if_id < 4) {
			fprintf(stderr,
				"[ETH] Error: Interface was enabled in 40G loopback. Cannot set lane %d in 10G loopback\n",
				eth_if);
			return -1;
		}
		break;
	default:
		return -1;
	}

	return 0;
}

static inline pkt_rule_entry_t
mppabeth_lb_get_rule(void __iomem *lb_addr,
	unsigned int rule_id,
	unsigned int entry_id) {
	pkt_rule_entry_t entry = {0};
	entry.offset = mppabeth_lb_get_rule_offset(lb_addr, rule_id, entry_id);
	entry.cmp_mask = mppabeth_lb_get_rule_cmp_mask(lb_addr, rule_id, entry_id);
	entry.cmp_value = mppabeth_lb_get_rule_expected_value(lb_addr, rule_id, entry_id);
	entry.hash_mask = mppabeth_lb_get_rule_hashmask(lb_addr, rule_id, entry_id);
	return entry;
}

static int compare_rule_entries(const pkt_rule_entry_t entry1,
				const pkt_rule_entry_t entry2)
{
	return entry1.offset != entry2.offset ||
		entry1.cmp_mask != entry2.cmp_mask ||
		entry1.cmp_value != entry2.cmp_value ||
		entry1.hash_mask != entry2.hash_mask;
}

static int check_rules_identical(const pkt_rule_t *rules, int nb_rules)
{
	for ( int rule_id = 0; rule_id < nb_rules; ++rule_id ) {
		for ( int entry_id = 0; entry_id < rules[rule_id].nb_entries; ++entry_id ) {
			pkt_rule_entry_t entry = mppabeth_lb_get_rule((void *) &(mppa_ethernet[0]->lb), rule_id, entry_id);
			if ( compare_rule_entries(rules[rule_id].entries[entry_id], entry ) ) {
				fprintf(stderr, "Rule[%d] entry[%d] differs from already set rule\n", rule_id, entry_id);
				return 1;
			}
		}
	}
	return 0;
}

// dispatch hash lut between registered clusters
static void update_lut(unsigned if_id)
{

	if (lb_status.enabled)
		return;

	const int eth_if = if_id % 4;
	uint32_t clusters = 0;
	for (int i = 0; i < BSP_NB_CLUSTER_MAX; ++i) {
		if (status[eth_if].cluster[i].opened != ETH_CLUS_STATUS_OFF &&
		    status[eth_if].cluster[i].hashpolicy &&
		    status[eth_if].cluster[i].rx_enabled)
			clusters |=  1 << i;
	}

	const int nb_registered = __k1_cbs(clusters);
	int chunks[nb_registered];

	for ( int j = 0; j < nb_registered; ++j ) {
		chunks[j] = MPPABETHLB_LUT_ARRAY_SIZE / nb_registered +
			( ( j < ( MPPABETHLB_LUT_ARRAY_SIZE % nb_registered ) ) ? 1 : 0 );
	}

	for ( int i = 0, j = 0; i < MPPABETHLB_LUT_ARRAY_SIZE ; i+= chunks[j], j++ ) {
		int registered_cluster = __k1_ctz(clusters);
		clusters &= ~(1 << registered_cluster);
		int tx_id = status[eth_if].cluster[registered_cluster].txId;
		int noc_if = status[eth_if].cluster[registered_cluster].nocIf;
#ifdef VERBOSE
		printf("config lut[%3d-%3d] -> C%2d: %d %d %d %d\n",
			   i, i + chunks[j] - 1, registered_cluster,
			   eth_if, tx_id, ETH_DEFAULT_CTX, noc_if - 4);
#endif
		for ( int lut_id = i; lut_id < i + chunks[j] ; ++lut_id ) {
			mppabeth_lb_cfg_luts((void *) &(mppa_ethernet[0]->lb),
								 eth_if, lut_id, tx_id,
								 ETH_DEFAULT_CTX,
								 noc_if - 4);
		}
	}
}

int ethtool_apply_rules(unsigned remoteClus, unsigned if_id,
			int nb_rules, const pkt_rule_t rules[nb_rules] )
{
	const int eth_if = if_id % 4;

	if (!nb_rules)
		return 0;

#ifdef VERBOSE
	printf("Applying %d rules for cluster %d\n", nb_rules, remoteClus);
#endif

	if (!lb_status.enabled) {
		/* No Hash rules yet. Make sure no one opened anything yet */
		for (int i =0; i < N_ETH_LANE; ++i){
			if (status[i].initialized != ETH_LANE_OFF) {
				fprintf(stderr, "[ETH] Error: Lane %d already opened without hashpolicy\n", i);
				return -1;
			}
		}
		/* Configure the LB */
		for ( int rule_id = 0; rule_id < nb_rules; ++rule_id) {
			for ( int entry_id = 0; entry_id < rules[rule_id].nb_entries; ++entry_id) {
#ifdef VERBOSE
				printf("Rule[%d] (P%d) Entry[%d]: offset %d cmp_mask 0x%02x cmp_value "
				       "0x%016llx hash_mask 0x%02x>\n",
				       rule_id,
				       rules[rule_id].priority,
				       entry_id,
				       rules[rule_id].entries[entry_id].offset,
				       rules[rule_id].entries[entry_id].cmp_mask,
				       rules[rule_id].entries[entry_id].cmp_value,
				       rules[rule_id].entries[entry_id].hash_mask);
#endif
				mppabeth_lb_cfg_rule((void *) &(mppa_ethernet[0]->lb),
						     rule_id, entry_id,
						     rules[rule_id].entries[entry_id].offset,
						     rules[rule_id].entries[entry_id].cmp_mask,
						     rules[rule_id].entries[entry_id].cmp_value,
						     rules[rule_id].entries[entry_id].hash_mask);
				mppabeth_lb_cfg_min_max_swap((void *) &(mppa_ethernet[0]->lb),
							     rule_id, (entry_id >> 1), 0);
			}
			mppabeth_lb_cfg_extract_table_mode((void *) &(mppa_ethernet[0]->lb),
							   rule_id,
							   rules[rule_id].priority,
							   MPPA_ETHERNET_DISPATCH_POLICY_HASH);
		}
		lb_status.enabled = 1;
	} else if ( check_rules_identical(rules, nb_rules) ) {
		fprintf(stderr, "[ETH] Error: Non matching hash policy already registered\n");
		return -1;
	}
	status[eth_if].cluster[remoteClus].hashpolicy = 1;
	return 0;
}

void ethtool_cleanup_cluster(unsigned remoteClus, unsigned if_id)
{
	int eth_if = if_id % 4;
	int noc_if = status[eth_if].cluster[remoteClus].nocIf;
	int tx_id = status[eth_if].cluster[remoteClus].txId;
	int rx_tag = status[eth_if].cluster[remoteClus].rx_tag;

	if (rx_tag >= 0)
		mppa_noc_dnoc_rx_free(noc_if, rx_tag);

	if (tx_id >= 0) {
		mppa_dnoc[noc_if]->tx_chan_route[tx_id].
			min_max_task_id[ETH_DEFAULT_CTX]._.min_max_task_id_en = 0;

		mppa_noc_dnoc_tx_free(noc_if, tx_id);

	}

	if (status[eth_if].cluster[remoteClus].hashpolicy) {
		/* If we were the last hash policy. Clear up the tables
		 * and reset the LB hash data */
		int hash_global_count = 0;
		for (int i = 0; i < N_ETH_LANE; ++i) {
			hash_global_count += status[i].hash_refcount;
		}
		if (!hash_global_count){
			for (int i = 0; i < MPPABETHLB_XT_TABLE_ARRAY_SIZE; ++i){
				mppabeth_lb_cfg_extract_table_mode((void *) &(mppa_ethernet[0]->lb),
								   i, 0,
								   MPPA_ETHERNET_DISPATCH_POLICY_OFF);
			}
			_eth_lb_status_init(&lb_status);
		}
	}
	_eth_cluster_status_init(&(status[eth_if].cluster[remoteClus]));

}

int ethtool_enable_cluster(unsigned remoteClus, unsigned if_id)
{
	const int eth_if = if_id % 4;
	const int noc_if = status[eth_if].cluster[remoteClus].nocIf;
	const int tx_id = status[eth_if].cluster[remoteClus].txId;
	const int hashpolicy = status[eth_if].cluster[remoteClus].hashpolicy;

	if(noc_if < 0 ||
	   status[eth_if].cluster[remoteClus].enabled) {
		return -1;
	}


	status[eth_if].cluster[remoteClus].enabled = 1;

	if (status[eth_if].cluster[remoteClus].rx_enabled) {
		/* Configure dispatcher so that the defaulat "MATCH ALL" also
		 * sends packet to our cluster */
		if (hashpolicy) {
			update_lut(if_id);
		} else {
			mppabeth_lb_cfg_default_rr_dispatch_channel((void *)&(mppa_ethernet[0]->lb),
								    eth_if, noc_if - 4, tx_id,
								    (1 << ETH_DEFAULT_CTX));
		}
	}

	if (hashpolicy) {
		if (!status[eth_if].hash_refcount) {
			// TODO
		}
		status[eth_if].hash_refcount++;
	} else {
		if (!status[eth_if].enabled_refcount) {
			mppabeth_lb_cfg_default_dispatch_policy((void *)&(mppa_ethernet[0]->lb),
								eth_if,
								MPPABETHLB_DISPATCH_DEFAULT_POLICY_RR);
		}
		status[eth_if].enabled_refcount++;
	}

	return 0;
}
int ethtool_disable_cluster(unsigned remoteClus, unsigned if_id)
{

	const int eth_if = if_id % 4;
	const int noc_if = status[eth_if].cluster[remoteClus].nocIf;
	const int tx_id = status[eth_if].cluster[remoteClus].txId;
	const int hashpolicy = status[eth_if].cluster[remoteClus].hashpolicy;

	if(status[eth_if].cluster[remoteClus].nocIf < 0 ||
	   status[eth_if].cluster[remoteClus].enabled == 0) {
		return -1;
	}
	status[eth_if].cluster[remoteClus].enabled = 0;

	if (hashpolicy) {
		status[eth_if].hash_refcount--;
		if (!status[eth_if].hash_refcount) {
			// TODO
		}

		if (status[eth_if].cluster[remoteClus].rx_enabled) {
			update_lut(if_id);
		}

	} else {
		status[eth_if].enabled_refcount--;
		if (!status[eth_if].enabled_refcount) {
			mppabeth_lb_cfg_default_dispatch_policy((void *)&(mppa_ethernet[0]->lb),
								eth_if,
								MPPABETHLB_DISPATCH_DEFAULT_POLICY_DROP);
		}

		if (status[eth_if].cluster[remoteClus].rx_enabled) {
			mppabeth_lb_cfg_default_rr_dispatch_channel((void *)&(mppa_ethernet[0]->lb),
								    eth_if, noc_if - ETH_BASE_TX,tx_id, 0);
		}
	}


	return 0;
}


