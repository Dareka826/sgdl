#ifndef __FETCH_URL_H
#define __FETCH_URL_H

#include <stdlib.h>

struct fetch_url_result {
	char *ptr;
	size_t len;
};

int fetch_url(char *url, struct fetch_url_result *result);

int init_fetch_url_result(struct fetch_url_result *r);
int destroy_fetch_url_result(struct fetch_url_result *r);

#endif
