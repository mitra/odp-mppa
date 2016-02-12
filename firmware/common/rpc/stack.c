#include <HAL/hal/hal.h>
 extern int RM_STACK_START;
extern int _firmware_rm4_start;
extern void __k1_asm_proc_poweroff();

void * __rm4_stack_start[8] = {
	&RM_STACK_START,
	&RM_STACK_START,
	&RM_STACK_START,
	&RM_STACK_START,
	&RM_STACK_START,
	&RM_STACK_START,
	&RM_STACK_START,
	&RM_STACK_START
};

void  __attribute__ ((constructor(101))) __boot_rm4()
{
	if (__k1_get_cpu_id() != 0)
		return;
	if (__k1_get_cluster_id() != 128 && __k1_get_cluster_id() != 192)
		return;

	_K1_PE_START_ADDRESS[4] = &_firmware_rm4_start;
	__k1_mb();
	__k1_poweron(4);
	__k1_asm_proc_poweroff();
}
