#include "get_image.h"
#include "sgdl_common_defs.h"

#include <curl/curl.h>
#include <assert.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl_result_string.h"

void get_image(int id, char* result_url) {
	assert(id >= 0);

	// Init cURL
	CURL *curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "[E]: Failed to init cURL\n");
		exit(EXIT_FAILURE);
	}

	// Generate the request url
	char url[100];
	sprintf(url, "https://gelbooru.com/index.php?page=post&s=view&id=%u", id);

	// Set options
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// Create the response data struct
	struct curl_result_string str;
	init_curl_result_string(&str);

	// Get the response
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writefunction_result_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);

	int success = 0;
	int try_count = RETRY_COUNT;
	while(success == 0 && try_count > 0) {
		// Execute the request
		CURLcode result = curl_easy_perform(curl);

		if(result == CURLE_OK)
			success = 1;
		else try_count--;
	}

	// Find the original image url in html data
	regex_t regex;
	regmatch_t matches[1];
	int ret;

	// Compile the regex
	ret = regcomp(&regex, "https://[^<]*Original image</a>", REG_EXTENDED);
	if(ret != 0) {
		fprintf(stderr, "[E]: regcomp() failed\n");
		exit(EXIT_FAILURE);
	}

	// Execute the regex
	ret = regexec(&regex, str.ptr, 1, matches, 0);
	if(ret != 0) {
		// No matches --> Return empty url
		result_url[0] = '\0';
		return;
	}

	char buf[256]; // Buffer for url
	int len = 0;

	// Read characters until " or end of match
	for(int i = matches[0].rm_so; str.ptr[i] != '"' && i < matches[0].rm_eo; i++)
		buf[len++] = str.ptr[i];

	buf[len++] = '\0'; // End the string

	// Return the url
	printf("URL: %s\n", buf);
	strcpy(result_url, buf);

	// Cleanup
	regfree(&regex); // Free the regex
	free_curl_result_string(&str);
	curl_easy_cleanup(curl);
}

