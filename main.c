#include <stdio.h>
#include <stdlib.h>
#include "sgdl.h"

int main(int argc, char **argv) {
	char url[100];
	get_image(atoi(argv[1]), url);
	printf("Got url: %s\n", url);

	return EXIT_SUCCESS;
}

