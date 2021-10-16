# encoding=utf-8
import sqlite3
from sqlite3.dbapi2 import Cursor

# need to run with python3
# example modified from https://github.com/wangfenjin/simple/issues/56

# https://www.sqlite.org/fts5.html#external_content_tables
TABLE_SQL = "CREATE TABLE IF NOT EXISTS chat (_id INTEGER PRIMARY KEY AUTOINCREMENT,sender_nickname TEXT,data BLOB)"
SEARCH_TABLE_SQL = "CREATE VIRTUAL TABLE IF NOT EXISTS chat_fts USING fts5(sender_nickname,data, content=chat, content_rowid=_id, tokenize='simple');"
# In order to use this command to delete a row, the text value 'delete' must be inserted into the special column with the same name as the table. The rowid of the row to delete is inserted into the rowid column.
TRIGGER_SQL = """
CREATE TRIGGER IF NOT EXISTS chat_fts_i AFTER INSERT ON chat BEGIN
  INSERT INTO chat_fts(rowid, sender_nickname, data) VALUES (new._id, new.sender_nickname, new.data);
END;
CREATE TRIGGER IF NOT EXISTS chat_fts_d AFTER DELETE ON chat BEGIN
  INSERT INTO chat_fts(chat_fts, rowid, sender_nickname, data) VALUES('delete', old._id, old.sender_nickname, old.data);
END;
CREATE TRIGGER IF NOT EXISTS chat_fts_u AFTER UPDATE ON chat BEGIN
  INSERT INTO chat_fts(chat_fts, rowid, sender_nickname, data) VALUES('delete', old._id, old.sender_nickname, old.data);
  INSERT INTO chat_fts(rowid, sender_nickname, data) VALUES (new._id, new.sender_nickname, new.data);
END;
"""
INSERT_SQL = """
INSERT INTO chat(sender_nickname, data) VALUES(?, ?);
"""
SEARCH_SQL = "SELECT rowid,sender_nickname,data FROM chat_fts where chat_fts match ? ORDER BY rank;"

class ftsDB:
    # NOTE: you need to set the ext_path
    def __init__(self, path, ext_path) -> None:
        # create database file if not exist
        self.db = sqlite3.connect(path)
        # create table if table not exist
        self.init_db(ext_path)

    def __del__(self):
        if hasattr(self, 'db') and self.db:
            self.db.commit()
            self.db.close()
            del self.db

    def init_db(self, ext_path):
        self.db.enable_load_extension(True)
        self.db.load_extension(ext_path)
        cursor = self.db.cursor()
        cursor.execute(TABLE_SQL)
        cursor.execute(SEARCH_TABLE_SQL)
        cursor.executescript(TRIGGER_SQL)
        self.commit()
        self.cursor = cursor
    
    def search(self, query: str):
        return self.cursor.execute(SEARCH_SQL, query)

    def insert(self, *data):
        # insert a line of data
        self.cursor.execute(INSERT_SQL, data)

    def bulk_insert(self, datas):
        # insert bulk data
        self.cursor.executemany(INSERT_SQL, datas)
        
    def commit(self):
        self.db.commit()
    
    def process_journal(self, filename, offset=0):
        with open(filename, 'r', encoding='utf-8') as f:
            if offset >= 0:
                f.seek(offset)
            while True:
                try:
                    line = f.readline()
                    if not line:
                        break
                    line = eval(line)
                    self.insert(*line)
                except Exception as e:
                    print(repr(line))
                    print(f"========pos: {offset}======")
                    raise e
                finally:
                    self.commit()
                offset = f.tell()

if __name__ == '__main__':
    import os, sys

    assert(len(sys.argv) == 2)
    # sys.argv[1] should be the extension's path, `/path/to/libsimple`
    # without file extension such as so, dylib or dll
    db = ftsDB(':memory:', sys.argv[1])
    db.process_journal(os.path.dirname(os.path.abspath(__file__)) + "/journal.txt")
    matched_row_count = 0
    for r in db.search('不'):
        matched_row_count += 1
        print(r)
    assert(matched_row_count == 3)
    db.__del__()
