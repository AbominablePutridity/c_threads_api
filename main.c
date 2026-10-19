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

int main(void)
{
    // 1. Создаём пустой объект (корень)
    cJSON *root = cJSON_CreateObject();

    // 2. Добавляем в него поля
    cJSON_AddStringToObject(root, "name", "luffy");
    cJSON_AddNumberToObject(root, "age", 19);
    cJSON_AddBoolToObject(root, "active", 1);

    // 3. Превращаем в строку
    char *json_str = cJSON_Print(root);   // с отступами
    // или cJSON_PrintUnformatted(root)   // без отступов, компактнее

    printf("%s\n", json_str);

    return jwt_usability_example();
}