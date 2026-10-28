#include<stdio.h>
#include<string.h>
#include "../libs/sqlite3/sqlite3.h"

#include "user.h"

#include "../db/db_connection.h"

/*
* @brief создаем таблицу пользователей если у нас ее не существует
* @param db - экземпляр подключения
*/
int init_table_user(sqlite3* db)
{
    sqlite3_stmt* stmt = create_query(
        db,
        "CREATE TABLE IF NOT EXISTS User("
        "   Id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   Login     VARCHAR(64) UNIQUE,"
        "   Password  VARCHAR(16),"
        "   CreatedAt INTEGER NOT NULL"
        ")"
    );

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int rc = sqlite3_step(stmt); // выполняет запрос
    sqlite3_finalize(stmt); // полностью освобождает память от запроса (после выполнения)

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "init_table: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    return 0;    
}

/*
* @brief Поиск пользователя по логину
* @param db экземпляр подключения
* @param login логин по которому ищем пользователя
* @param out результат поиска
*/
int user_get_by_login(sqlite3* db, const char* login, struct User* out)
{
    if (!db || !login || !out) return -1;

    const char* sql =
        "SELECT Id, Login, Password, CreatedAt FROM User WHERE Login = ?;";

    sqlite3_stmt* stmt = create_query(db, sql);
    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, login, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    int result;

    //читаем результат и пишем его в User* out
    if (rc == SQLITE_ROW) {
        // читаем нулевую колонку с id
        out->id = (int)sqlite3_column_int64(stmt, 0);

        // читаем первую колонку
        const char* p = (const char*)sqlite3_column_text(stmt, 1);
        strncpy(out->login, p ? p : "", sizeof out->login - 1);
        out->login[sizeof out->login - 1] = '\0';

        // читаем вторую колонку (перезаписывая прошлую переменную результата)
        p = (const char*)sqlite3_column_text(stmt, 2);
        strncpy(out->password, p ? p : "", sizeof out->password - 1);
        out->password[sizeof out->password - 1] = '\0';

        // читаем вторую колонку (перезаписывая прошлую переменную результата)
        out->created_at = (long)sqlite3_column_int64(stmt, 3);
        result = 0;
    } else if (rc == SQLITE_DONE) {
        result = 1;
    } else {
        fprintf(stderr, "user_get_by_login: %s\n", sqlite3_errmsg(db));
        result = -1;
    }

    sqlite3_finalize(stmt); // очищаем результаты запроса
    return result;
}
