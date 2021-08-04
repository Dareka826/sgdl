#ifndef __FETCH_URL_H
#define __FETCH_URL_H

#include <stdlib.h>

enum FETCH_URL_CODE {
	FETCH_URL_E_OK,
	FETCH_URL_E_CURL_ERR,
	FETCH_URL_E_MALLOC_ERR,
	FETCH_URL_E_NULLPTR_ERR
};

struct fetch_url_result {
	char *ptr;
	size_t len;
};

enum FETCH_URL_CODE fetch_url(char *url, struct fetch_url_result *result, char *cookie);

enum FETCH_URL_CODE init_fetch_url_result(struct fetch_url_result *r);
void destroy_fetch_url_result(struct fetch_url_result *r);

#endif
