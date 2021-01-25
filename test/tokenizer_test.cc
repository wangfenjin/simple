#include "gtest/gtest.h"
#include "simple_tokenizer.h"

using namespace simple_tokenizer;

int printFn(void* pCtx, int flags, const char* index, int len, int start, int end) {
  std::cout << "\t" << index << " " << len << " " << start << " " << end << "\n";
  return 0;
}

TEST(simple, tokenizer) {
  SimpleTokenizer* t = new SimpleTokenizer(nullptr, 0);
  std::vector<std::string> arr;
  std::vector<std::string> query;
  arr.push_back("english@\"''");
  query.push_back(R"VAGON(( e n g l i s h* OR english* ) AND "@" AND """" AND "''" AND "''")VAGON");
  arr.push_back("zhou杰伦");
  query.push_back(R"VAGON(( z h o u* OR zhou* ) AND "杰" AND "伦")VAGON");
  for (int i = 0; i < arr.size(); i++) {
    std::string s = arr[i];
    std::cout << s << " as doc:\n";
    t->tokenize(nullptr, FTS5_TOKENIZE_DOCUMENT, s.c_str(), s.length(), printFn);
    std::cout << s << " as query:\n";
    t->tokenize(nullptr, FTS5_TOKENIZE_QUERY, s.c_str(), s.length(), printFn);
    std::cout << s << " as aux:\n";
    std::string result = SimpleTokenizer::tokenize_query(s.c_str(), s.length());
    std::cout << result << "\n";
    ASSERT_EQ(result, query[i]);
  }
}
