#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "get_image.h"
#include "get_by_tag.h"

#define ARR_LEN(arr) ( sizeof(arr) / sizeof(arr[0]) )

struct option {
	char letter;
	int has_arg; // Either has an arg of is t/f
	union {
		char *arg;
		int is_true;
	};
};

struct option_conflict {
	char opt1;
	char opt2;
};

typedef enum SGDL_CODE (*command_func)(int, struct option*, int, char**);

struct command_def {
	char *name;
	command_func func;
};

void usage(char *progname);

command_func check_command(struct command_def *cmds, int cmd_count, char *cmd);

enum SGDL_CODE parse_opts(int opt_len, struct option *options, int argc, char **args);
enum SGDL_CODE set_option(struct option *opt, int *i, int argc, char **args);

int is_option_set(struct option opt);

int is_digits(char *str);


enum SGDL_CODE  process_ids(int optc, struct option *options, int argc, char **args);
enum SGDL_CODE process_tags(int optc, struct option *options, int argc, char **args);


int main(int argc, char **argv) {

	// Check if enough args
	if(argc < 3) {
		usage(argv[0]);
		return EXIT_SUCCESS;
	}

	int DEBUG = 0;

	{
		char *SGDL_DEBUG = getenv("SGDL_DEBUG");

		if(SGDL_DEBUG != NULL && strcmp(SGDL_DEBUG, "1") == 0)
			DEBUG = 1;
	}

	struct command_def commands[] = {
		{  "id", &process_ids  },
		{ "tag", &process_tags }
	};

	// Check which command
	command_func cmd_f = check_command(commands, ARR_LEN(commands), argv[1]);

	// Specified command not found
	if(cmd_f == NULL) {
		fprintf(stderr, "[E] Command not found: %s\n", argv[1]);
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

	struct option_conflict opt_conflicts[] = {
		{ 'd', 'u' },
		{ 's', 'p' },
		{ 'e', 'p' }
	};

	// Parse options
	if(parse_opts(ARR_LEN(options), options, argc-2, argv+2) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	int first_non_opt = 2;
	for(int i = 0; i < ARR_LEN(options); i++) {
		struct option opt = options[i];

		if(opt.has_arg && opt.arg != NULL) {
			if(DEBUG)
				printf("Option -%c: %s\n", opt.letter, opt.arg);
			first_non_opt += 2;

		} else if(!opt.has_arg && opt.is_true) {
			if(DEBUG)
				printf("Option -%c: %d\n", opt.letter, opt.is_true);
			first_non_opt++;
		}
	}

	for(int i = 0; i < ARR_LEN(opt_conflicts); i++) {
		struct option_conflict oc = opt_conflicts[i];
		int how_many_set = 0;

		for(int j = 0; j < ARR_LEN(options); j++) {
			struct option opt = options[j];

			if(is_option_set(opt) &&
					(opt.letter == oc.opt1 || opt.letter == oc.opt2))
				how_many_set++;
		}

		if(how_many_set > 1) {
			fprintf(stderr, "[E] Conflicting options: -%c & -%c\n",
					oc.opt1, oc.opt2);
			return EXIT_FAILURE;
		}
	}

	if(first_non_opt >= argc) {
		fprintf(stderr, "[E] No command arguments supplied\n");
		return EXIT_FAILURE;
	}

	if(cmd_f(ARR_LEN(options), options,
			argc-first_non_opt, argv+first_non_opt) != SGDL_E_OK)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
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

enum SGDL_CODE parse_opts(int opt_len, struct option *options, int argc, char **args) {

	for(int i = 0; i < argc; i++) {

		if(strlen(args[i]) != 2 || args[i][0] != '-')
			break; // stop on first non-option

		else {
			int parsed = 0;

			for(int j = 0; j < opt_len; j++)
				if(options[j].letter == args[i][1]) {

					if(set_option(&options[j], &i, argc, args) != SGDL_E_OK)
						return SGDL_E_OPTION_ERR;

					parsed = 1;
					break;
				}

			if(!parsed) {
				fprintf(stderr, "[E] Unknown option: -%c\n", args[i][1]);
				return SGDL_E_OPTION_ERR;
			}
		}
	}

	return SGDL_E_OK;
}

enum SGDL_CODE set_option(struct option *opt, int *i, int argc, char **args) {

	if(opt->has_arg) {
		if((*i)+1 >= argc) {
			fprintf(stderr,
					"[E] No argument supplied to option: %c\n", opt->letter);
			return SGDL_E_OPTION_ERR;
		}

		(*i)++;
		opt->arg = args[*i];
	} else opt->is_true = 1;

	return SGDL_E_OK;
}

int is_option_set(struct option opt) {
	int is_set = 0;

	if((opt.has_arg && opt.arg != NULL) ||
			(!opt.has_arg && opt.is_true))
		is_set = 1;

	return is_set;
}

// Check if the string consists of only [0-9]
// It's in a separate function to avoid having the sting with numbers
// in multiple places in memory
int is_digits(char *str) {
	return (strspn(str, "0123456789") == strlen(str));
}

// Downloads images by their id
// Accepts: -d -u
enum SGDL_CODE process_ids(int optc, struct option *options, int argc, char **args) {

	int dl_files = 0;
	int print_id_url = 1;

	for(int i = 0; i < optc; i++) {
		struct option opt = options[i];

		if(opt.letter == 'd')
			dl_files = opt.is_true;
		else if(opt.letter == 'u')
			print_id_url = !opt.is_true;
	}

	int ret = SGDL_E_OK;

	char url[200];
	for(int i = 0; i < argc; i++) {
		// Check if arg is a number, if not print error, but don't quit
		int id;

		if(is_digits(args[i]))
			id = atoi(args[i]);
		else {
			fprintf(stderr, "[E] Not a valid id: %s\n", args[i]);
			ret = SGDL_E_BAD_ID;
			continue;
		}

		if(sgdl_get_image(id, url, sizeof(url)) != SGDL_E_OK)
			strncpy(url, "ERROR", 6);

		if(print_id_url)
			printf("[%d]: %s\n", id, url);
		else
			printf("%s\n", url);
	}

	return ret;
};

enum SGDL_CODE process_tags(int optc, struct option *options, int argc, char **args) {

	// Construct the query
	int query_len = 0;
	for(int i = 0; i < argc; i++)
		query_len += strlen(args[i]);
	query_len += argc; // for joining with ' ' and '\0' at end

	char *query = (char*) malloc(query_len);
	if(query == NULL) return SGDL_E_MALLOC_ERR;

	int index = 0;
	for(int i = 0; i < argc; i++) {
		strcpy(query+index, args[i]);
		index += strlen(args[i]);

		if(i < argc-1) {
			query[index] = ' ';
			index++;
		}
	}
	query[query_len - 1] = '\0';

	// Determine page range (-1 means unrestricted) and fringe content
	int page_range[2] = { -1, -1 };
	int enable_fringe = 0;

	for(int i = 0; i < optc; i++) {
		struct option opt = options[i];

		if(opt.letter == 'f') {
			enable_fringe = opt.is_true;

		} else if(opt.letter == 'p' && opt.arg != NULL) {
			if(is_digits(opt.arg))
				page_range[0] = page_range[1] = atoi(opt.arg);
			else return SGDL_E_OPTION_ERR;

		} else if(opt.letter == 's' && opt.arg != NULL) {
			if(is_digits(opt.arg))
				page_range[0] = atoi(opt.arg);
			else return SGDL_E_OPTION_ERR;

		} else if(opt.letter == 'e' && opt.arg != NULL) {
			if(is_digits(opt.arg))
				page_range[1] = atoi(opt.arg);
			else return SGDL_E_OPTION_ERR;
		}
	}

	int *result_ids = NULL;
	int num_results = 0;

	enum SGDL_CODE ret = SGDL_E_OK;

	if(sgdl_get_by_tag(query, enable_fringe, page_range[0], page_range[1],
			&result_ids, &num_results) == SGDL_E_OK) {

		for(int i = 0; i < num_results; i++) {
			printf("%d\n", result_ids[i]);
		}

	} else ret = SGDL_E_ERR;

	if(result_ids != NULL)
		free(result_ids);
	free(query);

	return ret;
};
