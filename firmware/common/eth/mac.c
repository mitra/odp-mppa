#include <stdint.h>
#include <stdio.h>
#include <HAL/hal/hal.h>

int mppa_ethernet_generate_mac(unsigned int ioeth_id, unsigned int ifce_id, uint8_t *buffer)
{
	uint64_t timestamp;
	uint8_t ifce_value = 0;
	int i;

	if (ifce_id >= 4) {
		fprintf(stderr, "[ETH] Error: Wrong interface id %d\n", ifce_id);
		return -1;
	}

	/* create the interface number for the MAC address */
	if (ioeth_id == 160) {
		ifce_value = ifce_id;
	} else if (ioeth_id == 224) {
		ifce_value = ifce_id + 4;
	} else {
		fprintf(stderr, "[ETH] Error: Wrong ethernet cluster id %d\n", ioeth_id);
		return -1;
	}

	if (__k1_get_cluster_id() != 128 && __k1_get_cluster_id() != 160) {
		/* no fuses on this cluser */
		fprintf(stderr, "[ETH] Warning: Serial number accessible only on IO0!\n");
		timestamp = 0ULL;
	} else {
		uint64_t serial;

		mppa_fuse_init();
		serial = mppa_fuse_get_serial();
		if (serial == 0) {
			fprintf(stderr, "[ETH] Warning: Fuse returns bad serial\n");
		}

		unsigned year = (serial >> 36) & 0xff;
		unsigned month = (serial >> 32) & 0x0f;
		unsigned day = (serial >> 27) & 0x1f;
		unsigned hour = (serial >> 22) & 0x1f;
		unsigned minute = (serial >> 16) & 0x3f;
		/* unsigned second = (serial >> 10) & 0x3f; */

		timestamp =
			year * 12 * 31 * 24 * 60 +
			month * 31 * 24 * 60 +
			day * 24 * 60 +
			hour * 60 +
			minute;
	}
	uint64_t mac = (0x02ULL << 40) | ((timestamp << 4) & 0xfffffffff0) | (ifce_value << 1);
	/* Generate the MAC */
	for (i = 0; i < 6; i++) {
		uint8_t v = (mac >> (i * 8)) & 0xff;
		buffer[5-i] = v;
	}
	return 0;
}
