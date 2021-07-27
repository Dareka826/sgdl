OBJ="fetch_url.o get_image.o main.o"

redo-ifchange $OBJ
gcc -lcurl $OBJ -o $3
