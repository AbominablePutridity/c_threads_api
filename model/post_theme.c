#include<stdlib.h>
#include<string.h>
#include "../libs/sqlite3/sqlite3.h"
#include "post_theme.h"

int init_table_post_theme(sqlite3* db)
{
    sqlite3_stmt* stmt = create_query(
        db, 
        "CREATE TABLE IF NOT EXISTS PostTheme("
        "   Id      INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   IdPost  INTEGER NOT NULL,"
        "   IdTheme INTEGER NOT NULL"
        ");"
    );

    if (!stmt) {
        fprintf(stderr, "init_table_post_theme: пустой stmt!\n");
		return -1;
    }

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "init_table_post_theme: %s\n", sqlite3_errmsg(db));
		return -1;
    }

    return 0;
}

int create_post_theme(sqlite3* db, struct PostTheme* post_theme)
{
    char* sql = 
        "INSERT INTO PostTheme("
        "   IdPost,"
        "   IdTheme"
        ") VALUES (?, ?);";

    sqlite3_stmt* stmt = create_query(
        db,
        sql
    );

    if (!stmt) {
        fprintf(stderr, "create_post_theme: пустой stmt!");
        return -1;
    }

    sqlite3_bind_int(stmt, 1, post_theme->post_id);
    sqlite3_bind_int(stmt, 2, post_theme->theme_id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "create_post_theme: запрос не выполнен!");
        return -1;
    }

    return 0;
}

int delete_post_theme(sqlite3* db, struct PostTheme* post_theme)
{
    char* sql = 
        "DELETE FROM PostTheme WHERE "
        "IdPost  = :post_id AND "
        "IdTheme = :theme_id";

    sqlite3_stmt* stmt = create_query(
        db,
        sql
    );

    if (!stmt) {
        fprintf(stderr, "create_post_theme: пустой stmt!");
        return -1;
    }

    int post_id_idx = sqlite3_bind_parameter_index(stmt, ":post_id");
    int theme_id_idx = sqlite3_bind_parameter_index(stmt, ":theme_id");
    
    if (post_id_idx && theme_id_idx) {
        sqlite3_bind_int(stmt, post_id_idx, post_theme->post_id);
        sqlite3_bind_int(stmt, theme_id_idx, post_theme->theme_id);
    } else {
        sqlite3_finalize(stmt);
        fprintf(stderr, "create_post_theme: индексы запроса post_id_idx и theme_id_idx не найдены!");
        return -1;
    }

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "create_post_theme: запрос не выполнен!");
        return -1;
    }

    return 0;
}