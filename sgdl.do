redo-ifchange config.sh .env
. ./config.sh
. ./.env

OBJ="fetch_url.o dl_url.o get_image.o get_by_tag.o main.o"
redo-ifchange $OBJ

gcc $LDFLAGS $OBJ -o $3
