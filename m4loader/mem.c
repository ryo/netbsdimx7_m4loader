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

#include <stdio.h>
#include <paths.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include "mem.h"

static int memfd = -1;

void
memopen(void)
{
	if (memfd < 0) {
		memfd = open(_PATH_MEM, O_RDWR);
		if (memfd < 0)
			err(10, "open: " _PATH_MEM);
	}
	return;
}

void
memclose(void)
{
	if (memfd >= 0)
		close(memfd);
	memfd = -1;
}

void
memseek(uint32_t addr)
{
	off_t rc;

	if (memfd < 0)
		errx(11, _PATH_MEM ": not opened");

	rc = lseek(memfd, addr, SEEK_SET);
	if (rc != addr)
		err(12, _PATH_MEM ": cannot seek 0x%08x\n", addr);
}

uint32_t
memread4(uint32_t addr)
{
	ssize_t rc;
	uint32_t v;

	memseek(addr);
	rc = read(memfd, &v, sizeof(uint32_t));
	if (rc < 0)
		err(13, _PATH_MEM ": read");
	return v;
}

void
memwrite4(uint32_t addr, uint32_t value)
{
	ssize_t rc;

	memseek(addr);
	rc = write(memfd, &value, sizeof(uint32_t));
	if (rc < 0)
		err(13, _PATH_MEM ": write");
}

int32_t
memread(uint32_t addr, void *buf, uint32_t size)
{
	ssize_t rc;

	memseek(addr);
	rc = read(memfd, buf, size);
	if (rc < 0)
		err(13, _PATH_MEM ": read");

	return rc;
}

int32_t
memwrite(uint32_t addr, const void *buf, uint32_t size)
{
	ssize_t rc;

	memseek(addr);
	rc = write(memfd, buf, size);
	if (rc < 0)
		err(13, _PATH_MEM ": read");

	return rc;
}
