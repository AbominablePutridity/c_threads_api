#ifndef POST_H
#define POST_H

#include<time.h>

struct PostData
{
	char*  description;
	time_t created_at;
};

struct Post
{
	int	   id;
	char   header[150];
	int    id_user;
	struct PostData* post_data;	
};

#include "../db/db_connection.h"

int init_table_post(sqlite3* db);
int get_posts_by_filter(sqlite3* db, char* search_field, int id_theme, struct Post out[], int page, int limit);
struct Post* find_concrete_post_by_id(sqlite3* db, int id, struct Post* out);

#endif
