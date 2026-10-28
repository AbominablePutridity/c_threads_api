#ifndef POST_H
#define POST_H

#include<time.h>

struct Post
{
	int	   id;
	char   header[150];
	char*  description;
	time_t created_at;
	int    id_user;	
};

#include "../db/db_connection.h"

int init_table_post(sqlite3* db);
int get_posts_by_filter(sqlite3* db, char* search_field, int id_theme, struct Post out[], int limit, int offset);
struct Post* find_concrete_post_by_id(sqlite3* db, int id, struct Post* out);

#endif
