# CortexM4 program loader for NetBSD/imx7

## howto

### build m4loader

    % make -C m4loader
    rm -f .gdbinit
    touch .gdbinit
    #   compile  m4loader/m4loader.o
    gcc -O2 -I/usr/src/sys/arch    -std=gnu99   -Werror     -c    m4loader.c
    ctfconvert -g -L VERSION m4loader.o
    #   compile  m4loader/mem.o
    gcc -O2 -I/usr/src/sys/arch    -std=gnu99   -Werror     -c    mem.c
    ctfconvert -g -L VERSION mem.o
    #      link  m4loader/m4loader
    gcc        -o m4loader  m4loader.o mem.o  -Wl,-rpath-link,/lib  -L=/lib     
    ctfmerge -t -g -L VERSION -o m4loader m4loader.o mem.o

### install m4loader

    % sudo make -C m4loader install
    #   install  /usr/local/bin/m4loader
    install  -c  -r -o root -g wheel -m 555   m4loader /usr/local/bin/m4loader

### build example

    % make -C example/hello_uart4_ocram
    gcc -nostdinc -nostdlib -march=armv7e-m -mthumb   -x assembler-with-cpp -c m4_start.S
    gcc -nostdinc -nostdlib -W -Wall -O2 -march=armv7e-m -mthumb  -c m4_main.c
    m4_main.c: In function 'xputc':
    m4_main.c:15:32: warning: initialization makes pointer from integer without a cast [enabled by default]
      volatile unsigned int *tcml = 0x1fff8000; // debug
                                    ^
    gcc -nostdinc -nostdlib -W -Wall -O2 -march=armv7e-m -mthumb -Wl,-Ttext,0x00180000 -Wl,-T ldscript -Wl,-Map -Wl,m4.map -nostdlib   -o m4.elf m4_start.o m4_main.o
    objcopy -O binary m4.elf m4.bin

### run example

    % sudo m4loader load ocram example/hello_uart4_ocram/m4.bin reset
    Hello CortexM4

## what is m4loader?

read Cortex-M4 binary file and write to physical memory using /dev/mem, and reset Cortex-M4.

## test program

output "Hello CortexM4" to UART5 (imxuart4 at axi0 addr 0x30a70000 intr 62).

## reference

http://cache.nxp.com/files/soft_dev_tools/doc/app_note/AN5317.pdf - Loading Code on Cortex-M4 from Linux for the i.MX 6SoloX and i.MX 7Dual/7Solo Application Processors
