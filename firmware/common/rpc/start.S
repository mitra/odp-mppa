.global _K1_PE_STACK_ADDRESS
.global _firmware_boot_rm4


.align 8
.global _firmware_rm_start
.proc _firmware_rm_start
_firmware_rm_start:
	make $r12 = _rm_stack_start - 16
	;;
	call __k1_rm_c_startup
	;;
	goto __k1_asm_proc_poweroff
	;;
.endp

.align 8
.global _firmware_rm4_start
.proc _firmware_rm4_start
_firmware_rm4_start:
	get $r5 = $pcr
	;;
	extfz $r0 = $r5, 15, 11
	;;
	srl $r0 = $r5, 9
	;;
	and $r0 = $r0, 0x7c
	;;
	make $r1 = _K1_PE_STACK_ADDRESS
	;;
	add $r1 = $r1, $r0
	;;
	lw $r12 = 0[$r1]  # init pe stack
	;;
	call __k1_rm_c_startup
	;;
	goto __k1_asm_proc_poweroff
	;;
.endp
