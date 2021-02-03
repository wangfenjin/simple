#include "pinyin.h"

#include "gtest/gtest.h"

using namespace simple_tokenizer;

TEST(simple, pinyin_split) {
  PinYin* pinyin = new PinYin();
  auto res = pinyin->split_pinyin("a");
  ASSERT_EQ(res.size(), 1);
  res = pinyin->split_pinyin("ba");
  ASSERT_EQ(res.size(), 2);
  res = pinyin->split_pinyin("zhangliangy");
  ASSERT_EQ(res.size(), 4);
  for (auto r : res) std::cout << r << "\t";
  std::cout << std::endl;
}
