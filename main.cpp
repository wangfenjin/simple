#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <vector>

namespace pinyin {

class PinYin {
private:
  std::map<int, std::string> pinyin;

  const std::map<std::string, char> tone_to_plain = {
      {"ā", 'a'}, {"á", 'a'}, {"ǎ", 'a'}, {"à", 'a'}, {"ē", 'e'}, {"é", 'e'},
      {"ě", 'e'}, {"è", 'e'}, {"ế", 'e'}, {"ề", 'e'}, {"ō", 'o'}, {"ó", 'o'},
      {"ǒ", 'o'}, {"ò", 'o'}, {"ī", 'i'}, {"í", 'i'}, {"ǐ", 'i'}, {"ì", 'i'},
      {"ū", 'u'}, {"ú", 'u'}, {"ǔ", 'u'}, {"ù", 'u'}, {"ǘ", 'u'}, {"ǚ", 'u'},
      {"ǜ", 'u'}, {"ń", 'n'}, {"ň", 'n'}, {"ǹ", 'n'}, {"ḿ", 'm'},
  };

  std::string to_plain(const std::string& input) {
    std::set<std::string> s;
    std::string value;
    auto arr = toUtf8Array(input);
    for (std::string c : arr) {
      if (c == ",") {
        s.insert(value);
        value.clear();
      } else if (auto it = tone_to_plain.find(c); it != tone_to_plain.end()) {
        value += it->second;
      } else {
        value += c;
      }
    }
    s.insert(value);
    std::string res;
    for (auto const &e : s) {
      res += e;
      res += ',';
    }
    return res.erase(res.size() - 1);
  }

  std::map<int, std::string> build_pinyin_map() {
    std::ifstream pinyin_file("./contrib/pinyin.txt");
    std::string line;
    std::map<int, std::string> pinyin;
    std::regex re{R"(U\+(\w+):\s+(\S+)\s+.*)"};
    std::smatch match;
    while (std::getline(pinyin_file, line)) {
      if (std::regex_match(line, match, re)) {
        int codepoint = (int)std::stoul(match[1], 0, 16);
        pinyin[codepoint] = to_plain(match[2]);
      }
    }
    return pinyin;
  }

  size_t get_str_len(unsigned char byte) {
    if (byte >= 0xFC)
      return 6;
    else if (byte >= 0xF8)
      return 5;
    else if (byte >= 0xF0)
      return 4;
    else if (byte >= 0xE0)
      return 3;
    else if (byte >= 0xC0)
      return 2;
    return 1;
  }

  std::vector<std::string> toUtf8Array(const std::string &input) {
    std::vector<std::string> result;
    for (size_t i = 0, len = 0; i != input.length(); i += len) {
      unsigned char byte = (unsigned char)input[i];
      len = get_str_len(byte);
      result.push_back(input.substr(i, len));
    }
    return result;
  }

  int codepoint(const std::string &u) {
    int l = u.length();
    size_t len = get_str_len((unsigned char)u[0]);
    if (l != len)
      return -1;
    switch (len) {
    case 1:
      return (unsigned char)u[0];
    case 2:
      return ((unsigned char)u[0] - 192) * 64 + ((unsigned char)u[1] - 128);
    case 3: // most Chinese char in here
      return ((unsigned char)u[0] - 224) * 4096 +
             ((unsigned char)u[1] - 128) * 64 + ((unsigned char)u[2] - 128);
    case 4:
      return ((unsigned char)u[0] - 240) * 262144 +
             ((unsigned char)u[1] - 128) * 4096 +
             ((unsigned char)u[2] - 128) * 64 + ((unsigned char)u[3] - 128);
    default:
      return -1;
    }
  }

public:
  std::vector<std::string> get_pinyin(const std::string &chinese) {
    std::vector<std::string> result;
    std::vector<std::string> arr = toUtf8Array(chinese);
    for (auto s : arr) {
      result.push_back(pinyin[codepoint(s)]);
    }
    return result;
  }
  PinYin() { pinyin = build_pinyin_map(); }
};

} // namespace pinyin

int main() {
  pinyin::PinYin py;

  // search pinyin
  std::ifstream name_file("./contrib/name.txt");
  std::string line;
  while (std::getline(name_file, line)) {
    // delete ending \r
    if (line[line.length() - 1] == '\r') {
      line.erase(line.size() - 1);
    }
    std::cout << line << " ";
    std::vector<std::string> arr = py.get_pinyin(line);
    for (auto s : arr) {
      std::cout << s << " ";
    }
    std::cout << "\n";
  }

  // for (auto iter = pinyin.begin(); iter != pinyin.end(); iter++) {
  //   std::cout << iter->first << " " << iter->second << "\n";
  // }
  return 0;
}
