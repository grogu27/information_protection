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

// Вспомогательная функция: модульное обратное
long long mod_inverse(long long a, long long m) {
    auto [g, x, y] = extended_gcd2(a, m);
    if (g != 1) return -1;
    x %= m;
    if (x < 0) x += m;
    return x;
}

// Генерация простого > 256 (уже определена выше, но оставляем объявление здесь)
// long long generate_prime_for_crypto();  -- определено раньше

// =============== ЛАБА 4: АФФИННЫЙ ШИФР ("Шамир") ===============
bool shamir_process_file(const std::string& in_file, const std::string& out_file,
                         long long C1, long long C2, long long p, bool decrypt) {
    std::ifstream in(in_file, std::ios::binary);
    std::ofstream out(out_file, std::ios::binary);
    if (!in || !out) return false;

    if (decrypt) {
        long long inv = mod_inverse(C1, p);
        if (inv == -1) return false;
        char byte;
        while (in.read(&byte, 1)) {
            long long y = static_cast<unsigned char>(byte);
            long long x = ((y - C2 + p) % p * inv) % p;
            if (x > 255) return false;
            out.put(static_cast<char>(x));
        }
    } else {
        char byte;
        while (in.read(&byte, 1)) {
            long long x = static_cast<unsigned char>(byte);
            long long y = (C1 * x + C2) % p;
            if (y > 255) {
                std::cerr << "Ошибка: p слишком мало, y = " << y << " > 255\n";
                return false;
            }
            out.put(static_cast<char>(y));
        }
    }
    return true;
}

void lab4_shamir() {
    int mode;
    std::cout << "\n--- Лабораторная №4: Аффинный шифр (\"Шамир\") ---\n";
    std::cout << "1. Ввести p, C1, C2 вручную\n";
    std::cout << "2. Сгенерировать параметры\n";
    std::cin >> mode;

    long long p, C1, C2;
    if (mode == 1) {
        std::cout << "p (простое, >256): "; std::cin >> p;
        std::cout << "C1 (1 <= C1 < p, gcd(C1,p)=1): "; std::cin >> C1;
        std::cout << "C2 (0 <= C2 < p): "; std::cin >> C2;
        if (C1 <= 0 || C1 >= p || C2 < 0 || C2 >= p) {
            std::cerr << "Неверные границы!\n"; return;
        }
        if (mod_inverse(C1, p) == -1) {
            std::cerr << "C1 не обратим по модулю p!\n"; return;
        }
    } else {
        p = generate_prime_for_crypto();
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, p - 1);
        C1 = dist(gen);
        while (mod_inverse(C1, p) == -1) C1 = dist(gen);
        C2 = dist(gen) % p;
        std::cout << "Сгенерировано: p=" << p << ", C1=" << C1 << ", C2=" << C2 << "\n";
    }

    std::string in_file, out_file;
    std::cout << "Входной файл: "; std::cin >> in_file;
    std::cout << "Выходной файл: "; std::cin >> out_file;

    int op; std::cout << "1. Шифровать  2. Расшифровать: "; std::cin >> op;
    if (op == 1) {
        if (shamir_process_file(in_file, out_file, C1, C2, p, false))
            std::cout << "Успешно зашифровано.\n";
        else
            std::cerr << "Ошибка шифрования.\n";
    } else if (op == 2) {
        if (shamir_process_file(in_file, out_file, C1, C2, p, true))
            std::cout << "Успешно расшифровано.\n";
        else
            std::cerr << "Ошибка расшифровки.\n";
    }
}

// =============== ЛАБА 5: ЭЛЬ-ГАМАЛЬ ===============
// Запись/чтение 8-байтового числа
void write_long(std::ofstream& out, long long val) {
    for (int i = 0; i < 8; ++i) {
        out.put(static_cast<char>(val & 0xFF));
        val >>= 8;
    }
}

long long read_long(std::ifstream& in) {
    long long val = 0;
    for (int i = 0; i < 8; ++i) {
        char byte;
        if (!in.get(byte)) return -1;
        val |= (static_cast<unsigned long long>(static_cast<unsigned char>(byte)) << (8 * i));
    }
    return val;
}

bool elgamal_process_file(const std::string& in_file, const std::string& out_file,
                          long long p, long long g, long long y, long long x, bool decrypt) {
    std::ifstream in(in_file, std::ios::binary);
    std::ofstream out(out_file, std::ios::binary);
    if (!in || !out) return false;

    if (decrypt) {
        long long a, b;
        while ((a = read_long(in)) != -1 && (b = read_long(in)) != -1) {
            long long s = mod_pow(a, x, p);
            long long s_inv = mod_inverse(s, p);
            if (s_inv == -1) return false;
            long long m = (b * s_inv) % p;
            if (m > 255) return false;
            out.put(static_cast<char>(m));
        }
    } else {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> k_dist(1, p - 2);

        char byte;
        while (in.read(&byte, 1)) {
            long long m = static_cast<unsigned char>(byte);
            long long k = k_dist(gen);
            long long a = mod_pow(g, k, p);
            long long b = (m * mod_pow(y, k, p)) % p;
            write_long(out, a);
            write_long(out, b);
        }
    }
    return true;
}

