#include <stdio.h>
#include "security/jwt_encode.h"

#include "libs/cJSON/cJSON.h"

#include "libs/sqlite3/sqlite3.h"

#include "libs/mongoose/mongoose.h"

// cmake --build build
//./build/Debug/my_app.exe

/*
rm -rf build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build
*/

// Создаем прототипф функций контроллеров
bool users_handle(struct mg_connection *c, struct mg_http_message *hm);


// Обработчик событий соединения. Вызывается Mongoose при каждом новом запросе.
static void ev_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) { // Получено новое HTTP-сообщение
        struct mg_http_message *hm = (struct mg_http_message *) ev_data; // Разобранный запрос

        //выполняем функции контроллеров
        if (users_handle(c, hm)) return;

        mg_http_reply(c, 404, "", "Not found\n");
    }
}

int main() {
    struct mg_mgr mgr;  // Менеджер событий Mongoose (хранит все соединения)
    mg_mgr_init(&mgr);  // Инициализация менеджера

    // Запускаем HTTP-сервер на порту 8000, все запросы обрабатывает ev_handler
    mg_http_listen(&mgr, "http://192.168.1.112:8087", ev_handler, NULL);

    // Бесконечный цикл обработки событий
    for (;;) {
        mg_mgr_poll(&mgr, 1000); // Ожидание событий до 1000 мс
    }

    return 0;
}