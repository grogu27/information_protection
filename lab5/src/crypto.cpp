// src/crypto.cpp
#include "crypto.hpp"
#include <fstream>
#include <iostream>
#include <random>
#include <cmath>
#include <climits>
#include <unordered_map>
#include <tuple>

// ================= ЛАБА 1: базовые функции =================

// Быстрое возведение в степень по модулю (a^x mod p)
long long mod_pow(long long a, long long x, long long p) {
    if (p == 1) return 0;
    long long res = 1 % p;
    a %= p;
    if (a < 0) a += p;
    while (x > 0) {
        if (x & 1) res = (__int128)res * a % p;
        a = (__int128)a * a % p;
        x >>= 1;
    }
    return res;
}

// Тест Ферма (вероятностный)
bool is_prime_fermat(long long n, int k) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(2, n - 2);

    for (int i = 0; i < k; ++i) {
        long long a = dist(gen);
        long long r = mod_pow(a, n - 1, n);
        if (r != 1) return false;
    }
    return true; // вероятно простое
}

// Обобщённый алгоритм Евклида: возвращает (g, x, y) такие, что a*x + b*y = g
std::tuple<long long, long long, long long> extended_gcd2(long long a, long long b) {
    if (b == 0) {
        long long aa = a >= 0 ? a : -a;
        long long x = (a >= 0) ? 1 : -1;
        return {aa, x, 0};
    }
    long long x0 = 1, y0 = 0;
    long long x1 = 0, y1 = 1;
    while (b != 0) {
        long long q = a / b;
        long long r = a % b;
        long long x2 = x0 - q * x1;
        long long y2 = y0 - q * y1;
        a = b; b = r;
        x0 = x1; y0 = y1;
        x1 = x2; y1 = y2;
    }
    // теперь a = gcd, x0,y0 — коэфф.
    if (a < 0) { a = -a; x0 = -x0; y0 = -y0; }
    return {a, x0, y0};
}

// Вспомогательные генераторы для лабораторной 1
std::pair<long long, long long> random_ab() {
    // Генерируем числа порядка 10! (≈3.6e6) — безопасно для long long
    const long long MAXV = 3628800; // 10!
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(1, MAXV);
    long long a = dist(gen);
    long long b = dist(gen);
    return {a, b};
}

// ================= Вспомогательная: разложение на простые факторы =================
std::vector<long long> get_prime_factors(long long n) {
    std::vector<long long> res;
    if (n % 2 == 0) {
        res.push_back(2);
        while (n % 2 == 0) n /= 2;
    }
    for (long long p = 3; p * p <= n; p += 2) {
        if (n % p == 0) {
            res.push_back(p);
            while (n % p == 0) n /= p;
        }
    }
    if (n > 1) res.push_back(n);
    return res;
}

// Проверяет, является ли g примитивным корнем modulo p (p должно быть простым)
bool is_primitive_root(long long g, long long p) {
    if (p == 2) return true;
    long long phi = p - 1;
    auto factors = get_prime_factors(phi);
    for (long long q : factors) {
        if (mod_pow(g, phi / q, p) == 1) return false;
    }
    return true;
}

// Находит примитивный корень для p (если p простое)
long long find_primitive_root(long long p) {
    if (p == 2) return 1;
    for (long long g = 2; g < p; ++g) {
        if (is_primitive_root(g, p)) return g;
    }
    return -1;
}

// ================= ЛАБА: генерация простых/парам. =================
long long generate_prime_for_crypto() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    // Генерируем от 257 до 1_000_000 — достаточно для заданий
    std::uniform_int_distribution<long long> dist(257, 1000000);
    while (true) {
        long long n = dist(gen);
        if (n % 2 == 0) n += 1;
        // постепенно пробуем нечетные
        for (int i = 0; i < 1000; ++i) {
            if (is_prime_fermat(n, 20)) return n;
            n += 2;
        }
    }
}

// Генерирует пару простых чисел p,q
std::pair<long long, long long> random_prime_ab() {
    long long p = generate_prime_for_crypto();
    long long q = generate_prime_for_crypto();
    while (q == p) q = generate_prime_for_crypto();
    return {p, q};
}

// ================= ЛАБА 2: BSGS =================