void lab5_elgamal() {
    int mode;
    std::cout << "\n--- Лабораторная №5: Эль-Гамаль ---\n";
    std::cout << "1. Ввести параметры\n2. Сгенерировать ключи\n";
    std::cin >> mode;

    long long p, g, x = -1, y = -1;
    if (mode == 1) {
        std::cout << "p (>256): "; std::cin >> p;
        std::cout << "g (примитивный корень): "; std::cin >> g;
        std::cout << "y = g^x mod p: "; std::cin >> y;
        std::cout << "x (приватный ключ): "; std::cin >> x;
    } else {
        p = generate_prime_for_crypto();
        g = find_primitive_root(p);
        if (g == -1) { std::cerr << "Не найден g\n"; return; }
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dist(1, p - 2);
        x = dist(gen);
        y = mod_pow(g, x, p);
        std::cout << "Сгенерировано:\n p=" << p << "\n g=" << g << "\n x=" << x << "\n y=" << y << "\n";
    }

    std::string in_file, out_file;
    std::cout << "Входной файл: "; std::cin >> in_file;
    std::cout << "Выходной файл: "; std::cin >> out_file;

    int op; std::cout << "1. Шифровать  2. Расшифровать: "; std::cin >> op;
    if (op == 1) {
        if (elgamal_process_file(in_file, out_file, p, g, y, -1, false))
            std::cout << "Успешно зашифровано.\n";
        else
            std::cerr << "Ошибка шифрования.\n";
    } else if (op == 2) {
        if (elgamal_process_file(in_file, out_file, p, g, y, x, true))
            std::cout << "Успешно расшифровано.\n";
        else
            std::cerr << "Ошибка расшифровки.\n";
    }
}

// =============== ЛАБА 6: RSA ===============
bool rsa_process_file(const std::string& in_file, const std::string& out_file,
                      long long n, long long exp, bool decrypt) {
    std::ifstream in(in_file, std::ios::binary);
    std::ofstream out(out_file, std::ios::binary);
    if (!in || !out) return false;

    if (decrypt) {
        long long c;
        while ((c = read_long(in)) != -1) {
            long long m = mod_pow(c, exp, n);
            if (m > 255) return false;
            out.put(static_cast<char>(m));
        }
    } else {
        char byte;
        while (in.read(&byte, 1)) {
            long long m = static_cast<unsigned char>(byte);
            if (m >= n) {
                std::cerr << "Ошибка: m >= n! Увеличьте p, q.\n";
                return false;
            }
            long long c = mod_pow(m, exp, n);
            write_long(out, c);
        }
    }
    return true;
}

void lab6_rsa() {
    int mode;
    std::cout << "\n--- Лабораторная №6: RSA ---\n";
    std::cout << "1. Ввести p, q, d\n2. Сгенерировать ключи\n";
    std::cin >> mode;

    long long p, q, n, e, d;
    if (mode == 1) {
        std::cout << "p: "; std::cin >> p;
        std::cout << "q: "; std::cin >> q;
        std::cout << "d: "; std::cin >> d;
        n = p * q;
        long long phi = (p - 1) * (q - 1);
        e = mod_inverse(d, phi);
        if (e == -1) { std::cerr << "Неверный d\n"; return; }
    } else {
        auto [p1, q1] = random_prime_ab();
        p = p1; q = q1;
        n = p * q;
        long long phi = (p - 1) * (q - 1);
        e = 65537;
        if (e >= phi || mod_inverse(e, phi) == -1) {
            e = 3;
            while (e < phi && mod_inverse(e, phi) == -1) e += 2;
        }
        d = mod_inverse(e, phi);
        std::cout << "Сгенерировано:\n p=" << p << "\n q=" << q << "\n n=" << n << "\n e=" << e << "\n d=" << d << "\n";
    }

    std::string in_file, out_file;
    std::cout << "Входной файл: "; std::cin >> in_file;
    std::cout << "Выходной файл: "; std::cin >> out_file;

    int op; std::cout << "1. Шифровать (e)  2. Расшифровать (d): "; std::cin >> op;
    if (op == 1) {
        if (rsa_process_file(in_file, out_file, n, e, false))
            std::cout << "Успешно зашифровано.\n";
        else
            std::cerr << "Ошибка шифрования.\n";
    } else if (op == 2) {
        if (rsa_process_file(in_file, out_file, n, d, true))
            std::cout << "Успешно расшифровано.\n";
        else
            std::cerr << "Ошибка расшифровки.\n";
    }
}
