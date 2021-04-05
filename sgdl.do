redo-ifchange $2.c
CFLAGS="-Wall -g"
LDFLAGS="-lcurl"

gcc -MD -MF $2.d $CFLAGS $LDFLAGS -o $3 $2.c

read DEPS <$2.d
redo-ifchange ${DEPS#*:}
