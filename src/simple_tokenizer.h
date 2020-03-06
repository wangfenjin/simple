#ifndef SIMPLE_TOKENIZER_H_
#define SIMPLE_TOKENIZER_H_

#include <memory>
#include "pinyin.h"
#include "sqlite3ext.h"

typedef int (*xTokenFn)(void *, int, const char *, int, int, int);

namespace simple_tokenizer {

class SimpleTokenizer {
 private:
  static std::unique_ptr<PinYin> pinyin;

 public:
  SimpleTokenizer(const char **zaArg, int nArg);
  int tokenize(void *pCtx, int flags, const char *text, int textLen, xTokenFn xToken);
  static std::string tokenize_query(const char *text, int textLen);
};

}  // namespace simple_tokenizer

extern "C" int fts5_simple_xCreate(void *sqlite3, const char **azArg, int nArg, Fts5Tokenizer **ppOut);
extern "C" int fts5_simple_xTokenize(Fts5Tokenizer *tokenizer_ptr, void *pCtx, int flags, const char *pText, int nText,
                                     xTokenFn xToken);
extern "C" void fts5_simple_xDelete(Fts5Tokenizer *tokenizer_ptr);

extern "C" int sqlite3_simple_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);

#endif  // SIMPLE_TOKENIZER_H_
