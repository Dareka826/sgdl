redo-ifchange $2.c

# Depend on version.h before build for sgdl
[ "$2" = "sgdl" ] && redo-ifchange version.h

CFLAGS="-Wall -g"
gcc $CFLAGS -MD -MF $2.d -c -o $3 $2.c

# Read generated dependencies
read DEPS<$2.d
redo-ifchange ${DEPS#*:}
