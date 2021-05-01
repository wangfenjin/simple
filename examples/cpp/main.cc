#include <sqlite3.h>

#include <chrono>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace std;
using Clock = std::chrono::system_clock;
using ms = std::chrono::duration<double, std::milli>;

// https://www.tutorialspoint.com/find-out-the-current-working-directory-in-c-cplusplus
string get_current_dir() {
  char buff[FILENAME_MAX];  // create string buffer to hold path
  GetCurrentDir(buff, FILENAME_MAX);
  string current_working_dir(buff);
  return current_working_dir;
}

// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  // int argc: holds the number of results
  // (array) azColName: holds each column returned
  // (array) argv: holds each value
  for (int i = 0; i < argc; i++) {
    // Show column name, value, and newline
    cout << azColName[i] << ": " << argv[i] << endl;
  }
  if (argc > 0) {
    cout << endl;
  }
  // Return successful
  return 0;
}

void handle_rc(sqlite3 *db, int rc) {
  if (rc != SQLITE_OK) {
    cout << "sqlite3 rc: " << rc << ", error: " << sqlite3_errmsg(db) << endl;
    exit(rc);
  }
}

int main() {
  // Pointer to SQLite connection
  sqlite3 *db;
  // Save any error messages
  char *zErrMsg = 0;

  // Save the connection result
  int rc = sqlite3_open(":memory:", &db);
  handle_rc(db, rc);

  auto before = Clock::now();
  // load simple
  rc = sqlite3_enable_load_extension(db, 1);
  handle_rc(db, rc);
  rc = sqlite3_load_extension(db, "libsimple", NULL, NULL);
  handle_rc(db, rc);
  ms load_extension = Clock::now() - before;
  std::cout << "It took " << load_extension.count() << "ms to load extension" << std::endl;

  // warm-up
  before = Clock::now();
  string sql = "select simple_query('pinyin')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
  ms pinyin = Clock::now() - before;
  std::cout << "It took " << pinyin.count() << "ms to init pinyin" << std::endl;
  before = Clock::now();
  sql = "select jieba_query('结巴')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
  ms warm_up = Clock::now() - before;
  std::cout << "It took " << warm_up.count() << "ms to init jieba" << std::endl;

  before = Clock::now();
  // create fts table
  sql = "CREATE VIRTUAL TABLE t1 USING fts5(x, tokenize = 'simple')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);

  // insert some data
  sql = R"V0G0N(
          insert into t1(x) values ('周杰伦 Jay Chou:最美的不是下雨天，是曾与你躲过雨的屋檐'),
                         ('I love China! 我爱中国!'),
                         ('@English &special _characters."''bacon-&and''-eggs%')
          )V0G0N";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);

  // case 1: match pinyin
  sql = "select simple_highlight(t1, 0, '[', ']') as matched_pinyin from t1 where x match simple_query('zhoujiel')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
  // case 2: match special chars
  sql =
      "select simple_highlight(t1, 0, '[', ']') as matched_no_single_quote_special_chars from t1 where x match "
      "simple_query('@\"._-&%')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
  // case 3: single quote, will match!
  sql =
      "select simple_highlight(t1, 0, '[', ']') as matched_simple_query_special_chars from t1 where x match "
      "simple_query('@\"._''-&%')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
#ifdef USE_JIEBA
  // set dict path manually
  string dict_path = get_current_dir() + "/dict";
  sql = "select jieba_dict('" + dict_path + "')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
  // case 4: jieba, no match
  sql = "select simple_highlight(t1, 0, '[', ']') as no_matched_jieba from t1 where x match jieba_query('国中')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
  // case 5: jieba, match
  sql = "select simple_highlight(t1, 0, '[', ']') as matched_jieba from t1 where x match jieba_query('中国')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);
#endif
  ms last_query = Clock::now() - before;
  std::cout << "It took " << last_query.count() << "ms for all query" << std::endl;

  // Close the connection
  sqlite3_close(db);

  return (0);
}
