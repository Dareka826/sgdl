#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "get_image.h"

int main(int argc, char **argv) {

	if(argc <= 1) {
		printf("Usage: %s ids...\n", argv[0]);
		return EXIT_SUCCESS;
	}

	char url[200];
	for(int i = 1; i < argc; i++) {
		int id = atoi(argv[i]);
		if(sgdl_get_image(id, url, 200) != 0)
			strncpy(url, "ERROR", 6);
		printf("[%d]: %s\n", id, url);
	}

	return EXIT_SUCCESS;
}

