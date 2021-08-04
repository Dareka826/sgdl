#include "get_by_tag.h"

#include <string.h>
#include <regex.h>
#include <curl/curl.h>
#include "fetch_url.h"

#define SGDL_GELBOORU_IMAGES_PER_PAGE 42
#define SGDL_GELBOORU_PAGE_LIMIT 20000

enum SGDL_CODE sgdl_get_by_tag(char *query, int fringe, int start_page,
		int end_page, int **result_ids, int *num_results) {

	// Make sure page values are correct
	if(start_page < 0) start_page = 0;
	if(start_page > SGDL_GELBOORU_PAGE_LIMIT)
		start_page = SGDL_GELBOORU_PAGE_LIMIT;

	if(end_page < 0) end_page = 0;
	if(end_page > SGDL_GELBOORU_PAGE_LIMIT)
		end_page = SGDL_GELBOORU_PAGE_LIMIT;

	if(start_page > end_page) return SGDL_E_ERR;

	// Escape query
	char *escaped_query = NULL;

	CURL *curl = curl_easy_init();
	if(!curl) return SGDL_E_CURL_ERR;

	escaped_query = curl_easy_escape(curl, query, 0);
	if(!escaped_query) return SGDL_E_CURL_ERR;

	curl_easy_cleanup(curl); // No longer need curl in this function

	// Create fringe content cookie
	char cookie[30];
	snprintf(cookie, sizeof(cookie), "fringeBenefits=%s",
			(fringe ? "yup" : "nope"));

	// Create url buffer
	int url_len = 100 + strlen(escaped_query);
	char *url = (char*) malloc(url_len);

	// Prepare regex
	regex_t regex;
	regcomp(&regex, "<a id=\"p([0-9]+)\" ", REG_EXTENDED);
	regmatch_t matches[2]; // one sub-group
	char *cursor; // Data pointer

	struct fetch_url_result r;

	enum SGDL_CODE ret = SGDL_E_OK;

	// For each page in range
	for(int p = start_page; p <= end_page; p++) {
			int p_id = p * SGDL_GELBOORU_IMAGES_PER_PAGE;

		// Construct url
		snprintf(url, url_len,
			"https://gelbooru.com/index.php?page=post&s=list&tags=%s&pid=%d",
			escaped_query, p_id);

		if(init_fetch_url_result(&r) != FETCH_URL_E_OK) {
			ret = SGDL_E_FETCH_URL_ERR;
			break;
		}

		if(fetch_url(url, &r, cookie) != FETCH_URL_E_OK) {
			ret = SGDL_E_FETCH_URL_ERR;
			destroy_fetch_url_result(&r);
			break;
		}

		// Go through the data
		cursor = r.ptr;
		for(int i = 0; i < SGDL_GELBOORU_IMAGES_PER_PAGE; i++) {

			if(regexec(&regex, cursor, 2, matches, 0))
				break; // no more matches

			int match_len = (matches[1].rm_eo - matches[1].rm_so) + 1;
			char result[match_len];

			strncpy(result, cursor+matches[1].rm_so, match_len-1);
			result[match_len] = '\0';

			cursor += matches[1].rm_eo; // Move start to end of match

			if(*result_ids != NULL) {
				*num_results += 1;
				*result_ids = (int*) realloc(*result_ids,
											sizeof(int) * (*num_results));

			} else {
				*num_results = 1;
				*result_ids = (int*) malloc(sizeof(int));
			}

			(*result_ids)[(*num_results)-1] = atoi(result);
		}

		destroy_fetch_url_result(&r);
	}

	regfree(&regex);
	free(escaped_query);
	free(url);

	return ret;
}
