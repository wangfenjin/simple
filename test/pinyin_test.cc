#include "pinyin.h"
#include "gtest/gtest.h"

using namespace simple_tokenizer;

TEST(simple, pinyin_split) {
  PinYin* pinyin = new PinYin();
  auto res = pinyin->split_pinyin("a");
  ASSERT_EQ(res.size(), 1);
  res = pinyin->split_pinyin("zhangliangy");
}
