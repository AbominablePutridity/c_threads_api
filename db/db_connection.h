#ifndef DB_H
#define DB_H

#define DB_PATH "/app.db"

sqlite3*      db_init(const char *db_path);
sqlite3_stmt* create_query(sqlite3* db, const char *query);

#endif