// baby-step giant-step: находит x такое, что a^x ≡ y (mod p), либо -1 если не найден
// baby-step giant-step: находит x такое, что a^x ≡ y (mod p), либо -1 если не найден
long long baby_step_giant_step(long long a, long long y, long long p) {
    a %= p; y %= p;
    if (p == 1) return 0;
    if (y == 1) return 0;
    long long m = (long long) std::ceil(std::sqrt((double)p));
    std::unordered_map<long long, long long> table;
    long long aj = 1;
    for (long long j = 0; j < m; ++j) {
        if (!table.count(aj)) table[aj] = j;
        aj = (__int128)aj * a % p;
    }
    long long a_m = mod_pow(a, m, p);
    // Найдём обратный к a_m по модулю p с помощью расширенного Евклида
    auto [g, inv_am, tmp] = extended_gcd2(a_m, p);
    if (g != 1) {
        // если a_m и p не взаимно просты — fallback: используй прямые гигантские шаги
        long long cur = y;
        for (long long i = 0; i <= m; ++i) {
            if (table.count(cur)) {
                long long ans = i * m + table[cur];
                return ans;
            }
            cur = (__int128)cur * a_m % p;
        }
        return -1;
    }
    inv_am %= p; if (inv_am < 0) inv_am += p;
    long long gamma = y;
    for (long long i = 0; i <= m; ++i) {
        if (table.count(gamma)) {
            long long ans = i * m + table[gamma];
            return ans;
        }
        gamma = (__int128)gamma * inv_am % p;
    }
    return -1;
}


// Сгенерировать параметры (a,y,p,x) для BSGS: выбираем p простое, a — прим.корень, x случайный, y = a^x mod p
std::tuple<long long, long long, long long, long long> bsgs_with_random_params() {
    long long p = generate_prime_for_crypto();
    long long a = find_primitive_root(p);
    if (a == -1) return {-1,-1,-1,-1};
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(1, p-2);
    long long x = dist(gen);
    long long y = mod_pow(a, x, p);
    return {a, y, p, x};
}

// ================= ЛАБА 3: Диффи-Хеллман =================

void diffie_hellman_manual() {
    long long p, g, Xa, Xb;
    std::cout << "p g Xa Xb: ";
    std::cin >> p >> g >> Xa >> Xb;
    Xa %= (p-1); if (Xa < 0) Xa += (p-1);
    Xb %= (p-1); if (Xb < 0) Xb += (p-1);
    long long A = mod_pow(g, Xa, p);
    long long B = mod_pow(g, Xb, p);
    long long K1 = mod_pow(B, Xa, p);
    long long K2 = mod_pow(A, Xb, p);
    std::cout << "Public keys: A=" << A << " B=" << B << "\n";
    std::cout << "Shared secrets: K1=" << K1 << " K2=" << K2 << "\n";
}

void diffie_hellman_auto() {
    long long p = generate_prime_for_crypto();
    long long g = find_primitive_root(p);
    if (g == -1) {
        std::cerr << "Не удалось найти примитивный корень\n";
        return;
    }
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(1, p-2);
    long long Xa = dist(gen);
    long long Xb = dist(gen);
    long long A = mod_pow(g, Xa, p);
    long long B = mod_pow(g, Xb, p);
    long long K1 = mod_pow(B, Xa, p);
    long long K2 = mod_pow(A, Xb, p);
    std::cout << "Generated: p=" << p << " g=" << g << "\n";
    std::cout << "Xa=" << Xa << " Xb=" << Xb << "\n";
    std::cout << "Public keys: A=" << A << " B=" << B << "\n";
    std::cout << "Shared secrets: K1=" << K1 << " K2=" << K2 << "\n";
}

// =============== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ И ЛАБЫ 4-6 ===============

long long mod_inverse(long long a, long long m) {
    auto [g, x, y] = extended_gcd2(a, m);
    if (g != 1) return -1;
    x %= m;
    if (x < 0) x += m;
    return x;
}

void write_long(std::ofstream& out, long long val) {
    for (int i = 0; i < 8; ++i) {
        out.put(static_cast<char>(val & 0xFF));
        val >>= 8;
    }
}

// Считать 8-байтовое число; вернуть true при успешном чтении, false при EOF/ошибке
bool read_long(std::ifstream& in, long long &val) {
    val = 0;
    for (int i = 0; i < 8; ++i) {
        char byte;
        if (!in.get(byte)) return false; // EOF или ошибка — сообщаем вызывающему
        val |= (static_cast<unsigned long long>(static_cast<unsigned char>(byte)) << (8 * i));
    }
    return true;
}

// =============== ЛАБА 4: ТРЁХЭТАПНЫЙ ПРОТОКОЛ ШАМИРА ===============

