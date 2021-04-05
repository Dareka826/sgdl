redo-ifchange $2.c

CFLAGS="-Wall -g"
gcc $CFLAGS -MD -MF $2.d -c -o $3 $2.c

# Read generated dependencies
read DEPS<$2.d
redo-ifchange ${DEPS#*:}
