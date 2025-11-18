#include "gost341094.h"
#include <random>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

static uint64_t to_uint64(const std::vector<unsigned char>& v) {
    if (v.empty()) return 0;
    uint64_t res = 0;
    for (int i = v.size() - 1; i >= 0; --i) {
        res = (res << 8) | v[i];
    }
    return res;
}

static std::vector<unsigned char> from_uint64(uint64_t n) {
    std::vector<unsigned char> result;
    if (n == 0) {
        result.push_back(0);
        return result;
    }
    while (n > 0) {
        result.push_back(static_cast<unsigned char>(n & 0xFF));
        n >>= 8;
    }
    return result;
}

static uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t mod) {
    return (a * b) % mod;
}

static uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 1) return 0;
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
            result = mod_mul(result, base, mod);
        }
        base = mod_mul(base, base, mod);
        exp >>= 1;
    }
    return result;
}

// Расширенный алгоритм Евклида для нахождения обратного по модулю
static uint64_t mod_inv(uint64_t a, uint64_t mod) {
    a %= mod;
    if (a == 0) throw std::runtime_error("mod_inv: division by zero");
    
    int64_t x0 = 0, x1 = 1, r0 = mod, r1 = a;
    while (r1 != 0) {
        int64_t q = r0 / r1;
        int64_t tmp = r1;
        r1 = r0 - q * r1;
        r0 = tmp;
        
        tmp = x1;
        x1 = x0 - q * x1;
        x0 = tmp;
    }
    
    if (r0 != 1) throw std::runtime_error("mod_inv: no inverse");
    if (x0 < 0) x0 += mod;
    return static_cast<uint64_t>(x0);
}

GOST341094::GOST341094() {
    p = from_uint64(1279); // Простое число
    q = from_uint64(71);   // Простой делитель (p-1)
    a = from_uint64(34);    // Образующий элемент a^q mod p = 1
}

bool GOST341094::is_zero(const vector<unsigned char>& n) {
    for (auto byte : n) {
        if (byte != 0) return false;
    }
    return true;
}

bool GOST341094::less(const vector<unsigned char>& a, const vector<unsigned char>& b) {
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = a.size() - 1; i >= 0; i--) {
        if (a[i] != b[i]) return a[i] < b[i];
    }
    return false;
}

vector<unsigned char> GOST341094::random_number(const vector<unsigned char>& max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned char> dis(0, 255);
    
    vector<unsigned char> result(max.size());
    do {
        for (size_t i = 0; i < max.size(); i++) {
            result[i] = dis(gen);
        }
    } while (!less(result, max) || is_zero(result));
    
    return result;
}

// Генерирует случайный закрытый ключ d (1 < d < q)
// Вычисляет открытый ключ: c = a^d mod p
// Проверяет корректность: c^q mod p == 1
void GOST341094::generate_keys() {
    d = random_number(q);
    uint64_t d_val = to_uint64(d);
    uint64_t p_val = to_uint64(p);
    uint64_t a_val = to_uint64(a);
    uint64_t y_val = mod_exp(a_val, d_val, p_val);
    
    // Проверка: y^q mod p == 1
    uint64_t check = mod_exp(y_val, to_uint64(q), p_val);
    if (check != 1) {
        throw std::runtime_error("Invalid public key: y^q mod p != 1");
    }
    
    c = from_uint64(y_val);
}

// Вычисляет h = hash(message) mod q (если h=0, то h=1)
// Генерирует случайное k (1 < k < q)
// Вычисляет r = (a^k mod p) mod q
// Вычисляет s = (k*h + d*r) mod q
// Если r=0 или s=0, повторяет с новым k
// Результат: пара (r, s) - цифровая подпись
pair<vector<unsigned char>, vector<unsigned char>> 
GOST341094::sign(const vector<unsigned char>& message_hash) {
    uint64_t h = to_uint64(message_hash) % to_uint64(q);
    if (h == 0) h = 1;

    uint64_t p_val = to_uint64(p);
    uint64_t q_val = to_uint64(q);
    uint64_t a_val = to_uint64(a);
    uint64_t d_val = to_uint64(d);

    uint64_t r = 0, s = 0, k = 0;
    do {
        k = to_uint64(random_number(q));
        if (k == 0) continue;

        r = mod_exp(a_val, k, p_val) % q_val;
        s = (k * h + d_val * r) % q_val;

    } while (r == 0 || s == 0);

    return make_pair(from_uint64(r), from_uint64(s));
}

// Проверяет: 0 < r < q и 0 < s < q
// Вычисляет h = hash(message) mod q
// Вычисляет v = h^(-1) mod q
// Вычисляет z1 = s*v mod q
// Вычисляет z2 = (-r)*v mod q
// Вычисляет u = (a^z1 * y^z2 mod p) mod q
// Если u == r - подпись верна
bool GOST341094::verify(const vector<unsigned char>& message_hash,
                       const pair<vector<unsigned char>, vector<unsigned char>>& signature) {
    uint64_t r = to_uint64(signature.first);
    uint64_t s = to_uint64(signature.second);
    uint64_t q_val = to_uint64(q);

    if (r == 0 || r >= q_val || s == 0 || s >= q_val) {
        return false;
    }

    uint64_t h = to_uint64(message_hash) % q_val;
    if (h == 0) h = 1;

    uint64_t p_val = to_uint64(p);
    uint64_t a_val = to_uint64(a);
    uint64_t y_val = to_uint64(c); // открытый ключ

    uint64_t v = mod_inv(h, q_val);
    uint64_t z1 = (s * v) % q_val;
    uint64_t z2 = ((q_val - r) * v) % q_val;  // -r mod q

    uint64_t u1 = mod_exp(a_val, z1, p_val);
    uint64_t u2 = mod_exp(y_val, z2, p_val);
    uint64_t u = (u1 * u2) % p_val;
    u %= q_val;

    return (u == r);
}

void GOST341094::save_signature(const pair<vector<unsigned char>, vector<unsigned char>>& signature, 
                               const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file for writing signature");

    // Пишем размер r (1 байт — достаточно для тестов)
    uint8_t len_r = static_cast<uint8_t>(signature.first.size());
    uint8_t len_s = static_cast<uint8_t>(signature.second.size());
    file.write(reinterpret_cast<const char*>(&len_r), 1);
    file.write(reinterpret_cast<const char*>(&len_s), 1);

    // Пишем байты r и s
    file.write(reinterpret_cast<const char*>(signature.first.data()), len_r);
    file.write(reinterpret_cast<const char*>(signature.second.data()), len_s);
}

pair<vector<unsigned char>, vector<unsigned char>> 
GOST341094::load_signature(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file for reading signature");

    uint8_t len_r, len_s;
    file.read(reinterpret_cast<char*>(&len_r), 1);
    file.read(reinterpret_cast<char*>(&len_s), 1);

    pair<vector<unsigned char>, vector<unsigned char>> signature;
    signature.first.resize(len_r);
    signature.second.resize(len_s);

    file.read(reinterpret_cast<char*>(signature.first.data()), len_r);
    file.read(reinterpret_cast<char*>(signature.second.data()), len_s);

    return signature;
}