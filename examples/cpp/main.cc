#include <sqlite3.h>

#include <iostream>

using namespace std;

// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  // int argc: holds the number of results
  // (array) azColName: holds each column returned
  // (array) argv: holds each value
  for (int i = 0; i < argc; i++) {
    // Show column name, value, and newline
    cout << azColName[i] << ": " << argv[i] << endl;
  }
  // Insert a newline
  cout << endl;
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

  // load simple
  rc = sqlite3_enable_load_extension(db, 1);
  handle_rc(db, rc);
  rc = sqlite3_load_extension(db, "libsimple", NULL, NULL);
  handle_rc(db, rc);

  // create fts table
  string sql = "CREATE VIRTUAL TABLE t1 USING fts5(x, tokenize = 'simple')";
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

  // match the string
  sql = "select simple_highlight(t1, 0, '[', ']') as matched_row from t1 where x match simple_query('zhoujiel')";
  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
  handle_rc(db, rc);

  // Close the connection
  sqlite3_close(db);

  return (0);
}
