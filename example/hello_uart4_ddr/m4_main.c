
#define CONADDR 0x30a70000

#define IMX_UTXD	0x40	/* w  */	/* UART Transmitter Reg */
#define IMX_USR2	0x98	/* rw */	/* UART Status Reg 2 */

#define IMX_USR2_TXDC	(1<<3)


#define REG(base,reg)	((volatile unsigned int *)((char *)base + reg))

static void
xputc(char c)
{
	volatile unsigned int *tcml = 0x1fff8000;	// debug
	unsigned int x = 0x100;	// debug

	*tcml = __LINE__;	// debug
	while ((*REG(CONADDR, IMX_USR2) & IMX_USR2_TXDC) == 0) {
		*tcml = x++;	// debug
	}

	*REG(CONADDR, IMX_UTXD) = c;
}

static void
xputs(char *s)
{
	while (*s)
		xputc(*s++);
}

void
cortexM4_main(void)
{
	xputs("Hello CortexM4\r\n");
	for (;;)
		asm("wfi");
}
