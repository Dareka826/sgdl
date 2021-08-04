#ifndef __DL_URL_H
#define __DL_URL_H

enum DL_URL_CODE {
	DL_URL_E_OK,
	DL_URL_E_REGCOMP_ERR,
	DL_URL_E_FETCH_URL_ERR,
	DL_URL_E_FILE_ERR,
	DL_URL_E_ERR
};

enum DL_URL_CODE dl_url(char *url, char *fname);

#endif
