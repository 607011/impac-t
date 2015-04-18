/*
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

   for changes see miniunz.c
*/


#ifndef _miniunz_H
#define _miniunz_H


#ifdef __cplusplus
extern "C" {
#endif

#include "unzip.h"

int do_extract(unzFile uf, int opt_extract_without_path, int opt_overwrite, const char* password);
int do_extract_currentfile(unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password);

#ifdef __cplusplus
}
#endif

#endif /* _miniunz_H */
