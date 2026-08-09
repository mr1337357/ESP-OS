#PREFIX=/home/deck/.arduino15/packages/esp32/tools/esp-x32/2601/bin/xtensa-esp32s3-elf
PREFIX=xtensa-esp32s3-elf
CC=${PREFIX}-gcc
STRIP=${PREFIX}-strip
ELFEDIT=${PREFIX}-elfedit
READELF=${PREFIX}-readelf
OBJDUMP=${PREFIX}-objdump

CFLAGS="-fPIE -nostdlib -g -static"
#CFLAGS="-static-pie -nostdlib"

${CC} ${CFLAGS} lib.c string.c launcher.c -o launcher.elf
${CC} ${CFLAGS} lib.c string.c hello.c -o hello.elf
${CC} ${CFLAGS} lib.c string.c filetest.c -o filetest.elf

CFLAGS="-fPIE -g -static"
${CC} ${CFLAGS} lib2.c init.c -o init.elf

${OBJDUMP} -S init.elf > init.src
#${OBJDUMP} -S launcher.elf > launcher.src
#${READELF} -a launcher.elf > launcher.readelf
