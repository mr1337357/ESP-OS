export PREFIX=xtensa-esp32s3-elf
export CC=${PREFIX}-gcc
export STRIP=${PREFIX}-strip
export ELFEDIT=${PREFIX}-elfedit
export READELF=${PREFIX}-readelf
export OBJDUMP=${PREFIX}-objdump
export AR=${PREFIX}-ar

APPS="hello pwd init sh ls cat echo gpio tm1650 ds1302"

if [ $# -ge 1 ]
then
    if [ $1 == "clean" ]
    then
        echo cleaning
        for APP in $APPS
        do
            rm $APP
        done
        exit
    fi
fi

CFLAGS="-Wall -Wl,-gc-sections -Wl,-q -fPIE -nostdlib -static -static-libgcc -lgcc"
#CFLAGS="-static-pie -nostdlib"

pushd libc
echo "BUILDING LIBC"
CFLAGS="${CFLAGS} -ffunction-sections -I../inc" make
popd
echo

echo "BULIDING APPS"
CFLAGS="${CFLAGS} -Iinc"

for APP in $APPS
do
    echo building $APP
    ${CC} ${CFLAGS} -o $APP ${APP}.c libc/esplibc.a
done

echo done
read
