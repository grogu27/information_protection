#include "crypto.hpp"
#include <iostream>
#include <random>
#include <ctime>
#include <map>



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
        if (gcd != 1) return false;  
        if (mod_pow(a, n - 1, n) != 1) return false;
    }
    return true;
}


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

/*  ~~~~~ LAB2 ~~~~  */
long long baby_step_giant_step(long long a, long long y, long long p) {
    if (a % p == 0) {
        if (y % p == 0) return 1; // или неопределено, но для простоты
        return -1;
    }

    long long m = (long long)ceil(sqrt(p - 1));
    
    // Baby Steps: вычисляем y * a^j mod p для j = 0..m-1
    std::map<long long, long long> baby;
    long long aj = y % p; // начальное значение: y * a^0 = y
    for (long long j = 0; j < m; ++j) {
        // Если значение уже есть — оставляем наименьшее j (не обязательно, но правильно)
        if (baby.find(aj) == baby.end()) {
            baby[aj] = j;
        }
        aj = (aj * a) % p;
    }

    // Giant Steps: вычисляем a^(m*i) mod p
    long long base = mod_pow(a, m, p); // a^m mod p
    if (base == 0) return -1;

    long long giant = 1; // будет a^(m*i)
    for (long long i = 1; i <= m; ++i) {
        giant = (giant * base) % p;
        auto it = baby.find(giant);
        if (it != baby.end()) {
            long long j = it->second;
            long long x = i * m - j;
            if (x < 0) continue; // иногда может быть отрицательным, если параметры не подходят
            // Проверка на всякий случай
            if (mod_pow(a, x, p) == y % p) {
                return x;
            }
        }
    }

    return -1; // решение не найдено
}

// Генерация случайных параметров и решение
std::tuple<long long, long long, long long, long long> bsgs_with_random_params() {
    // Генерируем простое p (небольшое для теста)
    long long p = 1009; // можно заменить на random_prime() из твоей библиотеки
    // Генерируем примитивный корень a по модулю p (для простоты — перебором)
    long long a = 2;
    while (true) {
        bool is_generator = true;
        // Проверка: a — генератор, если a^((p-1)/q) != 1 mod p для всех простых делителей q | (p-1)
        // Для простоты — просто пробуем, пока не найдём работающий
        // В реальности нужно разложить p-1 и проверить порядок
        std::vector<long long> factors = {2, 3, 7}; // пример для p=1009 → p-1=1008=2^4*3^2*7
        for (long long q : factors) {
            if (mod_pow(a, (p-1)/q, p) == 1) {
                is_generator = false;
                break;
            }
        }
        if (is_generator) break;
        a++;
        if (a >= p) {
            a = -1;
            break;
        }
    }

    if (a == -1) {
        return std::make_tuple(-1, -1, -1, -1);
    }

    // Генерируем случайный x в [1, p-2]
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> dis(1, p-2);
    long long x = dis(gen);

    // Вычисляем y = a^x mod p
    long long y = mod_pow(a, x, p);

    // Решаем дискретный логарифм
    long long x_found = baby_step_giant_step(a, y, p);

    return std::make_tuple(a, y, p, x_found);
}

#include <vector>
#include <cmath>

// Получение уникальных простых делителей числа n
std::vector<long long> get_prime_factors(long long n) {
    std::vector<long long> factors;
    for (long long i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            factors.push_back(i);
            while (n % i == 0)
                n /= i;
        }
    }
    if (n > 1)
        factors.push_back(n);
    return factors;
}

// Проверка, является ли g примитивным корнем по модулю p
// Условие: g^((p-1)/q) != 1 mod p для всех простых делителей q числа p-1
bool is_primitive_root(long long g, long long p) {
    if (g <= 1 || g >= p) return false;
    auto factors = get_prime_factors(p - 1); // получаем уникальные простые делители p-1
    for (long long q : factors) {
        if (mod_pow(g, (p - 1) / q, p) == 1)
            return false;
    }
    return true;
}

