#include <stdio.h>
#include <stdlib.h>
#include "get_image.h"

int main(int argc, char **argv) {
	char url[200];
	sgdl_get_image(6311288, url, 200);
	printf("RESULT: %s\n", url);

	return EXIT_SUCCESS;
}

