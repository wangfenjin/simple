#ifndef ENTRY_H_
#define ENTRY_H_

#include "sqlite3ext.h"

// Add this before the function
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_simple_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);

// Add this after it
#ifdef __cplusplus
}
#endif

#endif  // ENTRY_H_
