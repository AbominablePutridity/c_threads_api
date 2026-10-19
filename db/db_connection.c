#include<stdio.h>
#include "../libs/sqlite3/sqlite3.h"

sqlite3* db_init(const char *db_path) {
    sqlite3* db = NULL;
    int rc;

    // 1. Открываем (или создаём) базу данных
    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Не удалось открыть БД: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    } else {
        return db;
    }
}

sqlite3_stmt* create_query(sqlite3* db, const char *query) {
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        
        return NULL;
    }

    return stmt;
}