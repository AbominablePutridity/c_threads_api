#ifndef POST_THEME_H
#define POST_THEME_H

struct PostTheme
{
    int id;
    int post_id;
    int theme_id;
};

#include "../db/db_connection.h"

int init_table_post_theme(sqlite3* db);
int create_post_theme(sqlite3* db, struct PostTheme* post_theme);
int delete_post_theme(sqlite3* db, struct PostTheme* post_theme);

#endif