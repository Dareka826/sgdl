/* *
 * This module contains the struct and init function for getting a string
 * as a result of a libcurl request
 * */

#ifndef __CURL_RESPONSE_STRING_H
#define __CURL_RESPONSE_STRING_H

#include <stdlib.h>

// The struct used for data storage
struct curl_response_string {
	char *ptr;
	size_t len;
};

// Initialize the struct
void init_curl_response_string(struct curl_response_string *s);

// Free the memory used by the struct
void free_curl_response_string(struct curl_response_string *s);

// The callback function for libcurl that writes to the struct
size_t curl_writefunction_response_string(
		void *ptr, size_t size, size_t nmemb, struct curl_response_string *s);

#endif
