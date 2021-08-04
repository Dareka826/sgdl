#ifndef __FETCH_URL_H
#define __FETCH_URL_H

#include <stdlib.h>

enum fetch_url_code {
	OK,
	CURL_ERR,
	MALLOC_ERR,
	NULLPTR_ERR,

};

struct fetch_url_result {
	char *ptr;
	size_t len;
};

enum fetch_url_code fetch_url(char *url, struct fetch_url_result *result);

enum fetch_url_code init_fetch_url_result(struct fetch_url_result *r);
enum fetch_url_code destroy_fetch_url_result(struct fetch_url_result *r);

#endif
