DEPS="sgdl.o curl_result_string.o get_image.o"
redo-ifchange $DEPS

CFLAGS="-Wall -g"
LDFLAGS="-lcurl"

gcc $LDFLAGS $CFLAGS $DEPS -o $3
