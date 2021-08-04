#include "get_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include "fetch_url.h"

// Assumes *result_url is not in use
enum SGDL_CODE sgdl_get_image(unsigned int id, char **result_url) {

	*result_url = NULL; // Indication if program exits before malloc

	if(id <= 0)
		return SGDL_E_BAD_ID;

	char url[100];
	snprintf(url, 100, "https://gelbooru.com/index.php?page=post&s=view&id=%u", id);

	struct fetch_url_result r;

	if(init_fetch_url_result(&r) != FETCH_URL_E_OK)
		return SGDL_E_FETCH_URL_ERR;

	if(fetch_url(url, &r, NULL) != FETCH_URL_E_OK)
		return SGDL_E_FETCH_URL_ERR;

	// Find the original image url in html data
	regex_t regex;
	regmatch_t matches[2];

	// Compile the regex
	if(regcomp(&regex, "(https://[^\"]+)\"[^>]*>Original image</a>",
			REG_EXTENDED) != 0)
		return SGDL_E_REGCOMP;

	// Execute the regex
	if(regexec(&regex, r.ptr, 2, matches, 0) != 0) { // no matches
		regfree(&regex);
		return SGDL_E_NO_MATCH;
	}

	size_t match_len = matches[1].rm_eo - matches[1].rm_so + 1;
	*result_url = (char*) malloc(match_len);

	strncpy(*result_url, r.ptr + matches[1].rm_so, match_len - 1);
	(*result_url)[match_len - 1] = '\0';

	regfree(&regex);
	destroy_fetch_url_result(&r);

	return SGDL_E_OK;
}

