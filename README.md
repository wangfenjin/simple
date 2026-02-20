[![Downloads](https://img.shields.io/github/downloads/wangfenjin/simple/total)](https://img.shields.io/github/downloads/wangfenjin/simple/total)
[![build](https://github.com/wangfenjin/simple/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/wangfenjin/simple/actions/workflows/main.yml)
[![codecov](https://codecov.io/gh/wangfenjin/simple/branch/master/graph/badge.svg?token=8SHLFZ3RB4)](https://codecov.io/gh/wangfenjin/simple)
[![CodeFactor](https://www.codefactor.io/repository/github/wangfenjin/simple/badge)](https://www.codefactor.io/repository/github/wangfenjin/simple)
[![License: MIT](https://img.shields.io/badge/Dual_License-MIT_or_GPL_v3_later-blue.svg)](https://github.com/wangfenjin/simple/blob/master/LICENSE)

# Simple tokenizer

simple 是一个支持中文和拼音的 [sqlite3 fts5](https://www.sqlite.org/fts5.html) 拓展。它完整提供了 [微信移动端的全文检索多音字问题解决方案](https://cloud.tencent.com/developer/article/1198371) 一文中的方案四，非常简单和高效地支持中文及拼音的搜索。

实现相关介绍：https://www.wangfenjin.com/posts/simple-tokenizer/

在此基础上，我们还支持通过 [cppjieba](https://github.com/yanyiwu/cppjieba) 实现更精准的词组匹配，介绍文章见 https://www.wangfenjin.com/posts/simple-jieba-tokenizer/

## 用法

### 代码使用

* 下载已经编译好的插件：https://github.com/wangfenjin/simple/releases 参考 examples 目录，目前已经有 c++, python, go 和 node-sqlite3 的例子。
* iOS可以参考:
  - [#73](https://github.com/wangfenjin/simple/pull/73)
  - [@hxicoder](https://github.com/hxicoder) 提供的 [demo](https://github.com/hxicoder/DBDemo)
  - [@pipi32167](https://github.com/pipi32167)提供的[demo](https://github.com/pipi32167/SQLiteSwiftDemo)
* 在 Rust 中使用的例子 https://github.com/wangfenjin/simple/issues/89 https://github.com/fundon/tiny-docs-se
* Java 例子 https://github.com/wangfenjin/sqlite-java-connect
* C# 例子 https://github.com/dudylan/SqliteCheck/
* Rust 例子 https://github.com/xuxiaocheng0201/libsimple/
* Android Flutter 的例子 https://github.com/SageMik/sqlite3_simple

### 命令行使用

首先需要确认你用到的 sqlite 版本支持 fts5 拓展，确认方法是：
```sql
select fts5(?1);
```
然后就可以使用了，具体的例子可以参考 [example.sql](./example.sql) 和 [cpp](https://github.com/wangfenjin/simple/blob/master/examples/cpp/main.cc) 

```
$ ./sqlite3
SQLite version 3.32.3 2020-06-18 14:00:33
Enter ".help" for usage hints.
Connected to a transient in-memory database.
Use ".open FILENAME" to reopen on a persistent database.
sqlite> .load libsimple
sqlite> CREATE VIRTUAL TABLE t1 USING fts5(text, tokenize = 'simple');
sqlite> INSERT INTO t1 VALUES ('中华人民共和国国歌');
sqlite> select simple_highlight(t1, 0, '[', ']') as text from t1 where text match simple_query('中华国歌');
[中华]人民共和[国国歌]
sqlite> select simple_highlight(t1, 0, '[', ']') as text from t1 where text match jieba_query('中华国歌');
[中华]人民共和国[国歌]
sqlite> select simple_highlight(t1, 0, '[', ']') as text from t1 where text match simple_query('中华人民共和国');
[中华人民共和国国]歌
sqlite> select simple_highlight(t1, 0, '[', ']') as text from t1 where text match jieba_query('中华人民共和国');
[中华人民共和国]国歌
```

## 功能

1. simple tokenizer 支持中文和拼音的分词，并且可通过开关控制是否需要支持拼音
2. simple_query() 函数实现自动组装 match query 的功能，用户不用学习 fts5 query 的语法
3. simple_highlight() 实现连续高亮 match 的词汇，与 sqlite 自带的 highlight 类似，但是 simple_highlight 实现了连续 match 的词汇分到同一组的逻辑，理论上用户更需要这样
4. simple_highlight_pos() 实现返回 match 的词汇位置，用户可以自行决定怎么使用
5. simple_snippet() 实现截取 match 片段的功能，与 sqlite 自带的 snippet 功能类似，同样是增强连续 match 的词汇分到同一组的逻辑
6. jieba_query() 实现jieba分词的效果，在索引不变的情况下，可以实现更精准的匹配。可以通过 `-DSIMPLE_WITH_JIEBA=OFF ` 关掉结巴分词的功能 [#35](https://github.com/wangfenjin/simple/pull/35)
7. jieba_dict() 指定 dict 的目录，只需要调用一次，需要在调用 jieba_query() 之前指定。
8. pinyin_dict() 支持指定自定义的 `pinyin.txt` 文件路径。调用成功后会立即切换拼音映射；如果文件格式不正确，会返回错误并保持当前映射不变。

### 自定义 pinyin.txt

默认会使用内置在 so 中的 `contrib/pinyin.txt`。如果希望使用自己的拼音表，可以在查询前调用：

```sql
select pinyin_dict('/path/to/pinyin.txt');
```

`pinyin.txt` 每行格式与默认文件一致，例如：

```text
U+3007: líng,yuán,xīng
U+3007: líng,yuán,xīng # 行尾注释也支持（前面需要空格）
```

注意：
- 建议在建索引和查询前先调用一次 `pinyin_dict()`。
- 如果替换了拼音映射，已有索引中的拼音 token 不会自动重建，需要按你的业务策略重建索引。

## 开发

### 编译相关

使用支持 c++14 以上的编译器编译，直接在根目录 ./build-and-run 就会编译所有需要的文件并运行测试。编译输出见 output 目录

也可以手动 cmake:
```shell
mkdir build; cd build
cmake ..
make -j 12
make install
```

支持 iOS 编译：
```
./build-ios.sh
```

支持 ohos 编译：

从 [官方发布渠道](https://gitcode.com/openharmony/docs/blob/master/en/release-notes/OpenHarmony-v5.0.1-release.md#acquiring-source-code-from-mirrors)下载适用于目标平台的SDK.
```
tar -zxvf ohos-sdk.tar.gz
cd $OHOS_SDK/Linux
for i in *.zip;do unzip ${i};done
```
开始编译
```
./build-ohos.sh
```

### 代码
- `src/entry` 入口文件，注册 sqlite tokenizer 和函数
- `src/simple_tokenizer` 分词器实现
- `src/simple_highlight` 高亮函数，基于内置的高亮函数改的，让命中的相邻单词连续高亮
- `src/pinyin` 中文转拼音以及拼音拆 query 的实现

## TODO

- [x] 添加 CI/CD 
- [x] 添加使用的例子，参见 [cpp](https://github.com/wangfenjin/simple/blob/master/examples/cpp/main.cc) [python3](https://github.com/wangfenjin/simple/blob/master/examples/python3/db_connector.py)
- [x] 部分参数可配，比如拼音文件的路径(已经把文件打包到 so 中)
- [x] 减少依赖，减小 so 的大小
- [x] 给出性能数据：加载扩展时间2ms内；第一次使用拼音功能需要加载拼音文件，大概 500ms；第一次使用结巴分词功能需要加载结巴分词文件，大概 4s。

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=wangfenjin/simple&type=Date)](https://www.star-history.com/#wangfenjin/simple&Date)
