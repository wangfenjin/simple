#ifndef SIMPLE_TOKENIZER_H_
#define SIMPLE_TOKENIZER_H_

#include <memory>

#ifdef USE_JIEBA
#include "cppjieba/Jieba.hpp"
#endif
#include "pinyin.h"
#include "sqlite3ext.h"

typedef int (*xTokenFn)(void *, int, const char *, int, int, int);

namespace simple_tokenizer {

#ifdef USE_JIEBA
extern std::string jieba_dict_path;
#endif

enum class TokenCategory {
  SPACE,
  ASCII_ALPHABETIC,
  DIGIT,
  OTHER,
};

class SimpleTokenizer {
 private:
  static PinYin *get_pinyin();
  bool enable_pinyin = true;

 public:
  SimpleTokenizer(const char **zaArg, int nArg);
  int tokenize(void *pCtx, int flags, const char *text, int textLen, xTokenFn xToken) const;
  static std::string tokenize_query(const char *text, int textLen, int flags = 1);
#ifdef USE_JIEBA
  static std::string tokenize_jieba_query(const char *text, int textLen, int flags = 1);
#endif

 private:
  static void append_result(std::string &result, std::string part, TokenCategory category, int offset, int flags);
};

}  // namespace simple_tokenizer

extern "C" int fts5_simple_xCreate(void *sqlite3, const char **azArg, int nArg, Fts5Tokenizer **ppOut);
extern "C" int fts5_simple_xTokenize(Fts5Tokenizer *tokenizer_ptr, void *pCtx, int flags, const char *pText, int nText,
                                     xTokenFn xToken);
extern "C" void fts5_simple_xDelete(Fts5Tokenizer *tokenizer_ptr);

extern "C" int sqlite3_simple_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);

#endif  // SIMPLE_TOKENIZER_H_
