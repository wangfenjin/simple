![build](https://github.com/wangfenjin/simple/workflows/CI/badge.svg)


# Simple tokenizer

simple 是一个支持中文和拼音的 [sqlite3 fts5](https://www.sqlite.org/fts5.html) 拓展。它完整提供了 [微信移动端的全文检索多音字问题解决方案](https://cloud.tencent.com/developer/article/1198371) 一文中的方案四，非常简单和高效地支持中文及拼音的搜索。

实现相关介绍：https://www.wangfenjin.com/posts/simple-tokenizer/

## 用法

首先需要确认你用到的 sqlite 版本支持 fts5 拓展，确认方法是：
```sql
select fts5(?1);
```
然后就可以使用了，具体的例子可以参考 [example.sql](./example.sql)

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

### 代码
- `src/entry` 入口文件，注册 sqlite tokenizer 和函数
- `src/simple_tokenizer` 分词器实现
- `src/simple_highlight` 高亮函数，基于内置的高亮函数改的，让命中的相邻单词连续高亮
- `src/pinyin` 中文转拼音以及拼音拆 query 的实现

## TODO

- [ ] 加一些代码注释
- [x] 添加 CI/CD 
- [ ] 添加 Java 和 Swift 使用的例子，方便在移动端使用
- [x] 部分参数可配，比如拼音文件的路径(已经把文件打包到 so 中)
- [ ] 减少依赖，减小 so 的大小
- [ ] 给出性能数据
