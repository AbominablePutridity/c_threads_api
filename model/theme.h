#ifndef THEME_H
#define THEME_H

struct Theme
{
	int id;
	char name[150];	
};

#include "../db/db_connection.h"

int init_table_theme(sqlite3* db);
int get_themes_by_filter(sqlite3* db, int id, char name[150], struct Theme out[], int page, int limit);
struct Theme find_theme_by_id(sqlite3* db, int id);

#endif
