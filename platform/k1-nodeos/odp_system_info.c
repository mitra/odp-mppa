/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include <odp/system_info.h>
#include <odp_internal.h>
#include <odp_debug_internal.h>
#include <odp/align.h>
#include <odp/cpu.h>
#include <string.h>
#include <stdio.h>
#include <mppa_bsp.h>
/* sysconf */
#include <unistd.h>


/*
 * Report the number of online CPU's
 */
static int sysconf_cpu_count(void)
{
	long ret;

	ret = sysconf(_SC_NPROCESSORS_ONLN);
	if (ret < 0)
		return 0;

	return (int)ret;
}

static int huge_page_size(void)
{
	return ODP_PAGE_SIZE;
}


/*
 * Use sysconf and dummy values in generic case
 */


static int systemcpu(odp_system_info_t *sysinfo)
{
	int ret;

	ret = sysconf_cpu_count();
	if (ret == 0) {
		ODP_ERR("sysconf_cpu_count failed.\n");
		return -1;
	}

	sysinfo->cpu_count = ret;
	sysinfo->huge_page_size = huge_page_size();
	sysinfo->cpu_hz          = _K1_CPU_FREQ;
	if (__bsp_flavour == BSP_EXPLORER) {

		sysinfo->cpu_hz = 20000000ULL;
	}

	sysinfo->cache_line_size = _K1_DCACHE_LINE_SIZE;

#if defined(__K1A__)
#define K1_MODEL_STR	"K1A - Andey"
#elif defined(__K1B__)
#define K1_MODEL_STR	"K1B - Bostan"
#else
#define K1_MODEL_STR	"K1 - Unknown"
#endif
	snprintf(sysinfo->model_str, sizeof(sysinfo->model_str), "%s", K1_MODEL_STR);

	return 0;
}


/*
 * System info initialisation
 */
int odp_system_info_init(void)
{
	memset(&odp_global_data.system_info, 0, sizeof(odp_system_info_t));

	odp_global_data.system_info.page_size = ODP_PAGE_SIZE;


	if (systemcpu(&odp_global_data.system_info)) {
		ODP_ERR("systemcpu failed\n");
		return -1;
	}

	return 0;
}

/*
 *************************
 * Public access functions
 *************************
 */
uint64_t odp_sys_cpu_hz(void)
{
	return odp_global_data.system_info.cpu_hz;
}

uint64_t odp_sys_huge_page_size(void)
{
	return odp_global_data.system_info.huge_page_size;
}

uint64_t odp_sys_page_size(void)
{
	return odp_global_data.system_info.page_size;
}

const char *odp_sys_cpu_model_str(void)
{
	return odp_global_data.system_info.model_str;
}

int odp_sys_cache_line_size(void)
{
	return odp_global_data.system_info.cache_line_size;
}

int odp_cpu_count(void)
{
	return odp_global_data.system_info.cpu_count - 1;
}