// Генерирует (e, d) такие, что e*d ≡ 1 (mod p-1)
std::pair<long long, long long> generate_shamir_keys(long long p) {
    long long phi = p - 1; // так как p — простое
    std::random_device rd;
    std::mt19937_64 gen(rd());
    // Убедимся, что phi >= 2
    if (phi < 2) {
        std::cerr << "p слишком мало!\n";
        exit(1);
    }
    std::uniform_int_distribution<long long> dist(2, phi - 1);

    long long e, d;
    int attempts = 0;
    do {
        e = dist(gen);
        d = mod_inverse(e, phi);
        attempts++;
        if (attempts > 1000) {
            // fallback: попробуем e=2,3,5...
            static long long small_primes[] = {2,3,5,7,11,13,17,19,23,29};
            for (long long cand : small_primes) {
                if (cand < phi) {
                    d = mod_inverse(cand, phi);
                    if (d != -1) {
                        e = cand;
                        goto found;
                    }
                }
            }
            std::cerr << "Не удалось сгенерировать ключи для p=" << p << "\n";
            exit(1);
        }
    } while (d == -1);
    found:
    return {e, d};
}

// Применяет операцию x -> x^exp mod p к каждому 8-байтовому блоку
bool shamir_three_pass_step(const std::string& in_file, const std::string& out_file,
                            long long exp, long long p) {
    std::ifstream in(in_file, std::ios::binary);
    std::ofstream out(out_file, std::ios::binary);
    if (!in || !out) return false;

    long long block;
    while (read_long(in, block)) {
        if (block >= p || block < 0) {
            std::cerr << "Ошибка: блок вне диапазона [0, p)\n";
            return false;
        }
        long long result = mod_pow(block, exp, p);
        write_long(out, result);
    }
    return true;
}

void lab4_shamir() {
    std::cout << "\n--- Лабораторная №4: Трёхэтапный протокол Шамира ---\n";

    long long p = generate_prime_for_crypto();
    // Убедимся, что p > 256 (для байтов)
    if (p <= 256) {
        p = 257; // минимальное подходящее простое
    }

    auto [eA, dA] = generate_shamir_keys(p); // Алиса
    auto [eB, dB] = generate_shamir_keys(p); // Боб

    std::cout << "Сгенерировано:\n";
    std::cout << "p = " << p << "\n";
    std::cout << "Алиса: eA=" << eA << ", dA=" << dA << "\n";
    std::cout << "Боб:   eB=" << eB << ", dB=" << dB << "\n";

    std::string original_file;
    std::cout << "Исходный файл: "; std::cin >> original_file;
    std::string stage1 = original_file + ".s1";
    std::string stage2 = original_file + ".s2";
    std::string stage3 = original_file + ".s3";
    std::string decrypted = original_file + ".dec";

    // === Этап 1: Алиса шифрует (M -> M^eA mod p) ===
    {
        std::ifstream in(original_file, std::ios::binary);
        std::ofstream out(stage1, std::ios::binary);
        if (!in || !out) {
            std::cerr << "Не удалось открыть файлы этапа 1\n";
            return;
        }
        char byte;
        while (in.read(&byte, 1)) {
            long long M = static_cast<unsigned char>(byte); // 0..255
            if (M >= p) {
                std::cerr << "Ошибка: p слишком мало для данных!\n";
                return;
            }
            long long C1 = mod_pow(M, eA, p);
            write_long(out, C1);
        }
    }
    std::cout << "Этап 1 (Алиса): " << stage1 << "\n";

    // === Этап 2: Боб шифрует (C1 -> C1^eB mod p) ===
    if (!shamir_three_pass_step(stage1, stage2, eB, p)) {
        std::cerr << "Ошибка на этапе 2\n";
        return;
    }
    std::cout << "Этап 2 (Боб): " << stage2 << "\n";

    // === Этап 3: Алиса расшифровывает (C2 -> C2^dA mod p) ===
    if (!shamir_three_pass_step(stage2, stage3, dA, p)) {
        std::cerr << "Ошибка на этапе 3\n";
        return;
    }
    std::cout << "Этап 3 (Алиса): " << stage3 << "\n";

    // === Боб расшифровывает (C3 -> C3^dB mod p) ===
    {
        std::ifstream in(stage3, std::ios::binary);
        std::ofstream out(decrypted, std::ios::binary);
        if (!in || !out) {
            std::cerr << "Не удалось открыть файлы расшифровки\n";
            return;
        }
        long long block;
        while (read_long(in, block)) {
            long long M = mod_pow(block, dB, p);
            if (M < 0 || M > 255) {
                std::cerr << "Ошибка: восстановленный байт вне диапазона [0,255]\n";
                return;
            }
            out.put(static_cast<char>(static_cast<unsigned char>(M)));
        }
    }
    std::cout << "Расшифровано: " << decrypted << "\n";
    std::cout << "Готово! Сравните " << original_file << " и " << decrypted << "\n";
}


