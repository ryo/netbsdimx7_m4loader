# CortexM4 program loader for NetBSD/imx7

## howto

    # make
    gcc -nostdinc -nostdlib -march=armv7e-m -mthumb   -x assembler-with-cpp -c m4_start.S
    gcc -nostdinc -nostdlib -W -Wall -O2 -march=armv7e-m -mthumb  -c m4_main.c
    m4_main.c: In function 'xputc':
    m4_main.c:15:32: warning: initialization makes pointer from integer without a cast [enabled by default]
      volatile unsigned int *tcml = 0x1fff8000; // debug
                                    ^
    gcc -nostdinc -nostdlib -W -Wall -O2 -march=armv7e-m -mthumb -Wl,-Ttext,0x1fff8000 -Wl,-T ldscript -Wl,-Map -Wl,m4.map -nostdlib   -o m4.elf m4_start.o m4_main.o
    objcopy -O binary m4.elf m4.bin
    gcc -I/usr/src/sys/arch m4loader.c -o m4loader

    # sudo m4loader
    write m4.bin 2284 bytes
    Hello CortexM4

## test program

output "Hello CortexM4" to UART5 (imxuart4 at axi0 addr 0x30a70000 intr 62).
