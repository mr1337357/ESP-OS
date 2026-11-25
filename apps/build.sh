export PREFIX=xtensa-esp32s3-elf
export CC=${PREFIX}-gcc
export STRIP=${PREFIX}-strip
export ELFEDIT=${PREFIX}-elfedit
export READELF=${PREFIX}-readelf
export OBJDUMP=${PREFIX}-objdump
export AR=${PREFIX}-ar

CFLAGS="-Wall -Wl,-q -fPIE -nostdlib -static -static-libgcc -lgcc"
#CFLAGS="-static-pie -nostdlib"

pushd libc
echo "BUILDING LIBC"
CFLAGS="${CFLAGS} -I../inc" make
popd
echo

echo "BULIDING APPS"
CFLAGS="${CFLAGS} -Iinc"

${CC} ${CFLAGS} -o hello hello.c libc/esplibc.a
${CC} ${CFLAGS} pwd.c -o pwd libc/esplibc.a
${CC} ${CFLAGS} init.c -o init libc/esplibc.a
${CC} ${CFLAGS} sh.c -o sh libc/esplibc.a
${CC} ${CFLAGS} ls.c -o ls libc/esplibc.a
${CC} ${CFLAGS} cat.c -o cat libc/esplibc.a
${CC} ${CFLAGS} echo.c -o echo libc/esplibc.a
${CC} ${CFLAGS} gpio.c -o gpio libc/esplibc.a

${CC} ${CFLAGS} tm1650.c -o tm1650 libc/esplibc.a
${CC} ${CFLAGS} ds1302.c -o ds1302 libc/esplibc.a

#${OBJDUMP} -x -r -R -s -S sh > sh.dis

echo done
read
