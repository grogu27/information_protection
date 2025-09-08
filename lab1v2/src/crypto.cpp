#include "crypto.hpp"
#include <iostream>
#include <random>
#include <ctime>

long long mod_pow(long long a, long long x, long long p) {
    long long result = 1;
    a %= p;
    while (x > 0) {
        if (x & 1) result = (result * a) % p;
        a = (a * a) % p;
        x >>= 1;
    }
    return result;
}

// вер = 1 - 1/2^k              a^(n-1)modn = 1 Если p — простое число, a — целое число, не делящееся на p .    У a и p нет общих делителей
// 2^(9-1)mod9 = 256 mod 9 = 4  -> 9 составное
bool is_prime_fermat(long long n, int k) {  
    if (n < 4) return (n == 2 || n == 3);

    std::mt19937_64 gen(std::time(nullptr));
    std::uniform_int_distribution<long long> dist(2, n - 2);
    for (int i = 0; i < k; i++) {
        long long a = dist(gen);
        auto [gcd, x, y] = extended_gcd2(a, n);
        if (gcd != 1) return false;  //числа Кармайкла не пройдут
        if (mod_pow(a, n - 1, n) != 1) return false;
    }
    return true;
}

// long long extended_gcd(long long a, long long b, long long &x, long long &y) {
//     if (b == 0) {
//         x = 1; y = 0;
//         return a;
//     }
//     long long x1, y1;
//     long long gcd = extended_gcd(b, a % b, x1, y1);
//     x = y1;
//     y = x1 - (a / b) * y1;
//     return gcd;
// }

std::tuple<long long, long long, long long> extended_gcd2(long long a, long long b) {
    long long u1 = a, u2 = 1, u3 = 0;
    long long v1 = b, v2 = 0, v3 = 1;

    while (v1 != 0) {
        long long q = u1 / v1;
        long long t1 = u1 % v1;
        long long t2 = u2 - q * v2;
        long long t3 = u3 - q * v3;

        u1 = v1; u2 = v2; u3 = v3;
        v1 = t1; v2 = t2; v3 = t3;
    }
    return {u1, u2, u3};
}

// // Вариант 1: ввод чисел с клавиатуры
// std::pair<long long, long long> input_ab() {
//     long long a, b;
//     std::cout << "Введите a и b: ";
//     std::cin >> a >> b;
//     return {a, b};
// }

std::pair<long long, long long> random_ab() {
    std::mt19937_64 gen(std::time(nullptr));
    std::uniform_int_distribution<long long> dist(2, 1000000);
    return {dist(gen), dist(gen)};
}

std::pair<long long, long long> random_prime_ab() {
    std::mt19937_64 gen(std::time(nullptr));
    std::uniform_int_distribution<long long> dist(100, 100000);

    auto generate_prime = [&](int attempts = 100) {
        while (true) {
            long long candidate = dist(gen);
            if (is_prime_fermat(candidate, attempts))
                return candidate;
        }
    };

    long long a = generate_prime();
    long long b = generate_prime();
    return {a, b};
}
