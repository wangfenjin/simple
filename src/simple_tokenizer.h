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

#endif  // SIMPLE_TOKENIZER_H_
