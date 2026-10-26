#ifndef USER_H
#define USER_H

struct User {
    int   id;
    char  login[64];
    char  password[16];
    long  created_at;
};

#include "../db/db_connection.h"

int init_table(sqlite3* db);
int user_get_by_login(sqlite3* db, const char* login, struct User* out);

#endif