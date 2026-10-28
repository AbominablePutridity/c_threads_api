#include<stdio.h>
#include<string.h>
#include "../libs/sqlite3/sqlite3.h"
#include "theme.h"
#include <stdbool.h>

/*
* @brief создаем таблицу тем если у нас ее не существует
* @param db - экземпляр подключения
*/
int init_table_theme(sqlite3* db)
{
    sqlite3_stmt* stmt = create_query(
        db,
        "CREATE TABLE IF NOT EXISTS Theme("
        "   Id   INTEGER PRIMARY KEY AUTOINCREMENT,"
        "   Name VARCHAR(64) UNIQUE"
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
int get_themes_by_filter(sqlite3* db, int id, char name[150], struct Theme* out, int limit, int offset);
struct Theme* find_theme_by_id(sqlite3* db, int id);
*/

int get_themes_by_filter(sqlite3* db, int id, char name[150], struct Theme out[], int limit, int offset)
{
	// 1. гедерация sql и проверка фильтров
	char sql[512] =
	        "SELECT * FROM Theme ";

	bool is_and = false;
	if (id != 0) {

		if (is_and) // если НЕ первый элемент - то and, иначе where
			strcat(sql, "AND ");
		else
			strcat(sql, "WHERE ");

		strcat(sql, "id = :id ");

		is_and = true;
	}

	if (name != NULL) {

		if (is_and) // если НЕ первый элемент - то and, иначе where
			strcat(sql, "AND ");
		else
			strcat(sql, "WHERE ");
	
		strcat(sql, "name = :name ");
	
		is_and = true;
	}

	strcat(sql, "LIMIT :limit OFFSET :offset");

	// 2. установка значений в prepared_statement
	sqlite3_stmt* stmt = create_query(
		db,
		sql
	);

	if (id != 0) { // устанавливаем индекс для id если тот есть в строке
	    int idx = sqlite3_bind_parameter_index(stmt, ":id");
	    if (idx > 0) sqlite3_bind_int(stmt, idx, id);
	}
	
	if (name != NULL) { // устанавливаем индекс для name если та есть в строке
	    int idx = sqlite3_bind_parameter_index(stmt, ":name");
	    if (idx > 0) sqlite3_bind_text(stmt, idx, name, -1, SQLITE_TRANSIENT);
	}

	// устанавливаем индексы для пагинации
	int idx_limit = sqlite3_bind_parameter_index(stmt, ":limit");
	if (idx_limit > 0) sqlite3_bind_int(stmt, idx_limit, limit);
	
	int idx_offset = sqlite3_bind_parameter_index(stmt, ":offset");
	if (idx_offset > 0) sqlite3_bind_int(stmt, idx_offset, offset);
	
	// 4. Выполняем SELECT
	int count = 0;
	int rc;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && count < limit) {
	    // заполняем out[count] из колонок текущей строки
	 	out[count].id = sqlite3_column_int(stmt, 0);
	    const char* n = (const char*)sqlite3_column_text(stmt, 1);
	    if (n) {
	        strncpy(out[count].name, n, sizeof(out[count].name) - 1);
	        out[count].name[sizeof(out[count].name) - 1] = '\0';
	    } else {
	        out[count].name[0] = '\0';
	    }
	    count++;
	}
	
	if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
	    fprintf(stderr, "step: %s\n", sqlite3_errmsg(db));
	    sqlite3_finalize(stmt);
	    return -1;
	}
	
	// 6. Освобождаем
	sqlite3_finalize(stmt);
	return count;
}

struct Theme find_theme_by_id(sqlite3* db, int id)
{
	char sql[512] = "SELECT * FROM Theme WHERE id = :id";

	sqlite3_stmt* stmt = create_query(
		db,
		sql
	);

	int idx = sqlite3_bind_parameter_index(stmt, ":id");
	if (idx > 0) sqlite3_bind_int(stmt, idx, id);

	struct Theme theme = {0};

	int rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW) {
		theme.id = sqlite3_column_int(stmt, 0);
		const char* n = (const char*)sqlite3_column_text(stmt, 1);
		if (n) {
			strncpy(theme.name, n, sizeof(theme.name) - 1);
			theme.name[sizeof(theme.name) - 1] = '\0';
		} else {
			theme.name[0] = '\0';
		}
	} else if (rc != SQLITE_DONE) {
	    fprintf(stderr, "step: %s\n", sqlite3_errmsg(db));
	}

	// Освобождаем
	sqlite3_finalize(stmt);

	return theme;	
}
