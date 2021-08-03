#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "get_image.h"

#define ARR_LEN(arr) ( sizeof(arr) / sizeof(arr[0]) )

typedef int (*command_func)(int, char**);

struct command_def {
	char *name;
	command_func func;
};

void usage(char *progname);
command_func check_command(struct command_def *cmds, int cmd_count, char *cmd);

int  process_ids(int argc, char **args);
int process_tags(int argc, char **args);


int main(int argc, char **argv) {

	// Check if enough args
	if(argc < 3) {
		usage(argv[0]);
		return EXIT_SUCCESS;
	}

	struct command_def commands[] = {
		{  "id", &process_ids  },
		{ "tag", &process_tags }
	};

	// Check which command
	command_func cmd_f = check_command(commands, ARR_LEN(commands), argv[1]);

	// Specified command not found
	if(cmd_f == NULL) {
		fprintf(stderr, "[E]: Command not found: %s\n", argv[1]);
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	return cmd_f(argc, argv);
}


void usage(char *progname) {
	fprintf(stderr, "Usage: %s command [options] args...\n", progname);
	fprintf(stderr, "\n");

	fprintf(stderr, "Commands:\n");
	fprintf(stderr, "    id\t\tDownload images by id\n");
	fprintf(stderr, "    tag\t\tDownload images by tag search\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "Global Options:\n");
	fprintf(stderr, "    -d\t\tDownload files instead of printing urls\n");
	fprintf(stderr, "    -u\t\tOnly print urls\n");
	fprintf(stderr, "    -h\t\tDisplay this help\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "Tag Options:\n");
	fprintf(stderr, "    -f\t\tEnable fringe results\n");
	fprintf(stderr, "    -p\t\tPage id\n");
	fprintf(stderr, "    -s\t\tPage range start\n");
	fprintf(stderr, "    -e\t\tPage range end\n");
	fprintf(stderr, "\n");
}

command_func check_command(struct command_def *cmds, int cmds_len, char *cmd) {
	command_func ret = NULL;

	for(int i = 0; i < cmds_len; i++)
		if(strcmp(cmd, cmds[i].name) == 0) {
			ret = cmds[i].func;
			break;
		}

	return ret;
}

int process_ids(int argc, char **args) {

	char url[200];
	for(int i = 2; i < argc; i++) {
		int id = atoi(args[i]);
		if(sgdl_get_image(id, url, 200) != 0)
			strncpy(url, "ERROR", 6);
		printf("[%d]: %s\n", id, url);
	}

	return EXIT_SUCCESS;
};

int process_tags(int argc, char **args) {
	printf("Tag mode not implemented\n");

	return EXIT_SUCCESS;
};

