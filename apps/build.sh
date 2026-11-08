export PREFIX=xtensa-esp32s3-elf
export CC=${PREFIX}-gcc
export STRIP=${PREFIX}-strip
export ELFEDIT=${PREFIX}-elfedit
export READELF=${PREFIX}-readelf
export OBJDUMP=${PREFIX}-objdump
export AR=${PREFIX}-ar

export CFLAGS="-Wl,-q -fPIE -nostdlib -static"
#CFLAGS="-static-pie -nostdlib"

pushd libc
echo "BUILDING LIBC"
make
popd
echo

echo "BULIDING APPS"

${CC} ${CFLAGS} -o hello hello.c libc/esplibc.a
${CC} ${CFLAGS} pwd.c -o pwd libc/esplibc.a
${CC} ${CFLAGS} init.c -o init libc/esplibc.a
${CC} ${CFLAGS} sh.c -o sh libc/esplibc.a
${CC} ${CFLAGS} ls.c -o ls libc/esplibc.a

#${OBJDUMP} -x -r -R -s -S sh > sh.dis

read