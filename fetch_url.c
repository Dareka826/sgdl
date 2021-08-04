#include "fetch_url.h"

#include <curl/curl.h>
#include <string.h>

enum FETCH_URL_CODE init_fetch_url_result(struct fetch_url_result *r);
void destroy_fetch_url_result(struct fetch_url_result *r);

size_t fetch_url_curl_writefunc(void *ptr, size_t size, size_t nmemb,
		struct fetch_url_result *r);

enum FETCH_URL_CODE fetch_url(char *url, struct fetch_url_result *result) {
	enum FETCH_URL_CODE ret = FETCH_URL_E_OK;

	CURL *curl = curl_easy_init();
	if(!curl) return FETCH_URL_E_CURL_ERR;

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_url_curl_writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, result);

	if(curl_easy_perform(curl) != CURLE_OK)
		ret = FETCH_URL_E_CURL_ERR;

	curl_easy_cleanup(curl);

	return ret;
}

enum FETCH_URL_CODE init_fetch_url_result(struct fetch_url_result *r) {
	r->ptr = (char*) malloc(1);
	if(r->ptr == NULL) return FETCH_URL_E_MALLOC_ERR;

	r->len = 0;
	r->ptr[0] = '\0';

	return FETCH_URL_E_OK;
}

void destroy_fetch_url_result(struct fetch_url_result *r) {
	if(r->ptr == NULL) return;

	free(r->ptr);
	r->len = -1;
}

size_t fetch_url_curl_writefunc(void *ptr, size_t size, size_t nmemb,
		struct fetch_url_result *r) {

	size_t data_size = size * nmemb;

	size_t new_len = r->len + data_size;
	r->ptr = realloc(r->ptr, new_len+1);
	if(r->ptr == NULL) return 0; // Notify curl that no data was processed

	memcpy(r->ptr + r->len, ptr, data_size);

	r->len = new_len;
	r->ptr[r->len] = '\0';

	return data_size;
}

