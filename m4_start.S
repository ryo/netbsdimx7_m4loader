
.arch armv7e-m
.thumb

cortexM4_vector_table:
	.long	cortexM4_stack		// initail SP value
	.long	cortexM4_reset+1	// Reset
	.long	cortexM4_nmi+1		// NMI
	.long	cortexM4_hardfault+1	// Hard fault
	.long	cortexM4_mmufault+1	// Memory management fault
	.long	cortexM4_busfault+1	// Bus fault
	.long	cortexM4_usagefault+1	// Usage fault
	.long	0			// Reserved
	.long	0			// Reserved
	.long	0			// Reserved
	.long	0			// Reserved
	.long	cortexM4_svc+1		// SVCall
	.long	cortexM4_debug+1	// Reserved for Debug
	.long	0			// Reserved
	.long	cortexM4_pendsv+1	// PendSV
	.long	cortexM4_systick+1	// Systick

#define NIRQ	256
	.space	4 * NIRQ		// IRQ 0...N-1


	.space	1024
cortexM4_stack:


/* XXX */
cortexM4_nmi:
cortexM4_hardfault:
cortexM4_mmufault:
cortexM4_busfault:
cortexM4_usagefault:
cortexM4_svc:
cortexM4_debug:
cortexM4_pendsv:
cortexM4_systick:
	nop
1:	wfi
	b	1b

cortexM4_reset:
	cpsid	i
	b	cortexM4_main
