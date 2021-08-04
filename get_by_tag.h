#ifndef __SGDL_GET_BY_TAG_H
#define __SGDL_GET_BY_TAG_H

#include <stdlib.h>
#include "sgdl_code.h"

enum SGDL_CODE sgdl_get_by_tag(char *query, int fringe, int start_page,
		int end_page, int **result_ids, int *num_results);

#endif
