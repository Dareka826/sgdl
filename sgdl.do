
redo-ifchange config.rc
. ./config.rc

OBJ="main.o curl_response_string.o get_image.o"
redo-ifchange $OBJ

gcc $LDFLAGS $OBJ -o $3

