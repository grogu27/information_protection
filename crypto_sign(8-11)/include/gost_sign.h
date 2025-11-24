#ifndef GOST_SIGN_H
#define GOST_SIGN_H

#include <string>

// Функция для подписи файла по ГОСТ Р 34.10-94
void gost_sign_and_verify_file(const std::string& filename);

#endif // GOST_SIGN_H