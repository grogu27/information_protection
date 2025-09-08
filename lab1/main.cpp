#include "crypto_lib.h"
#include <iostream>
#include <limits>

void demonstrate_large_numbers() {
    std::cout << "=== Демонстрация работы с большими числами ===" << std::endl;
    
    // Вычисляем 10! = 3628800
    long long factorial_10 = 1;
    for (int i = 1; i <= 10; i++) {
        factorial_10 *= i;
    }
    std::cout << "10! = " << factorial_10 << std::endl;
    
    CryptoLibrary crypto;
    
    // Быстрое возведение в степень по модулю
    long long base = 12345;
    long long exponent = 67890;
    long long modulus = 987654321;
    long long result = crypto.mod_pow(base, exponent, modulus);
    std::cout << "\n" << base << "^" << exponent << " mod " << modulus << " = " << result << std::endl;
    
    // Тест простоты Ферма
    long long test_number = 982451653; // Простое число
    bool is_prime = crypto.fermat_primality_test(test_number, 20);
    std::cout << "\nЧисло " << test_number << " " << (is_prime ? "простое" : "составное") << std::endl;
    
    // Обобщенный алгоритм Евклида
    long long a = 123456, b = 987654;
    auto egcd_result = crypto.extended_gcd(a, b);
    std::cout << "\nНОД(" << a << ", " << b << ") = " << egcd_result.gcd << std::endl;
    std::cout << "Коэффициенты Безу: x = " << egcd_result.x << ", y = " << egcd_result.y << std::endl;
    std::cout << "Проверка: " << a << "*" << egcd_result.x << " + " << b << "*" << egcd_result.y 
              << " = " << (a * egcd_result.x + b * egcd_result.y) << std::endl;
}

void interactive_menu() {
    CryptoLibrary crypto;
    int choice;
    
    while (true) {
        std::cout << "\n=== Криптографическая библиотека ===" << std::endl;
        std::cout << "1. Быстрое возведение в степень по модулю" << std::endl;
        std::cout << "2. Тест простоты Ферма" << std::endl;
        std::cout << "3. Обобщенный алгоритм Евклида" << std::endl;
        std::cout << "4. Генерация простых чисел" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "Выберите действие: ";
        
        std::cin >> choice;
        
        if (choice == 1) {
            std::cout << "\n--- Быстрое возведение в степень по модулю ---" << std::endl;
            std::cout << "1. Ввод с клавиатуры" << std::endl;
            std::cout << "2. Автоматическая генерация" << std::endl;
            std::cout << "Ваш выбор: ";
            
            int input_choice;
            std::cin >> input_choice;
            
            long long base, exponent, modulus;
            if (input_choice == 1) {
                std::cout << "Введите основание (a): ";
                std::cin >> base;
                std::cout << "Введите степень (b): ";
                std::cin >> exponent;
                std::cout << "Введите модуль (p): ";
                std::cin >> modulus;
            } else {
                base = crypto.generate_random_number(1000, 10000);
                exponent = crypto.generate_random_number(10, 100);
                modulus = crypto.generate_random_number(100000, 1000000);
                std::cout << "Сгенерированные значения: a=" << base 
                         << ", b=" << exponent << ", p=" << modulus << std::endl;
            }
            
            long long result = crypto.mod_pow(base, exponent, modulus);
            std::cout << "Результат: " << base << "^" << exponent << " mod " << modulus 
                      << " = " << result << std::endl;
        }
        else if (choice == 2) {
            std::cout << "\n--- Тест простоты Ферма ---" << std::endl;
            std::cout << "1. Ввод с клавиатуры" << std::endl;
            std::cout << "2. Автоматическая генерация" << std::endl;
            std::cout << "Ваш выбор: ";
            
            int input_choice;
            std::cin >> input_choice;
            
            long long n;
            if (input_choice == 1) {
                std::cout << "Введите число для проверки: ";
                std::cin >> n;
            } else {
                n = crypto.generate_random_number(100000, 1000000);
                std::cout << "Сгенерированное число: " << n << std::endl;
            }
            
            bool is_prime = crypto.fermat_primality_test(n, 10);
            std::cout << "Число " << n << " " << (is_prime ? "вероятно простое" : "составное") << std::endl;
        }
        else if (choice == 3) {
            std::cout << "\n--- Обобщенный алгоритм Евклида ---" << std::endl;
            std::cout << "1. Ввод с клавиатуры" << std::endl;
            std::cout << "2. Автоматическая генерация" << std::endl;
            std::cout << "3. Автоматическая генерация простых чисел" << std::endl;
            std::cout << "Ваш выбор: ";
            
            int input_choice;
            std::cin >> input_choice;
            
            long long a, b;
            if (input_choice == 1) {
                auto pair = crypto.input_from_keyboard();
                a = pair.first;
                b = pair.second;
            } else if (input_choice == 2) {
                auto pair = crypto.auto_generate_numbers();
                a = pair.first;
                b = pair.second;
                std::cout << "Сгенерированные числа: a=" << a << ", b=" << b << std::endl;
            } else {
                auto pair = crypto.auto_generate_primes();
                a = pair.first;
                b = pair.second;
                std::cout << "Сгенерированные простые числа: a=" << a << ", b=" << b << std::endl;
            }
            
            auto result = crypto.extended_gcd(a, b);
            std::cout << "НОД(" << a << ", " << b << ") = " << result.gcd << std::endl;
            std::cout << "Коэффициенты: x=" << result.x << ", y=" << result.y << std::endl;
            std::cout << "Проверка: " << a << "*" << result.x << " + " << b << "*" << result.y 
                      << " = " << (a * result.x + b * result.y) << std::endl;
        }
        else if (choice == 4) {
            std::cout << "\n--- Генерация простых чисел ---" << std::endl;
            long long min_val, max_val;
            std::cout << "Введите минимальное значение: ";
            std::cin >> min_val;
            std::cout << "Введите максимальное значение: ";
            std::cin >> max_val;
            
            long long prime = crypto.generate_prime(min_val, max_val);
            std::cout << "Сгенерированное простое число: " << prime << std::endl;
        }
        else if (choice == 0) {
            std::cout << "До свидания!" << std::endl;
            break;
        }
        else {
            std::cout << "Неверный выбор!" << std::endl;
        }
    }
}

int main() {
    // Демонстрация работы с большими числами
    demonstrate_large_numbers();
    
    std::cout << "\n" << std::string(50, '=') << "\n" << std::endl;
    
    // Интерактивное меню
    interactive_menu();
    
    return 0;
}