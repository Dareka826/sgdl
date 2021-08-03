#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "get_image.h"

#define ARR_LEN(arr) ( sizeof(arr) / sizeof(arr[0]) )

struct option {
	char letter;
	int has_arg; // Either has an arg of is t/f
	union {
		char *arg;
		int is_true;
	};
};

typedef int (*command_func)(int, struct option*, int, char**);

struct command_def {
	char *name;
	command_func func;
};

void usage(char *progname);
command_func check_command(struct command_def *cmds, int cmd_count, char *cmd);
int parse_opts(int opt_len, struct option *options, int argc, char **args);
int set_option(struct option *opt, int *i, int argc, char **args);

int  process_ids(int optc, struct option *options, int argc, char **args);
int process_tags(int optc, struct option *options, int argc, char **args);


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

	// Parse options
	struct option options[] = {
		{ 'd', 0, { 0 }    }, // Dl instead of printing urls
		{ 'u', 0, { 0 }    }, // Don't print ida with urls
		{ 'h', 0, { 0 }    }, // Show help
		{ 'f', 0, { 0 }    }, // Enable fringe content
		{ 'p', 1, { NULL } }, // Page id for tag mode
		{ 's', 1, { NULL } }, // Page range start for tag mode
		{ 'e', 1, { NULL } }  // Page range end for tag mode
	};

	// Parse options
	if(parse_opts(ARR_LEN(options), options, argc-2, argv+2) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	int first_non_opt = 2;
	for(int i = 0; i < ARR_LEN(options); i++) {
		struct option opt = options[i];

		if(opt.has_arg && opt.arg != NULL) {
			printf("Option -%c: %s\n", opt.letter, opt.arg);
			first_non_opt += 2;

		} else if(!opt.has_arg && opt.is_true) {
			printf("Option -%c: %d\n", opt.letter, opt.is_true);
			first_non_opt++;
		}
	}

	printf("\nfirst_non_opt: %d\n\n", first_non_opt);

	if(first_non_opt >= argc) {
		fprintf(stderr, "[E] No command arguments supplied\n");
		return EXIT_FAILURE;
	}

	return cmd_f(ARR_LEN(options), options,
			argc-first_non_opt, argv+first_non_opt);
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

int parse_opts(int opt_len, struct option *options, int argc, char **args) {

	for(int i = 0; i < argc; i++) {

		if(strlen(args[i]) != 2 || args[i][0] != '-')
			break; // stop on first non-option

		else {
			int parsed = 0;

			for(int j = 0; j < opt_len; j++)
				if(options[j].letter == args[i][1]) {

					if(set_option(&options[j], &i, argc, args) != EXIT_SUCCESS)
						return EXIT_FAILURE;

					parsed = 1;
					break;
				}

			if(!parsed) {
				fprintf(stderr, "[E] Unknown option: -%c\n", args[i][1]);
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}

int set_option(struct option *opt, int *i, int argc, char **args) {

	if(opt->has_arg) {
		if((*i)+1 >= argc) {
			fprintf(stderr,
					"[E]: No argument supplied to option: %c\n", opt->letter);
			return EXIT_FAILURE;
		}

		(*i)++;
		opt->arg = args[*i];
	} else opt->is_true = 1;

	return EXIT_SUCCESS;
}

int process_ids(int optc, struct option *options, int argc, char **args) {

	char url[200];
	for(int i = 0; i < argc; i++) {
		int id = atoi(args[i]);
		if(sgdl_get_image(id, url, 200) != 0)
			strncpy(url, "ERROR", 6);
		printf("[%d]: %s\n", id, url);
	}

	return EXIT_SUCCESS;
};

int process_tags(int optc, struct option *options, int argc, char **args) {
	printf("Tag mode not implemented\n");

	return EXIT_SUCCESS;
};

