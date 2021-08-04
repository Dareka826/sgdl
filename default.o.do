redo-ifchange config.sh
. ./config.sh

redo-ifchange $2.c

gcc $CFLAGS -MD -MF $2.d $2.c -c -o $3

read DEPS <$2.d
redo-ifchange ${DEPS#*:}
