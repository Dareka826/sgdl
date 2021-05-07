
redo-ifchange config.rc
. ./config.rc

redo-ifchange $2.c

gcc $CFLAGS -c $2.c -o $3

