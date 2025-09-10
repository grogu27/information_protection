#include "src/crypto.hpp"
#include <iostream>
#include <chrono>

int main() {
    long long a = 7;
    long long x = 100000000;  
    long long p = 1000000007; 

    std::cout << "Тест: a=" << a << ", x=" << x << ", p=" << p << "\n";

    // ==== Быстрое возведение в степень ====
    auto start_fast = std::chrono::high_resolution_clock::now();
    long long y_fast = mod_pow(a, x, p);
    auto end_fast = std::chrono::high_resolution_clock::now();
    auto duration_fast = std::chrono::duration_cast<std::chrono::milliseconds>(end_fast - start_fast).count();

    std::cout << "Быстрое возведение: y = " << y_fast 
              << " (время: " << duration_fast << " мс)\n";

    // ===== Обычное возведение в степень =====
    auto start_std2 = std::chrono::high_resolution_clock::now();
    long long y_std = 1;
    for (long long i = 0; i < x; i++) {
        y_std = (y_std * a) % p;
    }
    auto end_std2 = std::chrono::high_resolution_clock::now();
    auto duration_std = std::chrono::duration_cast<std::chrono::milliseconds>(end_std2 - start_std2).count();

    std::cout << "Обычное возведение: y = " << y_std
              << " (время: " << duration_std << " мс)\n";

    if (y_fast == y_std)
        std::cout << "Результаты совпадают\n";
    else
        std::cout << "Результаты не совпадают\n";

    return 0;
}
