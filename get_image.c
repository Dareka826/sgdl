#include "get_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include "fetch_url.h"

enum SGDL_CODE sgdl_get_image(unsigned int id, char *result_url, size_t result_url_size) {

	if(id <= 0)
		return SGDL_E_BAD_ID;

	char url[100];
	snprintf(url, 100, "https://gelbooru.com/index.php?page=post&s=view&id=%u", id);

	struct fetch_url_result r;

	if(init_fetch_url_result(&r) != FETCH_URL_E_OK)
		return SGDL_E_FETCH_URL_ERR;

	if(fetch_url(url, &r) != FETCH_URL_E_OK)
		return SGDL_E_FETCH_URL_ERR;

	// Find the original image url in html data
	regex_t regex;
	regmatch_t matches[1];
	int ret;

	// Compile the regex
	ret = regcomp(&regex, "https://[^>]*>Original image</a>", REG_EXTENDED);
	if(ret != 0) return SGDL_E_REGCOMP;

	// Execute the regex
	ret = regexec(&regex, r.ptr, 1, matches, 0);
	if(ret != 0) return SGDL_E_NO_MATCH; // no matches

	const size_t BUFFER_MAX_LENGTH = 200;
	char buf[BUFFER_MAX_LENGTH]; // Buffer for url

	// Read characters until a " character or end of match
	// to determine length of image url
	size_t pos = matches[0].rm_so;
	while(r.ptr[pos] != '"' && pos <= matches[0].rm_eo)
		pos++;

	size_t len = pos - matches[0].rm_so;
	if(len >= BUFFER_MAX_LENGTH) // Restrict the max length
		len = BUFFER_MAX_LENGTH - 1;

	strncpy(buf, r.ptr + matches[0].rm_so, len);
	buf[len++] = '\0'; // End the string

	destroy_fetch_url_result(&r);
	regfree(&regex); // Free the regex

	// Return the url
	strncpy(result_url, buf, (len < result_url_size ? len : result_url_size));

	return SGDL_E_OK;
}

