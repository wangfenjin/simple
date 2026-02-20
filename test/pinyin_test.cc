#include "pinyin.h"

#include <cstdio>
#include <fstream>
#include <stdexcept>

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

TEST(simple, pinyin_custom_file) {
  std::string path = "simple_custom_pinyin_test.txt";
  std::ofstream file(path);
  ASSERT_TRUE(file.is_open());
  file << "# custom pinyin file\n";
  file << "U+6770: jié # trailing comment\n";
  file.close();

  PinYin pinyin(path);
  auto res = pinyin.get_pinyin("杰");
  ASSERT_EQ(res.size(), 2);
  ASSERT_EQ(res[0], "j");
  ASSERT_EQ(res[1], "jie");
  std::remove(path.c_str());
}

TEST(simple, pinyin_invalid_custom_file) {
  std::string path = "simple_invalid_pinyin_test.txt";
  std::ofstream file(path);
  ASSERT_TRUE(file.is_open());
  file << "invalid line\n";
  file.close();

  EXPECT_THROW(
      {
        PinYin pinyin(path);
        (void)pinyin;
      },
      std::runtime_error);
  std::remove(path.c_str());
}