// ================= ЛАБА 5: ШИФР ЭЛЬ-ГАМАЛЯ =================

void elgamal_encrypt_file(const std::string& input_file, const std::string& output_file,
                         long long p, long long g, long long y) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);
    
    if (!in || !out) {
        std::cerr << "Ошибка открытия файлов для шифрования\n";
        return;
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dist(1, p-2);

    char byte;
    while (in.read(&byte, 1)) {
        long long m = static_cast<unsigned char>(byte);
        
        // Убедимся, что сообщение меньше p
        if (m >= p) {
            std::cerr << "Ошибка: p слишком мало для шифрования байтов\n";
            return;
        }
        
        // Генерируем случайный k для каждого байта
        long long k = dist(gen);
        
        // Вычисляем a = g^k mod p
        long long a = mod_pow(g, k, p);
        
        // Вычисляем b = m * y^k mod p
        long long b = (m * mod_pow(y, k, p)) % p;
        
        // Записываем пару (a, b)
        write_long(out, a);
        write_long(out, b);
    }
    
    std::cout << "Файл зашифрован: " << output_file << "\n";
}

void elgamal_decrypt_file(const std::string& input_file, const std::string& output_file,
                         long long p, long long x) {
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);
    
    if (!in || !out) {
        std::cerr << "Ошибка открытия файлов для расшифрования\n";
        return;
    }

    long long a, b;
    while (read_long(in, a) && read_long(in, b)) {
        // Вычисляем s = a^x mod p
        long long s = mod_pow(a, x, p);
        
        // Находим обратный элемент s^{-1} mod p
        long long s_inv = mod_inverse(s, p);
        if (s_inv == -1) {
            std::cerr << "Ошибка: не удалось найти обратный элемент\n";
            return;
        }
        
        // Восстанавливаем сообщение: m = b * s^{-1} mod p
        long long m = (b * s_inv) % p;
        
        // Проверяем, что m в диапазоне байта
        if (m < 0 || m > 255) {
            std::cerr << "Ошибка: некорректное значение байта при расшифровании\n";
            return;
        }
        
        out.put(static_cast<char>(static_cast<unsigned char>(m)));
    }
    
    std::cout << "Файл расшифрован: " << output_file << "\n";
}

void lab5_elgamal() {
    std::cout << "\n--- Лабораторная №5: Шифр Эль-Гамаля ---\n";
    
    int mode;
    std::cout << "1. Ввод параметров с клавиатуры\n";
    std::cout << "2. Автоматическая генерация параметров\n";
    std::cout << "Выбор: ";
    std::cin >> mode;
    
    long long p, g, x, y;
    
    if (mode == 1) {
        // Ручной ввод параметров
        std::cout << "Введите p (простое число): ";
        std::cin >> p;
        std::cout << "Введите g (примитивный корень): ";
        std::cin >> g;
        std::cout << "Введите y (открытый ключ): ";
        std::cin >> y;
        
        // Для расшифрования нужен секретный ключ x
        std::cout << "Для расшифрования введите x (секретный ключ): ";
        std::cin >> x;
    } else {
        // Автоматическая генерация параметров
        p = generate_prime_for_crypto();
        g = find_primitive_root(p);
        
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, p-2);
        
        x = dist(gen);  // Секретный ключ
        y = mod_pow(g, x, p);  // Открытый ключ
        
        std::cout << "Сгенерированные параметры:\n";
        std::cout << "p = " << p << "\n";
        std::cout << "g = " << g << "\n";
        std::cout << "x (секретный) = " << x << "\n";
        std::cout << "y (открытый) = " << y << "\n";
    }
    
    int operation;
    std::cout << "\n1. Шифрование\n";
    std::cout << "2. Расшифрование\n";
    std::cout << "Выбор: ";
    std::cin >> operation;
    
    std::string input_file, output_file;
    std::cout << "Входной файл: ";
    std::cin >> input_file;
    std::cout << "Выходной файл: ";
    std::cin >> output_file;
    
    if (operation == 1) {
        elgamal_encrypt_file(input_file, output_file, p, g, y);
    } else {
        elgamal_decrypt_file(input_file, output_file, p, x);
    }
}