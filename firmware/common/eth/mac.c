#include <stdint.h>
#include <stdio.h>
#include <HAL/hal/hal.h>
#define DMSG printf

int mppa_ethernet_generate_mac(unsigned int ioeth_id, unsigned int ifce_id, uint8_t *buffer)
{
	uint64_t serial;
	uint8_t ifce_value = 0;
	int i;
	if (__k1_get_cluster_id() != 128) {
		/* no fuses on this cluser */
		DMSG ("Serial number accessible only on IODDR0!\n");
		return -1;
	}
	if (ifce_id >= 4) {
		DMSG ("Wrong interface id\n");
		return -1;
	}

	/* create the interface number for the MAC address */
	if (ioeth_id == 160) {
		ifce_value = ifce_id;
	} else if (ioeth_id == 224) {
		ifce_value = ifce_id + 4;
	} else {
		DMSG ("Wrong ethernet cluster id\n");
		return -1;
	}

	mppa_fuse_init();
	serial = mppa_fuse_get_serial();
	if (serial == 0) {
		DMSG ("Wrong serial\n");
		return -1;
	}
	unsigned year = (serial >> 36) & 0xff;
	unsigned month = (serial >> 32) & 0x0f;
	unsigned day = (serial >> 27) & 0x1f;
	unsigned hour = (serial >> 22) & 0x1f;
	unsigned minute = (serial >> 16) & 0x3f;
	/* unsigned second = (serial >> 10) & 0x3f; */
	uint64_t timestamp =
		year * 12 * 31 * 24 * 60 +
		month * 31 * 24 * 60 +
		day * 24 * 60 +
		hour * 60 +
		minute;

	serial = (0x02ULL << 40) | ((timestamp << 4) & 0xfffffffff0) | (ifce_value << 1);
	/* Generate the MAC */
	for (i = 0; i < 6; i++) {
		uint8_t v = (serial >> (i * 8)) & 0xff;
		buffer[5-i] = v;
	}
	return 0;
}
