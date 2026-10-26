#include <stdlib.h>
#include <string.h>

#include "../libs/sqlite3/sqlite3.h" 
#include "../security/jwt_encode.h"
#include "../db/db_connection.h"
#include "../model/user.h"
#include "../libs/cJSON/cJSON.h"

char* auth(char* login, char* password)
{
    sqlite3* db = db_init(DB_PATH);

    cJSON *json = cJSON_CreateObject(); // выделяет память в куче - после вызова необходим free()

    if (db != NULL) {
        struct User out; // структура куда выводится результат поиска

        if (user_get_by_login(db, login, &out) == 0) {
            if(strcmp(out.login, login) == 0
                && strcmp(out.password, password) == 0
            )
            {
                char* token = generate_token(login);

                cJSON_AddStringToObject(json, "token", token);

                if (token != NULL) {
                    free(token);
                }
            } else {
                cJSON_AddStringToObject(json, "token", "Ошибка: неправильно введен логин или пароль!");
            }
        } else {
            cJSON_AddStringToObject(json, "token", "Ошибка: пользователь не найден!");
        }

        char* result = cJSON_Print(json);

        // ОБЯЗАТЕЛЬНО освобождаем память
        cJSON_Delete(json); // Удаляет весь JSON-объект со всеми внутренностями
        sqlite3_close(db); // обязательно закрываем соединение с бд

        return result;
    } else {
        cJSON_AddStringToObject(json, "error", "db no connection");

        char* result = cJSON_Print(json);

        cJSON_Delete(json);

        return result;
    }
}