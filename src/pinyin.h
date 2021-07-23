#ifndef PINYIN_H_
#define PINYIN_H_

#include <map>
#include <set>
#include <string>
#include <vector>

namespace simple_tokenizer {

class PinYin {
 private:
  std::map<int, std::vector<std::string> > pinyin;
  const std::vector<std::string> empty_vector;

  // clang-format off
  const std::map<std::string, char> tone_to_plain = {
      {"ā", 'a'}, {"á", 'a'}, {"ǎ", 'a'}, {"à", 'a'},
      {"ē", 'e'}, {"é", 'e'}, {"ě", 'e'}, {"è", 'e'},
      {"ế", 'e'}, {"ề", 'e'}, {"ê", 'e'},
      {"ō", 'o'}, {"ó", 'o'}, {"ǒ", 'o'}, {"ò", 'o'},
      {"ī", 'i'}, {"í", 'i'}, {"ǐ", 'i'}, {"ì", 'i'},
      {"ū", 'u'}, {"ú", 'u'}, {"ǔ", 'u'}, {"ù", 'u'},
      {"ǘ", 'u'}, {"ǚ", 'u'}, {"ǜ", 'u'}, {"ü", 'u'},
      {"ń", 'n'}, {"ň", 'n'}, {"ǹ", 'n'},
      {"ḿ", 'm'},
  };
  // 不是合法拼音，但是是前缀，只能出现在结尾。
  const std::set<std::string> pinyin_prefix = {
        "be","bia",
        "ch","cho","chon","chua","co","con","cua",
        "din","don","do","dua",
        "fe",
        "go","gon",
        "ho","hon",
        "len","lon","lua",
        "mia",
        "nia","no","non","nua",
        "pe","pia",
        "qio","qion","qua",
        "ra","ro","ron","rua",
        "sh","sho","so","son","sua",
        "ten","tia","tin","to","ton","tua",
        "we",
        "xio","xion","xua",
        "yon","yua",
        "zh","zho","zhon","zo","zon","zua",
    };

    // 合法拼音
  const std::set<std::string> pinyin_valid = {
        "a", "ai", "an", "ang", "ao",
        "ba", "bai", "ban", "bang", "bao",
              "bei", "ben", "beng", "bi", "bian", "biao", "bie", "bin", "bing", "bo", "bu",
        "ca", "cai", "can", "cang", "cao", "ce", "cen", "ceng", "cha", "chai",
              "chan", "chang", "chao", "che", "chen", "cheng", "chi", "chong", "chou", "chu",
              "chuai", "chuan", "chuang", "chui", "chun", "chuo", "ci", "cong", "cou", "cu",
              "cuan", "cui", "cun", "cuo",
        "da", "dai", "dan", "dang", "dao", "de", "dei",
              "den", "deng", "di", "dia", "dian", "diao", "die", "ding", "diu",
              "dong", "dou", "du", "duan", "dui", "dun", "duo",
        "e", "ei", "en", "eng", "er",
        "fa", "fan", "fang", "fei", "fen", "feng", "fo", "fou", "fu",
        "ga", "gai", "gan", "gang", "gao", "ge", "gei", "gen", "geng",
              "gong", "gou", "gu", "gua", "guai", "guan", "guang", "gui", "gun", "guo",
        "ha", "hai", "han", "hang", "hao", "he",
              "hei", "hen", "heng", "hong", "hou", "hu",
              "hua", "huai", "huan", "huang", "hui", "hun", "huo",
        // "i"=>[],
        "ji", "jia", "jian", "jiang", "qiao", "jiao", "jie", "jin", "jing", "jiong",
              "jiu", "ju", "juan", "jue", "jun","jv",
        "ka", "kai", "kan", "kang", "kao", "ke", "kei", "ken", "keng", "kong", "kou", "ku", "kua", "kuai",
              "kuan", "kuang", "kui", "kun", "kuo",
        "la", "lai", "lan", "lang", "lao",
              "le", "lei", "leng", "li", "lia", "lian", "liang", "liao", "lie", "lin",
              "ling", "liu", "long", "lo", "lou", "lu", "luan", "lue", "lun", "luo","lv",
        "ma", "mai", "man", "mang", "mao", "me", "mei", "men", "meng", "mi", "mian",
              "miao", "mie", "min", "ming", "miu", "mo", "mou", "mu",
        "na", "nai", "nan", "nang", "nao", "ne", "nei", "nen", "neng", "ni", "nian", "niang",
              "niao", "nie", "nin", "ning", "niu", "nong", "nou", "nu", "nuan", "nue", "nun", "nuo", "nv",
        "o", "ou",
        "pa", "pai", "pan", "pang", "pao", "pei", "pen",
              "peng", "pi", "pian", "piao", "pie", "pin", "ping", "po", "pou", "pu",
        "qi", "qia", "qian", "qiang", "qie", "qin", "qing", "qiong", "qiu", "qu",
              "quan", "que", "qun","qv",
        "ran", "rang", "rao", "re", "ren", "reng", "ri",
              "rong", "rou", "ru", "ruan", "rui", "run", "ruo",
        "sa", "sai", "san",
              "sang", "sao", "se", "sen", "seng", "sha", "shai", "shan", "shang", "shao",
              "she", "shei", "shen", "sheng", "shi", "shou", "shu", "shua", "shuai", "shuan", "shuang",
              "shui", "shun", "shuo", "si", "song", "sou", "su", "suan", "sui", "sun", "suo",
        "ta", "tai", "tan", "tang", "tao", "te", "tei", "teng", "ti", "tian",
              "tiao", "tie", "ting", "tong", "tou", "tu", "tuan", "tui", "tun", "tuo",
        // "u"=>[],
        // "v"=>[],
        "wa", "wai", "wan", "wang", "wei", "wen", "weng", "wo", "wu",
        "xi", "xia", "xian", "xiang", "xiao", "xie", "xin", "xing", "xiong", "xiu", "xu",
              "xuan", "xue", "xun","xv",
        "ya", "yan", "yang","yao", "ye", "yi", "yin", "ying",
              "yo", "yong", "you", "yu", "yuan", "yue", "yun",
        "za", "zai", "zan",
              "zang", "zao", "ze", "zei", "zen", "zeng", "zha", "zhai", "zhan", "zhang",
              "zhao", "zhe", "zhen", "zheng", "zhi", "zhong", "zhou", "zhu", "zhua", "zhuai",
              "zhuan", "zhuang", "zhui", "zhun", "zhuo", "zi", "zong", "zou", "zu", "zuan",
        "zui", "zun", "zuo",
    };
  // clang-format on
  std::set<std::string> to_plain(const std::string &input);
  std::map<int, std::vector<std::string> > build_pinyin_map();
  static int codepoint(const std::string &u);
  std::vector<std::string> _split_pinyin(const std::string &input, int begin, int end);

 public:
  const std::vector<std::string> &get_pinyin(const std::string &chinese);
  static int get_str_len(unsigned char byte);
  std::set<std::string> split_pinyin(const std::string &input);
  PinYin();
};

}  // namespace simple_tokenizer

#endif  // PINYIN_H_
