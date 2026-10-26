#include "../libs/mongoose/mongoose.h"
#include "../service/user_service.h"
#include<string.h>
#include <stdlib.h>

// Вернёт true, если это был ЕГО запрос и он ответил.
bool users_handle(struct mg_connection *c, struct mg_http_message *hm) {
    if (mg_match(hm->uri, mg_str("/users"), NULL)) {

        // Извлекаем строки по JSON-путям $.login и $.password
        // hm->body — это структура типа struct mg_str, содержащая тело запроса
        char* login = mg_json_get_str(hm->body, "$.login");
        char* password = mg_json_get_str(hm->body, "$.password");

         // Проверяем, что JSON был валидным и поля успешно распарсились
        if (login != NULL && password != NULL) {
            char* result = auth(login, password);
            mg_http_reply(c, 200, "Content-Type: text/plain; charset=utf-8\r\n", result);

            free(result);
        } else {
            mg_http_reply(c, 400, "Content-Type: application/json\r\n", "{\"error\":\"Invalid JSON\"}\n");
        }

        //Обязательно освобождаем память, которую выделила Mongoose под login и password!
        if (login != NULL) free(login);
        if (password != NULL) free(password);

        return true;   // я ответил, дальше не надо
    }
    return false;      // это не моё, пусть попробует кто-то другой
}