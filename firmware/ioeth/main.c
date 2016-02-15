#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <HAL/hal/hal.h>

#include "odp_rpc_internal.h"
#include "rpc-server.h"
#include "boot.h"

int main (int argc, char *argv[])
{

	int ret;

	ret = odp_rpc_server_start();
	if (ret) {
		fprintf(stderr, "Failed to start server\n");
		exit(EXIT_FAILURE);
	}

	boot_clusters(argc, argv);
	join_clusters();

	return 0;
}
