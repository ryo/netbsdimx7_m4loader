CC	= armv7--netbsdelf-eabihf-gcc
OBJCOPY	= armv7--netbsdelf-eabihf-objcopy
CC	= gcc
OBJCOPY	= objcopy

AFLAGS	= -nostdinc -nostdlib
CFLAGS	= -nostdinc -nostdlib -W -Wall
CFLAGS	+= -O2
CFLAGS	+= -march=armv7e-m -mthumb
AFLAGS	+= -march=armv7e-m -mthumb

all: m4.bin

m4.bin: m4.elf
	${OBJCOPY} -O binary m4.elf m4.bin

m4.elf: m4_start.o m4_main.o
	${CC} ${CFLAGS} -Wl,-Ttext,0x00180000 -Wl,-T ldscript -Wl,-Map -Wl,m4.map -nostdlib  \
		-o m4.elf m4_start.o m4_main.o

run:
	sudo m4loader load tcm zero16k load ocram m4.bin reset

dump:
	sudo m4loader dump tcm

stop:
	sudo m4loader stop

clean:
	rm -f *.o m4.bin m4.elf m4.map

