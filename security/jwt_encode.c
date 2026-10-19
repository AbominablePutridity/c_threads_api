// ============================================================
// main.c — точка входа программы.
// Здесь мы создаём (кодируем) JWT-токен с помощью l8w8jwt.
// ============================================================

#include "jwt_encode.h"

// Подключаем стандартный ввод/вывод (printf).
#include <stdio.h>

// Подключаем работу со строками (strlen).
#include <string.h>

// Главный заголовок библиотеки l8w8jwt для кодирования JWT.
// Путь "l8w8jwt/encode.h" работает, потому что CMake через
// target_link_libraries уже добавил нужную папку include.
#include "l8w8jwt/encode.h"

#include "l8w8jwt/decode.h" // для декодирования

const unsigned char* SECRET_KEY = "YourSuperSecretKey1337";

/**
 * @brief Создает JWT токен
 * @param user_login Строка для генерации в токен
 * @return возвращает токен если токен валиден, NULL - если нет
 */
char* generate_token(const char* user_login)
{
    // 1. Объявляем переменные для результата.

    // Сюда библиотека положит указатель на строку с готовым токеном.
    // Изначально значение мусорное — библиотека сама его перезапишет.
    char* jwt = NULL;

    // Сюда библиотека положит длину строки токена (без '\0').
    size_t jwt_length = 0;

    // 2. Готовим структуру с параметрами кодирования.

    // Структура, в которую мы сложим все настройки токена:
    // алгоритм, claims (sub/iss/aud), время жизни, ключ и т.д.
    struct l8w8jwt_encoding_params params;

    // ВАЖНО: сначала инициализируем структуру.
    // Функция заполняет все поля значениями по умолчанию (нулями),
    // чтобы мы случайно не передали мусор из стека.
    l8w8jwt_encoding_params_init(&params);

    // 3. Заполняем параметры токена.

    // Алгоритм подписи. HS512 = HMAC + SHA-512 (симметричный).
    // Ключ для подписи и проверки — один и тот же (secret_key).
    params.alg = L8W8JWT_ALG_HS512;

    // sub (Subject) — «о ком» токен. Обычно ID пользователя.
    params.sub = user_login;

    // iat (Issued At) — время выпуска токена.
    // l8w8jwt_time(NULL) возвращает текущее Unix-время (секунды с 1970).
    params.iat = l8w8jwt_time(NULL);

    // exp (Expiration Time) — когда токен истечёт.
    // Берём текущее время и прибавляем 600 секунд (10 минут).
    params.exp = l8w8jwt_time(NULL) + 600;

    // 4. Задаём секретный ключ для подписи.

    // Сам ключ. Для HS512 это просто набор байт (строка).
    // В реальном проекте ключ берут из переменной окружения
    // или конфига, а НЕ хардкодят в исходнике.
    params.secret_key = SECRET_KEY;

    // Длина ключа в байтах. strlen возвращает число символов
    // без учёта завершающего '\0'.
    params.secret_key_length = strlen((char*)params.secret_key);

    // 5. Указываем, куда положить результат.

    // Передаём АДРЕС переменной jwt, чтобы функция могла
    // записать в неё указатель на строку токена.
    params.out = &jwt;

    // Аналогично — адрес переменной длины.
    params.out_length = &jwt_length;

    // 6. Вызываем кодирование.

    // Функция собирает header + payload, подписывает их ключом
    // и формирует итоговую строку "xxxxx.yyyyy.zzzzz".
    // Возвращает код результата (0 = успех).
    int r = l8w8jwt_encode(&params);

    // 7. Проверяем результат и печатаем токен.

    if (r == L8W8JWT_SUCCESS) {
        // Всё хорошо — выводим токен на экран.
        printf("JWT: %s\n", jwt);
    } else {
        // Что-то пошло не так — печатаем код ошибки.
        // Расшифровку кодов смотрите в l8w8jwt/retcodes.h
        printf("Ошибка кодирования: %d\n", r);
    }

    // 8. Освобождаем память.

    // Библиотека сама выделила память под строку токена (malloc).
    // Если её не освободить — утечка памяти.
    // Вызываем ВСЕГДА, независимо от результата кодирования.
    //l8w8jwt_free(jwt);


    // Возвращаем токен — программа завершилась успешно.
    return jwt;
}

/**
 * @brief Проверяет валидность JWT токена
 * @param token Строка с токеном
 * @return 1 если токен валиден, 0 если нет
 */
int validate_token(const char* token)
{
    // 1. Готовим структуру с параметрами для декодирования
    struct l8w8jwt_decoding_params params;
    l8w8jwt_decoding_params_init(&params);

    // 2. Указываем сам токен, который хотим проверить
    params.jwt = (char*)token;
    params.jwt_length = strlen(token);

    // 3. Указываем алгоритм (у нас был HS512)
    params.alg = L8W8JWT_ALG_HS512;

    // 4. Указываем ключ для проверки подписи
    // Для HS512 это тот же самый секретный ключ, что и при создании
    params.verification_key = (unsigned char*)SECRET_KEY;
    params.verification_key_length = strlen(SECRET_KEY);

    // 5. Настраиваем, что именно хотим проверить
    // (здесь можно оставить NULL, если не хотим проверять конкретные значения)
    params.validate_iss = NULL;
    params.validate_sub = NULL;
    params.validate_aud = NULL;

    // 6. Включаем проверку срока действия (exp)
    // Это важно: библиотека проверит, не истёк ли токен
    params.validate_exp = 1;
    params.exp_tolerance_seconds = 60; // допустимое расхождение во времени (секунды)

    // 7. Включаем проверку времени выпуска (iat)
    params.validate_iat = 1;
    params.iat_tolerance_seconds = 60;

    // 8. Сюда библиотека положит результат проверки
    enum l8w8jwt_validation_result validation_result;

    // 9. Вызываем функцию декодирования и проверки
    // Последние два NULL означают: "мне не нужны декодированные claims"
    int decode_result = l8w8jwt_decode(&params, &validation_result, NULL, NULL);

    // 10. Анализируем результат
    // decode_result — успешно ли распарсили токен (синтаксически)
    // validation_result — прошёл ли токен все проверки
    if (decode_result == L8W8JWT_SUCCESS && validation_result == L8W8JWT_VALID)
    {
        printf("Токен валиден!\n");
        return 1;
    }
    else
    {
        printf("Токен невалиден! Код ошибки: %d\n", validation_result);
        
        // Можно распечатать конкретную причину
        if (validation_result & L8W8JWT_EXP_FAILURE)
            printf("  Причина: токен истёк\n");
        if (validation_result & L8W8JWT_SIGNATURE_VERIFICATION_FAILURE)
            printf("  Причина: подпись не совпадает (токен подделан)\n");
        if (validation_result & L8W8JWT_IAT_FAILURE)
            printf("  Причина: токен ещё не выпущен\n");
        
        return 0;
    }
}

int jwt_usability_example(void)
{
    char* token = generate_token("gordon");

    if(token == NULL) {
        return 0;    
    }

    int ok = validate_token(token);
    printf("validation is - %d", ok);

    l8w8jwt_free(token);

    return ok;
}