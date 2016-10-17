#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#include <arm/imx/imx7_srcreg.h>

#include "mem.h"

#define OCRAM_S_ADDR	0x00180000
#define TCM_ADDR	0x007f8000
#define DDR_ADDR	0x80000000
#define SRC_BASE	0x30390000

int verbose = 0;

static void
usage(void)
{
	fprintf(stderr, "usage: m4loader -v [command [arg ...] [command [arg ...] ...]]\n");
	fprintf(stderr, "	-v	verbose\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "	command:\n");
	fprintf(stderr, "	    load <address> <file>\n");
	fprintf(stderr, "	        load <file> to <address>\n");
	fprintf(stderr, "	    loadv <address> <file>\n");
	fprintf(stderr, "	        load <file> to <address> with copying vectors to OCRAM\n");
	fprintf(stderr, "	    stop\n");
	fprintf(stderr, "	        stop Cortex-M4 core\n");
	fprintf(stderr, "	    reset\n");
	fprintf(stderr, "	        reset Cortex-M4 core\n");
	fprintf(stderr, "	    dump <address>[-<address>]\n");
	fprintf(stderr, "	    dump <address>[:<size>]\n");
	fprintf(stderr, "	        dump specified address of physical memory\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "	e.g.\n");
	fprintf(stderr, "	    m4loader -v load ocram m4.bin reset\n");
	fprintf(stderr, "	        load \"m4.bin\" to ocram_s:0x00180000, and reset Cortex-M4 core.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "	    m4loader -v load ocram m4.bin load 0x80001000 m4_main.bin reset\n");
	fprintf(stderr, "	        load \"m4.bin\" to ocram_s:0x00180000,\n");
	fprintf(stderr, "	        load \"m4_main.bin\" to DDR memory:0x80001000,\n");
	fprintf(stderr, "	        and reset Cortex-M4 core.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "	loadable addresses:\n");
	fprintf(stderr, "	    ocram: 0x00180000-0x00187fff (32KB)\n");
	fprintf(stderr, "	    tcm:   0x007f8000-0x00807fff (64KB)\n");
	fprintf(stderr, "	    ddr:   0x80000000-0x801fffff (32MB)\n");
	exit(99);
}

static void
m4_stop(void)
{
	uint32_t v;

	memopen();

#define SRC_BASE	0x30390000
	/* assert M4 core reset */
	v = memread4(SRC_BASE + SRC_M4RCR);
	v |= SRC_M4RCR_SW_M4C_NON_SCLR_RST;
	memwrite4(SRC_BASE + SRC_M4RCR, v);
}

static void
m4_reset(void)
{
	uint32_t v;

	memopen();

	/* reset M4 core */
	v = memread4(SRC_BASE + SRC_M4RCR);
	v |= SRC_M4RCR_ENABLE_M4;
	memwrite4(SRC_BASE + SRC_M4RCR, v);

	v = memread4(SRC_BASE + SRC_M4RCR);
	v &= ~SRC_M4RCR_SW_M4C_NON_SCLR_RST;
	memwrite4(SRC_BASE + SRC_M4RCR, v);

	v = memread4(SRC_BASE + SRC_M4RCR);
	v |= SRC_M4RCR_SW_M4C_RST;
	memwrite4(SRC_BASE + SRC_M4RCR, v);

	if (verbose)
		fprintf(stderr, "Resetting Cortex-M4...");

	/* wait reset done */
	for (;;) {
		v = memread4(SRC_BASE + SRC_M4RCR);
		if ((v & SRC_M4RCR_SW_M4C_RST) == 0)
			break;
		usleep(100000);
	}

	if (verbose)
		fprintf(stderr, "done\n");



}

static int
m4_load(uint32_t addr, const char *filename, int setupSPPC)
{
	uint32_t stack_pc[2];
	uint32_t addr0;
	char *buf;
	int fd;
	ssize_t len, totallen;

	memopen();

#define READBUFSIZE	(32 * 1024)
	buf = malloc(READBUFSIZE);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		err(1, "open: %s", filename);

	for (addr0 = addr, totallen = 0; ; addr += len, totallen += len) {
		len = read(fd, buf, READBUFSIZE);
		if (len < 0)
			err(2, "read: %s", filename);
		if (len == 0)
			break;

		if (setupSPPC && (addr0 == addr))
			memcpy(stack_pc, buf, sizeof(stack_pc));

		memwrite(addr, buf, len);
	}

	if (setupSPPC)
		memwrite(OCRAM_S_ADDR, stack_pc, sizeof(stack_pc));

	if (verbose) {
		printf("load \"%s\" to 0x%08x-0x%08x (%u bytes)\n",
		    filename, addr0, addr0 + totallen, totallen);
	}

	return totallen;
}


static void
memdump(uint32_t address, uint32_t size)
{
	unsigned char buf[16];
	int i;

	memopen();

	for (i = 0; i < size; i++) {
		if ((i & 15) == 0) {
			printf("%08x:", address);

			memread(address, buf, sizeof(buf));
			address += sizeof(buf);
		}

		printf(" %02x", buf[i]);

		if ((i & 15) == 15)
			printf("\n");
	}

	if ((i & 15) != 0)
		printf("\n");

	printf("\n");
}


static void
debugdump()
{
	uint32_t v;

	memopen();

	for (;;) {
		v = memread4(TCM_ADDR);
		printf("TCM_L  :0x%p = %08x\n", TCM_ADDR, v);
		v = memread4(TCM_ADDR + 4);
		printf("TCM_L  :0x%p = %08x\n", TCM_ADDR + 4, v);
		v = memread4(OCRAM_S_ADDR);
		printf("OCRAM_S:0x%p = %08x\n", OCRAM_S_ADDR, v);
		v = memread4(OCRAM_S_ADDR + 4);
		printf("OCRAM_S:0x%p = %08x\n", OCRAM_S_ADDR + 4, v);
		printf("\n");
		fflush(stdout);
		sleep(1);
	}
}

static int
parsehex(const char *str, uint32_t *value)
{
	char *ep;

	*value = strtoul(str, &ep, 16);
	if (*ep == '\0')
		return 0;
	return -1;
}

static int
parsenum(const char *str, uint32_t *value)
{
	char *ep;

	if (strncasecmp(str, "0x", 2) == 0) {
		*value = strtoul(str, &ep, 16);
		if (*ep == '\0')
			return 0;
	} else {
		*value = strtoul(str, &ep, 10);
		if (*ep == '\0')
			return 0;
	}
	return -1;
}

static int
parseaddr(const char *addrstr, uint32_t *addr)
{
	if (strcasecmp(addrstr, "ocram") == 0) {
		*addr = OCRAM_S_ADDR;
	} else if (strcasecmp(addrstr, "tcm") == 0) {
		*addr = TCM_ADDR;
	} else if (strcasecmp(addrstr, "ddr") == 0) {
		*addr = DDR_ADDR;
	} else if (parsehex(addrstr, addr) != 0) {
		fprintf(stderr, "%s: illegal load address\n", addrstr);
		return -1;
	}
	return 0;
}

static int
parseaddrrange(const char *addrstr, uint32_t *addr, uint32_t *size)
{
	char *p, *q, *s, *e;
	uint32_t eaddr;
	int rc;

	p = strdup(addrstr);
	if (p == NULL)
		err(3, "malloc");

	rc = 0;

	/*
	 * parse "address:size" or
	 "       "address-address"
	 */
	if ((q = index(p, ':')) != NULL) {
		*q++ = '\0';
		if (parsehex(p, addr) != 0) {
			fprintf(stderr, "%s: illegal address\n", addrstr);
			rc = -1;
			goto done;
		}
		if (parsenum(q, size) != 0) {
			fprintf(stderr, "%s: illegal size\n", addrstr);
			rc = -1;
			goto done;
		}
	} else if ((q = index(p, '-')) != NULL) {
		*q++ = '\0';
		if (parsehex(p, addr) != 0) {
			fprintf(stderr, "%s: illegal address\n", addrstr);
			rc = -1;
			goto done;
		}
		if (parsehex(q, &eaddr) != 0) {
			fprintf(stderr, "%s: illegal address\n", addrstr);
			rc = -1;
			goto done;
		}
		if (*addr >= eaddr) {
			fprintf(stderr, "%s: illegal range\n", addrstr);
			rc = -1;
			goto done;
		}
		*size = eaddr - *addr;

	} else {
		if (parsehex(p, addr) != 0) {
			fprintf(stderr, "%s: illegal address\n", addrstr);
			rc = -1;
			goto done;
		}
		*size = 0x100;
	}

 done:
	free(p);
	return rc;
}

int
main(int argc, char *argv[])
{
	int argleft, ch, i, dry_run;
	uint32_t address, dumpsize = 0x100;
	const char *addrstr, *filename;

	while ((ch = getopt(argc, argv, "v")) != -1) {
		switch (ch) {
		case 'v':
			verbose = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage();

	for (dry_run = 1; dry_run >= 0; dry_run--) {
		for (i = 0, argleft = argc; i < argc;
		    i++, argleft = argc - i) {
			if ((strcmp(argv[i], "load") == 0) || (strcmp(argv[i], "loadv") == 0)) {
				int setupSPPC = 0;
				if (strcmp(argv[i], "loadv") == 0)
					setupSPPC = 1;

				if (argleft < 3)
					usage();
				addrstr = argv[++i];
				filename = argv[++i];
				if (parseaddr(addrstr, &address) == 0) {
					if (dry_run == 0)
						m4_load(address, filename, setupSPPC);
				} else
					dry_run = -1;

			} else if (strcmp(argv[i], "dump") == 0) {
				if (argleft < 2)
					usage();
				addrstr = argv[++i];
				if (parseaddrrange(addrstr, &address, &dumpsize) == 0) {
					if (dry_run == 0)
						memdump(address, dumpsize);
				} else
					dry_run = -1;

			} else if (strcmp(argv[i], "stop") == 0) {
				if (dry_run == 0)
					m4_stop();

			} else if (strcmp(argv[i], "reset") == 0) {
				if (dry_run == 0)
					m4_reset();

			} else if (strcmp(argv[i], "debug") == 0) {
				if (dry_run == 0)
					debugdump();

			} else {
				fprintf(stderr, "unknown command: %s\n",
				    argv[i]);
				dry_run = -1;
			}
		}
	}
	if (dry_run < -1)
		usage();

	return 0;
}
