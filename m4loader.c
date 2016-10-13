#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include <arm/imx/imx7_srcreg.h>

#define	TCM_L_SIZE	(32*1024)
#define	TCM_H_SIZE	(32*1024)
#define	TCM_SIZE	(TCM_L_SIZE + TCM_H_SIZE)

char tcmbuf[TCM_SIZE];

uint32_t
memread4(int memfd, uint32_t addr)
{
	uint32_t v;

	lseek(memfd, addr, 0);
	read(memfd, &v, sizeof(uint32_t));
	return v;
}

void
memwrite4(int memfd, uint32_t addr, uint32_t value)
{
	lseek(memfd, addr, 0);
	write(memfd, &value, sizeof(uint32_t));
}

int
main(int argc, char *argv[])
{
	int memfd, fd;
	ssize_t len;
	off_t off;
	uint32_t v, *p;
	int i;

	fd = open("m4.bin", O_RDONLY);
	if (fd < 0)
		err(1, "open: m4.bin");
	len = read(fd, tcmbuf, sizeof(tcmbuf));
	close(fd);

	memfd = open("/dev/mem", O_RDWR);
	if (memfd < 0)
		err(1, "open: /dev/mem");

#define	M4_BOOT_OCRAM_S	0x00180000
#define	M4_BOOT_TCM_L	0x007f8000
	off = lseek(memfd, M4_BOOT_TCM_L, 0);
	if (off == -1)
		err(2, "lseek: 0x%llx", M4_BOOT_TCM_L);

	len = write(memfd, tcmbuf, len);
	if (len < 0)
		err(3, "write to /dev/mem");
	printf("write m4.bin %d bytes\n", len);

	/* setup stack/PC */
	p = (uint32_t *)tcmbuf;
	memwrite4(memfd, M4_BOOT_OCRAM_S, p[0]);
	memwrite4(memfd, M4_BOOT_OCRAM_S + 4, p[1]);

	sync();


	/* reset M4 core */
#define SRC_BASE	0x30390000
	v = memread4(memfd, SRC_BASE + SRC_M4RCR);
	v &= ~SRC_M4RCR_ENABLE_M4;
	memwrite4(memfd, SRC_BASE + SRC_M4RCR, v);

	v = memread4(memfd, SRC_BASE + SRC_M4RCR);
	v |= SRC_M4RCR_ENABLE_M4;
	memwrite4(memfd, SRC_BASE + SRC_M4RCR, v);

	v = memread4(memfd, SRC_BASE + SRC_M4RCR);
	v &= ~SRC_M4RCR_SW_M4C_NON_SCLR_RST;
	memwrite4(memfd, SRC_BASE + SRC_M4RCR, v);

	v = memread4(memfd, SRC_BASE + SRC_M4RCR);
	v |= SRC_M4RCR_SW_M4C_RST;
	memwrite4(memfd, SRC_BASE + SRC_M4RCR, v);


#if 1
	// debug

	for (;;) {
		v = memread4(memfd, M4_BOOT_TCM_L);
		printf("TCM_L:  0x%p = %08x\n", M4_BOOT_TCM_L, v);
		v = memread4(memfd, M4_BOOT_TCM_L + 4);
		printf("TCM_L:  0x%p = %08x\n", M4_BOOT_TCM_L + 4, v);
		v = memread4(memfd, M4_BOOT_OCRAM_S);
		printf("OCRAM_S:0x%p = %08x\n", M4_BOOT_OCRAM_S, v);
		v = memread4(memfd, M4_BOOT_OCRAM_S + 4);
		printf("OCRAM_S:0x%p = %08x\n", M4_BOOT_OCRAM_S + 4, v);
		printf("\n");
		fflush(stdout);
		sleep(1);
	}

	for (i = 0; i < 100000; i++) {
		v = memread4(memfd, SRC_BASE + SRC_M4RCR);
		printf("v = %08x\r", v);
		fflush(stdout);
	}
#endif

	return 0;
}
