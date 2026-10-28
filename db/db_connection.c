#include<stdio.h>
#include "../libs/sqlite3/sqlite3.h"

static sqlite3* g_db = NULL;

// singletoon для создания соединения
sqlite3* db_init(const char *db_path) {
	if (g_db) return g_db; // если существует - возвращаем текущее соединение
    
    // 1. Открываем (или создаём) базу данных
    int rc = sqlite3_open(db_path, &g_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Не удалось открыть БД: %s\n", sqlite3_errmsg(g_db));
        sqlite3_close(g_db);
        g_db = NULL;
        return NULL;
    }

    return g_db;
}

// для закрытия соединения
void db_shutdown(void) {
    if (g_db) {
        sqlite3_close(g_db);
        g_db = NULL;
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
