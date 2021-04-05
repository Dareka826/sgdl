/* *
 * This module contains the struct and init function for getting a string
 * as a result of a libcurl request
 * */

#ifndef __CURL_RESULT_STRING_H
#define __CURL_RESULT_STRING_H

#include <stdlib.h>

// The struct used for data storage
struct curl_result_string {
	char *ptr;
	size_t len;
};

// Initialize the struct
void init_curl_result_string(struct curl_result_string *s);

// Free the memory used by the struct
void free_curl_result_string(struct curl_result_string *s);

// The callback function for libcurl that writes to the struct
size_t curl_writefunction_result_string(
		void *ptr, size_t size, size_t nmemb, struct curl_result_string *s);

#endif
