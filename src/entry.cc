#include "simple_highlight.h"
#include "simple_tokenizer.h"
SQLITE_EXTENSION_INIT1

#include <cstring>
#include <new>

int fts5_simple_xCreate(void *sqlite3, const char **azArg, int nArg, Fts5Tokenizer **ppOut) {
  (void)sqlite3;
  auto *p = new simple_tokenizer::SimpleTokenizer(azArg, nArg);
  *ppOut = reinterpret_cast<Fts5Tokenizer *>(p);
  return SQLITE_OK;
}

int fts5_simple_xTokenize(Fts5Tokenizer *tokenizer_ptr, void *pCtx, int flags, const char *pText, int nText,
                          xTokenFn xToken) {
  auto *p = (simple_tokenizer::SimpleTokenizer *)tokenizer_ptr;
  return p->tokenize(pCtx, flags, pText, nText, xToken);
}

void fts5_simple_xDelete(Fts5Tokenizer *p) {
  auto *pST = (simple_tokenizer::SimpleTokenizer *)p;
  delete (pST);
}

/*
** Return a pointer to the fts5_api pointer for database connection db.
** If an error occurs, return NULL and leave an error in the database
** handle (accessible using sqlite3_errcode()/errmsg()).
*/
static int fts5_api_from_db(sqlite3 *db, fts5_api **ppApi) {
  sqlite3_stmt *pStmt = 0;
  int rc;

  *ppApi = 0;
  rc = sqlite3_prepare(db, "SELECT fts5(?1)", -1, &pStmt, 0);
  if (rc == SQLITE_OK) {
    sqlite3_bind_pointer(pStmt, 1, reinterpret_cast<void *>(ppApi), "fts5_api_ptr", 0);
    (void)sqlite3_step(pStmt);
    rc = sqlite3_finalize(pStmt);
  }

  return rc;
}

#ifdef USE_JIEBA
static void jieba_dict(sqlite3_context *pCtx, int nVal, sqlite3_value **apVal) {
  if (nVal >= 1) {
    const char *text = (const char *)sqlite3_value_text(apVal[0]);
    if (text) {
      std::string tmp(text);
      char sep = '/';
#ifdef _WIN32
      sep = '\\';
#endif
      if (tmp.back() != sep) {  // Need to add a
        tmp += sep;             // path separator
      }
      simple_tokenizer::jieba_dict_path = tmp;
      sqlite3_result_text(pCtx, tmp.c_str(), -1, SQLITE_TRANSIENT);
      return;
    }
  }
  sqlite3_result_null(pCtx);
}

static void jieba_query(sqlite3_context *pCtx, int nVal, sqlite3_value **apVal) {
  if (nVal >= 1) {
    const char *text = (const char *)sqlite3_value_text(apVal[0]);
    if (text) {
      int flags = 1;
      if (nVal >= 2) {
        flags = atoi((const char *)sqlite3_value_text(apVal[1]));
      }
      std::string result = simple_tokenizer::SimpleTokenizer::tokenize_jieba_query(text, (int)std::strlen(text), flags);
      sqlite3_result_text(pCtx, result.c_str(), -1, SQLITE_TRANSIENT);
      return;
    }
  }
  sqlite3_result_null(pCtx);
}
#endif

static void simple_query(sqlite3_context *pCtx, int nVal, sqlite3_value **apVal) {
  if (nVal >= 1) {
    const char *text = (const char *)sqlite3_value_text(apVal[0]);
    if (text) {
      int flags = 1;
      if (nVal >= 2) {
        flags = atoi((const char *)sqlite3_value_text(apVal[1]));
      }
      std::string result = simple_tokenizer::SimpleTokenizer::tokenize_query(text, (int)std::strlen(text), flags);
      sqlite3_result_text(pCtx, result.c_str(), -1, SQLITE_TRANSIENT);
      return;
    }
  }
  sqlite3_result_null(pCtx);
}

int sqlite3_simple_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
  (void)pzErrMsg;
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi)

  rc = sqlite3_create_function(db, "simple_query", -1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, &simple_query, NULL,
                               NULL);
#ifdef USE_JIEBA
  rc = sqlite3_create_function(db, "jieba_query", -1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, &jieba_query, NULL,
                               NULL);
  rc = sqlite3_create_function(db, "jieba_dict", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, NULL, &jieba_dict, NULL, NULL);
#endif

  // fts5_tokenizer tokenizer = {fts5AsciiCreate, fts5AsciiDelete, fts5AsciiTokenize };
  fts5_tokenizer tokenizer = {fts5_simple_xCreate, fts5_simple_xDelete, fts5_simple_xTokenize};
  fts5_api *fts5api;
  rc = fts5_api_from_db(db, &fts5api);
  if (rc != SQLITE_OK) return rc;
  if (fts5api == 0 || fts5api->iVersion < 2) {
    return SQLITE_ERROR;
  }
  rc = fts5api->xCreateTokenizer(fts5api, "simple", reinterpret_cast<void *>(fts5api), &tokenizer, NULL);
  rc =
      fts5api->xCreateFunction(fts5api, "simple_highlight", reinterpret_cast<void *>(fts5api), &simple_highlight, NULL);
  rc = fts5api->xCreateFunction(fts5api, "simple_highlight_pos", reinterpret_cast<void *>(fts5api),
                                &simple_highlight_pos, NULL);
  rc = fts5api->xCreateFunction(fts5api, "simple_snippet", reinterpret_cast<void *>(fts5api), &simple_snippet, NULL);
  return rc;
}
