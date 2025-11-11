#include "../include/gost_sign.h"
#include "../include/utils.h"
#include "gost341094.h"
#include <iostream>

using namespace std;

void gost_sign_file(const std::string& filename) {
    cout << "=== GOST R 34.10-94 Signature ===" << endl;
    
    // Вычисляем хеш файла
    auto hash = sha256_file(filename);
    cout << "File: " << filename << " (SHA256 hash computed)" << endl;
    
    // Создаем объект ГОСТ и генерируем ключи
    GOST341094 gost;
    cout << "Generating keys..." << endl;
    gost.generate_keys();
    
    // Создаем подпись
    cout << "Creating signature..." << endl;
    auto signature = gost.sign(hash);
    
    // Сохраняем подпись в отдельный файл
    string sigFilename = filename + ".gost_sig";
    gost.save_signature(signature, sigFilename);
    
    cout << "✓ Signature created and saved to: " << sigFilename << endl;
}

void gost_verify_file(const std::string& filename) {
    // Вычисляем хеш файла
    auto hash = sha256_file(filename);
    
    // Загружаем подпись
    GOST341094 gost;
    string sigFilename = filename + ".gost_sig";
    auto signature = gost.load_signature(sigFilename);
    
    // Проверяем подпись
    cout << "Verifying signature..." << endl;
    bool isValid = gost.verify(hash, signature);
    
    if (isValid) {
        cout << "✓ GOST R 34.10-94 signature VERIFIED SUCCESSFULLY" << endl;
    } else {
        cout << "✗ GOST R 34.10-94 signature VERIFICATION FAILED" << endl;
        throw runtime_error("Signature verification failed");
    }
}