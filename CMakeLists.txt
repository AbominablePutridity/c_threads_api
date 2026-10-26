cmake_minimum_required(VERSION 3.10)
project(MyApp C)

# Отключаем новое предупреждение GCC 15+, которое ломает сборку MbedTLS
add_compile_options(-Wno-unterminated-string-initialization)

# Подключаем библиотеки: CMake прочитает её собственный CMakeLists.txt
# и создаст цель с именем l8w8jwt - читаем библиотеку.
add_subdirectory(libs/l8w8jwt)

# Создаём наш исполняемый файл из НАШИХ исходников
add_executable(
    my_app main.c
    security/jwt_encode.c
    db/db_connection.c
    model/user.c
    controller/user_controller.c
    service/user_service.c

    libs/cJSON/cJSON.c
    libs/sqlite3/sqlite3.c
    libs/mongoose/mongoose.c
)

# Связываем наш файл с библиотекой
target_link_libraries(my_app PRIVATE l8w8jwt)

