CC	= armv7--netbsdelf-eabihf-gcc
OBJCOPY	= armv7--netbsdelf-eabihf-objcopy
CC	= gcc
OBJCOPY	= objcopy

AFLAGS	= -nostdinc -nostdlib
CFLAGS	= -nostdinc -nostdlib -W -Wall
CFLAGS	+= -O2
CFLAGS	+= -march=armv7e-m -mthumb
AFLAGS	+= -march=armv7e-m -mthumb

all: m4.bin m4loader

m4.bin: m4.elf
	${OBJCOPY} -O binary m4.elf m4.bin

m4.elf: m4_start.o m4_main.o
	${CC} ${CFLAGS} -Wl,-Ttext,0x1fff8000 -Wl,-T ldscript -Wl,-Map -Wl,m4.map -nostdlib  \
		-o m4.elf m4_start.o m4_main.o



#
# loader program
#
m4loader: m4loader.c
	gcc -I/usr/src/sys/arch m4loader.c -o m4loader


clean: clean-tmp

clean-tmp:
	rm -f *.o m4.bin m4.elf m4.map m4loader

