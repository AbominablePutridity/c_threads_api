#include "../libs/mongoose/mongoose.h"

// Вернёт true, если это был ЕГО запрос и он ответил.
bool users_handle(struct mg_connection *c, struct mg_http_message *hm) {
    if (mg_match(hm->uri, mg_str("/users"), NULL)) {
        mg_http_reply(c, 200, "Content-Type: text/plain; charset=utf-8\r\n", "Список пользователей\n");
        return true;   // я ответил, дальше не надо
    }
    return false;      // это не моё, пусть попробует кто-то другой
}