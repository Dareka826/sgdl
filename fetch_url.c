#include "fetch_url.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int init_fetch_url_result(struct fetch_url_result *r);
int destroy_fetch_url_result(struct fetch_url_result *r);

size_t fetch_url_curl_writefunc(void *ptr, size_t size, size_t nmemb,
		struct fetch_url_result *r);

int fetch_url(char *url, struct fetch_url_result *result) {
	int ret = 0;

	CURL *curl = curl_easy_init();
	if(!curl) return 1;

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_url_curl_writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, result);

	if(curl_easy_perform(curl) != CURLE_OK)
		ret = 1;

	curl_easy_cleanup(curl);

	return ret;
}

int init_fetch_url_result(struct fetch_url_result *r) {
	r->ptr = (char*) malloc(1);
	if(r->ptr == NULL) return 1;

	r->len = 0;
	r->ptr[0] = '\0';

	return 0;
}

int destroy_fetch_url_result(struct fetch_url_result *r) {
	if(r->ptr == NULL) return 1;

	free(r->ptr);
	r->len = -1;

	return 0;
}

size_t fetch_url_curl_writefunc(void *ptr, size_t size, size_t nmemb,
		struct fetch_url_result *r) {

	size_t data_size = size * nmemb;

	size_t new_len = r->len + data_size;
	r->ptr = realloc(r->ptr, new_len+1);
	if(r->ptr == NULL) return -1;

	memcpy(r->ptr + r->len, ptr, data_size);

	r->len = new_len;
	r->ptr[r->len] = '\0';

	return data_size;
}

