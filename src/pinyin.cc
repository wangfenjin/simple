#include "pinyin.h"

#include <cmrc/cmrc.hpp>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

CMRC_DECLARE(pinyin_text);

namespace simple_tokenizer {

PinYin::PinYin() { pinyin = build_pinyin_map(); }

std::set<std::string> PinYin::to_plain(const std::string &input) {
  std::set<std::string> s;
  std::string value;
  for (size_t i = 0, len = 0; i != input.length(); i += len) {
    auto byte = input[i];
    if (byte == ',') {
      s.insert(value);
      s.insert(value.substr(0, 1));
      value.clear();
      continue;
    }
    len = get_str_len((unsigned char)byte);
    if (len == 1) {
      value.push_back(byte);
      continue;
    }
    auto it = tone_to_plain.find(input.substr(i, len));
    if (it != tone_to_plain.end()) {
      value.push_back(it->second);
    } else {
      value.push_back(byte);
    }
  }
  s.insert(value);
  s.insert(value.substr(0, 1));
  return s;
}

// clang-format off
std::map<int, std::vector<std::string> > PinYin::build_pinyin_map() {
  std::map<int, std::vector<std::string> > map;
  // clang-format on
  auto fs = cmrc::pinyin_text::get_filesystem();
  auto pinyin_data = fs.open("contrib/pinyin.txt");
  std::istringstream pinyin_file(std::string(pinyin_data.begin(), pinyin_data.end()));
  std::string line;
  char delimiter = ' ';
  std::string cp, py;
  while (std::getline(pinyin_file, line)) {
    if (line.length() == 0 || line[0] == '#') continue;
    std::stringstream tokenStream(line);
    std::getline(tokenStream, cp, delimiter);
    std::getline(tokenStream, py, delimiter);
    int codepoint = static_cast<int>(std::stoul(cp.substr(2, cp.length() - 3), 0, 16l));
    std::set<std::string> s = to_plain(py);
    std::vector<std::string> m(s.size());
    std::copy(s.begin(), s.end(), m.begin());
    map[codepoint] = m;
  }
  return map;
}

// Get UTF8 character encoding length(via first byte)
int PinYin::get_str_len(unsigned char byte) {
  if (byte >= 0xF0)
    return 4;
  else if (byte >= 0xE0)
    return 3;
  else if (byte >= 0xC0)
    return 2;
  return 1;
}

// get the first valid utf8 string's code point
int PinYin::codepoint(const std::string &u) {
  size_t l = u.length();
  if (l < 1) return -1;
  size_t len = get_str_len((unsigned char)u[0]);
  if (l < len) return -1;
  switch (len) {
    case 1:
      return (unsigned char)u[0];
    case 2:
      return ((unsigned char)u[0] - 192) * 64 + ((unsigned char)u[1] - 128);
    case 3:  // most Chinese char in here
      return ((unsigned char)u[0] - 224) * 4096 + ((unsigned char)u[1] - 128) * 64 + ((unsigned char)u[2] - 128);
    case 4:
      return ((unsigned char)u[0] - 240) * 262144 + ((unsigned char)u[1] - 128) * 4096 +
             ((unsigned char)u[2] - 128) * 64 + ((unsigned char)u[3] - 128);
    default:
      throw std::runtime_error("should never happen");
  }
}

const std::vector<std::string> &PinYin::get_pinyin(const std::string &chinese) { return pinyin[codepoint(chinese)]; }

std::vector<std::string> PinYin::_split_pinyin(const std::string &input, int begin, int end) {
  if (begin >= end) {
    return empty_vector;
  }
  if (begin == end - 1) {
    return {input.substr(begin, end - begin)};
  }
  std::vector<std::string> result;
  std::string full = input.substr(begin, end - begin);
  if (pinyin_prefix.find(full) != pinyin_prefix.end() || pinyin_valid.find(full) != pinyin_valid.end()) {
    result.push_back(full);
  }
  int start = begin + 1;
  while (start < end) {
    std::string first = input.substr(begin, start - begin);
    if (pinyin_valid.find(first) == pinyin_valid.end()) {
      ++start;
      continue;
    }
    std::vector<std::string> tmp = _split_pinyin(input, start, end);
    for (const auto &s : tmp) {
      result.push_back(first + "+" + s);
    }
    ++start;
  }
  return result;
}

std::set<std::string> PinYin::split_pinyin(const std::string &input) {
  int slen = (int)input.size();
  const int max_length = 20;
  if (slen > max_length || slen <= 1) {
    return {input};
  }

  std::string spacedInput;
  for (auto c : input) {
    spacedInput.push_back('+');
    spacedInput.push_back(c);
  }
  spacedInput = spacedInput.substr(1, spacedInput.size());

  if (slen > 2) {
    std::vector<std::string> tmp = _split_pinyin(input, 0, slen);
    std::set<std::string> s(tmp.begin(), tmp.end());
    s.insert(spacedInput);
    s.insert(input);
    return s;
  }
  return {input, spacedInput};
}

}  // namespace simple_tokenizer
