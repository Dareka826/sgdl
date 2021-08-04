redo-ifchange config.sh
. ./config.sh

OBJ="fetch_url.o get_image.o main.o"
redo-ifchange $OBJ

gcc $LDFLAGS $OBJ -o $3
