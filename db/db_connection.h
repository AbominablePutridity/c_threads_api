#ifndef DB_H
#define DB_H

#define DB_PATH "D:\\Clang\\Proj1\\db\\app.db" // АБСОЛЮТНЫЙ ПУТЬ К БД - НУЖНО МЕНЯТЬ!

sqlite3*      db_init(const char *db_path);
sqlite3_stmt* create_query(sqlite3* db, const char *query);

#endif