// Поиск случайного примитивного корня по модулю p
long long find_primitive_root(long long p) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(2, p - 2);

    for (int attempts = 0; attempts < 100; ++attempts) {
        long long g = dist(gen);
        if (is_primitive_root(g, p))
            return g;
    }

    // Если не нашли случайно — перебираем с 2
    for (long long g = 2; g < p; ++g) {
        if (is_primitive_root(g, p))
            return g;
    }
    return -1; // не найден
}

// Режим 1: ручной ввод параметров
void diffie_hellman_manual() {
    long long p, g, X_A, X_B;

    std::cout << "\n--- Diffie-Hellman Key Exchange (ручной ввод) ---\n";
    std::cout << "Введите простое число p: ";
    std::cin >> p;
    std::cout << "Введите примитивный корень g по модулю p: ";
    std::cin >> g;
    std::cout << "Введите секретный ключ X_A первого абонента (1 <= X_A <= p-2): ";
    std::cin >> X_A;
    std::cout << "Введите секретный ключ X_B второго абонента (1 <= X_B <= p-2): ";
    std::cin >> X_B;

    // Проверки (упрощённые)
    if (X_A < 1 || X_A > p - 2 || X_B < 1 || X_B > p - 2) {
        std::cout << "Ошибка: секретные ключи должны быть в диапазоне [1, p-2]\n";
        return;
    }

    // Вычисление открытых ключей
    long long Y_A = mod_pow(g, X_A, p);
    long long Y_B = mod_pow(g, X_B, p);

    // Вычисление общего ключа
    long long K_A = mod_pow(Y_B, X_A, p);
    long long K_B = mod_pow(Y_A, X_B, p);

    std::cout << "\n--- Результаты ---\n";
    std::cout << "Открытый ключ A: Y_A = " << Y_A << "\n";
    std::cout << "Открытый ключ B: Y_B = " << Y_B << "\n";
    std::cout << "Общий ключ (вычислен A): K = " << K_A << "\n";
    std::cout << "Общий ключ (вычислен B): K = " << K_B << "\n";

    if (K_A == K_B) {
        std::cout << "✅ Общий ключ успешно согласован: K = " << K_A << "\n";
    } else {
        std::cout << "❌ Ошибка: ключи не совпадают!\n";
    }
}

// Режим 2: автоматическая генерация параметров
void diffie_hellman_auto() {
    std::cout << "\n--- Diffie-Hellman Key Exchange (автоматическая генерация) ---\n";

    // Генерируем простое p (можно взять из предыдущих функций или захардкодить для теста)
    long long p = 2357; // Пример безопасного простого (можно заменить на random_prime_ab().first, но нужно убедиться, что оно большое и безопасное)

    // Ищем примитивный корень
    long long g = find_primitive_root(p);
    if (g == -1) {
        std::cout << "Не удалось найти примитивный корень по модулю " << p << "\n";
        return;
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(1, p - 2);

    long long X_A = dist(gen);
    long long X_B = dist(gen);

    long long Y_A = mod_pow(g, X_A, p);
    long long Y_B = mod_pow(g, X_B, p);

    long long K_A = mod_pow(Y_B, X_A, p);
    long long K_B = mod_pow(Y_A, X_B, p);

    std::cout << "Сгенерированные параметры:\n";
    std::cout << "   p = " << p << "\n";
    std::cout << "   g = " << g << "\n";
    std::cout << "   X_A = " << X_A << "\n";
    std::cout << "   X_B = " << X_B << "\n";
    std::cout << "   Y_A = " << Y_A << "\n";
    std::cout << "   Y_B = " << Y_B << "\n";
    std::cout << "Общий ключ: K = " << K_A << "\n";

    if (K_A == K_B) {
        std::cout << "✅ Общий ключ успешно согласован.\n";
    } else {
        std::cout << "❌ Ошибка: ключи не совпадают!\n";
    }
}