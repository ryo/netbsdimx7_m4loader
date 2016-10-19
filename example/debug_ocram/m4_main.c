
#define CONADDR 0x30a70000

#define IMX_UTXD	0x40	/* w  */	/* UART Transmitter Reg */
#define IMX_USR2	0x98	/* rw */	/* UART Status Reg 2 */

#define IMX_USR2_TXDC	(1<<3)


#define REG(base,reg)	((volatile unsigned int *)((char *)base + reg))

static void
xputc(char c)
{
	while ((*REG(CONADDR, IMX_USR2) & IMX_USR2_TXDC) == 0)
		;

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
	unsigned int i, j;
	volatile unsigned int *tcml = 0x1fff8000;	// debug

	for (i = 0;; i++) {
		*tcml = i;

		xputs("#");

		for (j = 0; j < 0x80000; j++) {
			asm("nop");
		}
	}
}
