#include <stdio.h>
#include <string.h>
#include "curl_result_string.h"

void init_curl_result_string(struct curl_result_string *s) {
	s->len = 0;
	s->ptr = (char*) malloc(1);

	if(s->ptr == NULL) {
		fprintf(stderr, "[E]: malloc() failed\n");
		exit(EXIT_FAILURE);
	}

	s->ptr[0] = '\0';
}

void free_curl_result_string(struct curl_result_string *s) {
	free(s->ptr);
}

size_t curl_writefunction_result_string(
		void *ptr, size_t size,
		size_t nmemb, struct curl_result_string *s) {

	size_t new_len = s->len + size*nmemb; // Legth after new data is appended

	s->ptr = realloc(s->ptr, new_len+1); // realloc the memory

	if(s->ptr == NULL) {
		fprintf(stderr, "[E]: realloc() failed\n");
		exit(EXIT_FAILURE);
	}

	// Copy the new data into the str
	memcpy(s->ptr + s->len, ptr, size*nmemb);
	s->len = new_len; // Set the new length
	s->ptr[s->len] = '\0'; // Make last char \0

	return size*nmemb; // Return the amount of processed data
}

