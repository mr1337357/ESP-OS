PREFIX=xtensa-esp32s3-elf
CC=${PREFIX}-gcc
STRIP=${PREFIX}-strip
ELFEDIT=${PREFIX}-elfedit
READELF=${PREFIX}-readelf
OBJDUMP=${PREFIX}-objdump

CFLAGS="-fPIE -nostdlib -g -static"
#CFLAGS="-static-pie -nostdlib"

${CC} ${CFLAGS} lib.c hello.c -o hello

${CC} ${CFLAGS} lib.c init.c -o init
${CC} ${CFLAGS} lib.c sh.c -o sh
