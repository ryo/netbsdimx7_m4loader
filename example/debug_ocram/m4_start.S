/*
 * Copyright (c) 2016 Ryo Shimizu <ryo@nerv.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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


// write debug code to TCM_L[0x1fff8000]
#define	DYINGMSG(x)		\
	movw	r0, #0x8000;	\
	movt	r0, #0x1fff;	\
	movw	r1, #(x);	\
	movt	r1, #0xEEEE;	\
	str	r1, [r0, #0];	\
	b	halt

cortexM4_nmi:
	DYINGMSG(0x0002)
cortexM4_hardfault:
	DYINGMSG(0x0003)
cortexM4_mmufault:
	DYINGMSG(0x0004)
cortexM4_busfault:
	DYINGMSG(0x0005)
cortexM4_usagefault:
	DYINGMSG(0x0006)
cortexM4_svc:
	DYINGMSG(0x0007)
cortexM4_debug:
	DYINGMSG(0x0008)
cortexM4_pendsv:
	DYINGMSG(0x0009)
cortexM4_systick:
	DYINGMSG(0x000a)
halt:
1:	wfi
	b	1b

cortexM4_reset:
	cpsid	i
	b	cortexM4_main
