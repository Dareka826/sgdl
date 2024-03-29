#include "dl_url.h"

#include <stdio.h>
#include <regex.h>
#include <string.h>
#include "fetch_url.h"

enum DL_URL_CODE dl_url(char *url, char *fname) {

	int fname_len = strlen(fname) + 1;
	char *filename = (char*) malloc(fname_len);
	strncpy(filename, fname, fname_len-1);
	filename[fname_len-1] = '\0';

	// Try to determine extension
	regex_t regex;
	regmatch_t matches[1];

	if(regcomp(&regex, "\\.[^.]+$", REG_EXTENDED) != 0) {
		free(filename);
		return DL_URL_E_REGCOMP_ERR;
	}

	if(regexec(&regex, url, 1, matches, 0) == 0) {
		int match_len = matches[0].rm_eo - matches[0].rm_so;

		filename = (char*) realloc(filename, fname_len + match_len);
		strncpy(filename+fname_len-1, url+matches[0].rm_so, match_len);
		fname_len += match_len;
		filename[fname_len-1] = '\0';
	}

	enum DL_URL_CODE ret = DL_URL_E_OK;

	// Get image data
	struct fetch_url_result r;
	if(init_fetch_url_result(&r) == FETCH_URL_E_OK) {
		if(fetch_url(url, &r, NULL) == FETCH_URL_E_OK) {

			// Write to file
			FILE *f = fopen(filename, "w");
			if(f) {
				int success = 1;

				for(int i = 0; i < r.len; i++)
					if(fputc(r.ptr[i], f) == EOF) {
						success = 0;
						break;
					}

				fclose(f);

				if(!success) {
					remove(filename);
					ret = DL_URL_E_FILE_ERR;
				}
			} else
				ret = DL_URL_E_FILE_ERR;
		} else
			ret = DL_URL_E_FETCH_URL_ERR;
	} else
		ret = DL_URL_E_FETCH_URL_ERR;

	destroy_fetch_url_result(&r);
	regfree(&regex);
	free(filename);
	return ret;
}
