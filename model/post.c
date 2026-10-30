#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include "../libs/sqlite3/sqlite3.h"
#include "post.h"
#include <stdbool.h>


int init_table_post(sqlite3* db)
{
	sqlite3_stmt* stmt = create_query(
		db,
		"CREATE TABLE IF NOT EXISTS Post("
		"	Id	   		INTEGER PRIMARY KEY AUTOINCREMENT,"
		"	UserId 		INTEGER NOT NULL,"
		"	Header 		VARCHAR(150),"
		"	Description VARCHAR(10000),"
		"	CreatedAt 	INTEGER,"
		"	FOREIGN KEY (UserId) REFERENCES User(Id) ON DELETE CASCADE"
		");"
	);

	int rc = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "init_table_post: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}

int get_posts_by_filter(sqlite3* db, char* search_field, int id_theme, struct Post out[], int page, int limit)
{
	int offset = (page - 1) * limit;

	// формируем запрос sql
	char sql[512] =
			"SELECT id, header, id_user FROM Post ";

	bool is_and = false;
	if (id_theme != 0) {
		if (is_and)
			strcat(sql, "AND ");
		else
			strcat(sql, "WHERE ");

		strcat(sql, "Id = :id ");

		is_and = true;
	}
	
	if (search_field != NULL) {
		if (is_and)
			strcat(sql, "AND ");
		else
			strcat(sql, "WHERE ");

		strcat(sql, "Header LIKE :header ");

		is_and = true;
	}

	strcat(sql, " LIMIT :limit OFFSET :offset");

	// устанавливаем значения prepared statement
	sqlite3_stmt* stmt = create_query(
		db,
		sql	
	);

	if (id_theme != 0) {
		int idx = sqlite3_bind_parameter_index(stmt, ":id");
		if (idx) sqlite3_bind_int(stmt, idx, id_theme);
	}

	if (search_field != NULL) {
		int idx = sqlite3_bind_parameter_index(stmt, ":header");

		char pattern[256];
		snprintf(pattern, sizeof(pattern), "%%%s%%", search_field);
		
		if (idx) sqlite3_bind_text(stmt, idx, pattern, -1, SQLITE_TRANSIENT);
	}

	// устанавливаем индексы для пагинации
	int idx_limit = sqlite3_bind_parameter_index(stmt, ":limit");
	if (idx_limit > 0) sqlite3_bind_int(stmt, idx_limit, limit);
	
	int idx_offset = sqlite3_bind_parameter_index(stmt, ":offset");
	if (idx_offset > 0) sqlite3_bind_int(stmt, idx_offset, offset);

	// выполняем select
	int count = 0;
	while ((sqlite3_step(stmt)) == SQLITE_ROW && count < limit) {
		// заполняем out[count] из колонок текущей строки
		out[count].id = sqlite3_column_int(stmt, 0);

		strncpy(out[count].header, (const char*)sqlite3_column_text(stmt, 1), sizeof(out[count].header) - 1);

		out[count].id_user = sqlite3_column_int(stmt, 2);

		out[count].post_data = NULL;

		count++;
	}

	// освобождаем
	sqlite3_finalize(stmt);
	return count;
}

// обязательно очищать после получения!
struct Post* find_concrete_post_by_id(sqlite3* db, int id)
{
	char* sql = "SELECT Id, Header, UserId, Description, CreatedAt FROM Post WHERE id = :id";

	sqlite3_stmt* stmt = create_query(
		db,
		sql
	);

	int idx = sqlite3_bind_parameter_index(stmt, ":id");
	if (idx) sqlite3_bind_int(stmt, idx, id);

	struct Post* out = malloc(sizeof(struct Post));
	if (out != NULL) {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			out->id = sqlite3_column_int(stmt, 0);

			strncpy(out->header, (const char*)sqlite3_column_text(stmt, 1), sizeof(out->header) - 1);

			out->id_user = sqlite3_column_int(stmt, 2);

			out->post_data = malloc(sizeof(struct PostData));
			if (out->post_data != NULL) {
				out->post_data->description = malloc(10000 * sizeof(char));
				snprintf(out->post_data->description, 10000, "%s", (char*)sqlite3_column_text(stmt, 3));

				out->post_data->created_at = (time_t)sqlite3_column_int64(stmt, 4);
			}
		}
	}

	sqlite3_finalize(stmt);
	return out; 
}

// здесь post - указатель на кучу (выделять через malloc)
int create_post(sqlite3* db, struct Post* post)
{
	int result = -1;

	if (post->post_data != NULL) {
		char* sql =
			"INSERT INTO Post("
			"	UserId,"
			"	Header,"
			"	Description,"
			"	CreatedAt"
			") VALUES"
			"	(?, ?, ?, ?);";

		sqlite3_stmt* stmt = create_query(
			db,
			sql
		);

		//TODO: 1) сделать вставку по биндингам
		sqlite3_bind_int(stmt, 1, post->id_user);

		sqlite3_bind_text(stmt, 2, post->header, -1, SQLITE_TRANSIENT);

		
		sqlite3_bind_text(stmt, 3, post->post_data->description, -1, SQLITE_TRANSIENT);

		sqlite3_bind_int64(stmt, 4, (sqlite3_int64)post->post_data->created_at);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			result = 0;
		}

		sqlite3_finalize(stmt);
		
		//TODO: 2) сделать очистку передаваемой структуры (из кучи)
		free(post->post_data->description);
		free(post->post_data);
	} else {
		//выводим ошибку в консоль
		fprintf(stderr, "Ошибка: post_data не может быть NULL\n");
	}

	free(post);

	return result;
}
