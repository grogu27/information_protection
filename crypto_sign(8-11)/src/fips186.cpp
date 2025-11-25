#include "../include/fips186.h"
#include "../include/utils.h" // sha256_file, read_file, write_file
#include <random>
#include <fstream>
#include <stdexcept>
#include <iostream>

using namespace std;

// ---- Вспомогательные функции (аналогичные тем, что в ГОСТ-файле) ----
uint64_t FIPS186::to_uint64(const std::vector<unsigned char>& v) {
    if (v.empty()) return 0;
    uint64_t res = 0;
    for (int i = (int)v.size() - 1; i >= 0; --i) {
        res = (res << 8) | v[i];
    }
    return res;
}

std::vector<unsigned char> FIPS186::from_uint64(uint64_t n) {
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

uint64_t FIPS186::mod_mul(uint64_t a, uint64_t b, uint64_t mod) {
    return (a * b) % mod;
}

uint64_t FIPS186::mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 1) return 0;
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = mod_mul(result, base, mod);
        base = mod_mul(base, base, mod);
        exp >>= 1;
    }
    return result;
}

uint64_t FIPS186::mod_inv(uint64_t a, uint64_t mod) {
    a %= mod;
    if (a == 0) throw std::runtime_error("mod_inv: division by zero");
    int64_t t0 = 0, t1 = 1;
    int64_t r0 = mod, r1 = a;
    while (r1 != 0) {
        int64_t q = r0 / r1;
        int64_t tmp = r1;
        r1 = r0 - q * r1;
        r0 = tmp;

        tmp = t1;
        t1 = t0 - q * t1;
        t0 = tmp;
    }
    if (r0 != 1) throw std::runtime_error("mod_inv: no inverse");
    if (t0 < 0) t0 += mod;
    return static_cast<uint64_t>(t0);
}

bool FIPS186::is_zero(const vector<unsigned char>& n) {
    for (auto b : n) if (b != 0) return false;
    return true;
}

bool FIPS186::less(const vector<unsigned char>& a, const vector<unsigned char>& b) {
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        if (a[i] != b[i]) return a[i] < b[i];
    }
    return false;
}

vector<unsigned char> FIPS186::random_number(const vector<unsigned char>& max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned char> dis(0, 255);
    vector<unsigned char> result(max.size());
    do {
        for (size_t i = 0; i < max.size(); ++i) result[i] = dis(gen);
    } while (!less(result, max) || is_zero(result));
    return result;
}

// p=bq+1
//g=^q modp=1
//q=160 бит
// ---- Конструктор: тестовые параметры (НЕ ПРОДАКШН) ----
FIPS186::FIPS186() {
    // Небольшие тестовые простые p и q, и g = h^((p-1)/q) mod p
    // Используются те же числа, что в ГОСТ-примере для совместимости тестов.
    p = from_uint64(1279); // простое
    q = from_uint64(71);   // простой делитель p-1
    // вычислим g - берем h=2 и поднимаем в степень (p-1)/q
    uint64_t p_v = to_uint64(p), q_v = to_uint64(q);
    uint64_t h = 2;
    uint64_t exponent = (p_v - 1) / q_v;
    uint64_t g_v = mod_exp(h, exponent, p_v);
    if (g_v <= 1) g_v = 34; // fallback (как в ГОСТ)
    g = from_uint64(g_v);
    x = from_uint64(0);
    y = from_uint64(0);
}

// Генерация ключей: секретный x (0 < x < q), публичный y = g^x mod p
void FIPS186::generate_keys() {
    x = random_number(q);
    uint64_t x_v = to_uint64(x);
    uint64_t p_v = to_uint64(p);
    uint64_t g_v = to_uint64(g);
    uint64_t y_v = mod_exp(g_v, x_v, p_v);

    // записываем публичный ключ
    y = from_uint64(y_v);
}

// Подпись (DSA-подобная):
// r = (g^k mod p) mod q
// s = k^{-1} (h + x*r) mod q
//y=g^x modq

//r нужен, чтобы верифицировать подпись и связать её с одноразовым k и секретным ключом 𝑥
//s кодирует информацию о секретном ключе x в подписи.
pair<vector<unsigned char>, vector<unsigned char>> 
FIPS186::sign(const vector<unsigned char>& message_hash) {
    uint64_t q_v = to_uint64(q);
    uint64_t p_v = to_uint64(p);
    uint64_t g_v = to_uint64(g);
    uint64_t x_v = to_uint64(x);

    uint64_t h = to_uint64(message_hash) % q_v;
    if (h == 0) h = 1;

    uint64_t r = 0, s = 0;
    do {
        uint64_t k = to_uint64(random_number(q));
        if (k == 0) continue;
        uint64_t k_mod = k % q_v;
        uint64_t tmp = mod_exp(g_v, k, p_v) % q_v;
        r = tmp;
        if (r == 0) continue;
        uint64_t k_inv = mod_inv(k_mod, q_v);
        s = (k_inv * ((h + (x_v * r) % q_v) % q_v)) % q_v;
    } while (r == 0 || s == 0);

    return make_pair(from_uint64(r), from_uint64(s));
}

// Проверка подписи:
// 0 < r < q, 0 < s < q
// w = s^{-1} mod q
// u1 = (h * w) mod q
// u2 = (r * w) mod q
// v = ((g^u1 * y^u2) mod p) mod q
// проверяем v == r
bool FIPS186::verify(const vector<unsigned char>& message_hash,
                     const pair<vector<unsigned char>, vector<unsigned char>>& signature) {
    uint64_t r = to_uint64(signature.first);
    uint64_t s = to_uint64(signature.second);
    uint64_t q_v = to_uint64(q);
    if (r == 0 || r >= q_v || s == 0 || s >= q_v) return false;

    uint64_t h = to_uint64(message_hash) % q_v;
    if (h == 0) h = 1;

    uint64_t s_inv = mod_inv(s, q_v);
    uint64_t u1 = (h * s_inv) % q_v;
    uint64_t u2 = (r * s_inv) % q_v;

    uint64_t g_v = to_uint64(g);
    uint64_t y_v = to_uint64(y);
    uint64_t p_v = to_uint64(p);

    uint64_t v1 = mod_exp(g_v, u1, p_v);
    uint64_t v2 = mod_exp(y_v, u2, p_v);
    uint64_t v = ( (v1 * v2) % p_v ) % q_v;

    return v == r;
}

void FIPS186::save_signature(const pair<vector<unsigned char>, vector<unsigned char>>& signature,
                             const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file for writing signature");
    uint8_t len_r = static_cast<uint8_t>(signature.first.size());
    uint8_t len_s = static_cast<uint8_t>(signature.second.size());
    file.write(reinterpret_cast<const char*>(&len_r), 1);
    file.write(reinterpret_cast<const char*>(&len_s), 1);
    file.write(reinterpret_cast<const char*>(signature.first.data()), len_r);
    file.write(reinterpret_cast<const char*>(signature.second.data()), len_s);
}

pair<vector<unsigned char>, vector<unsigned char>> 
FIPS186::load_signature(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file for reading signature");
    uint8_t len_r = 0, len_s = 0;
    file.read(reinterpret_cast<char*>(&len_r), 1);
    file.read(reinterpret_cast<char*>(&len_s), 1);
    pair<vector<unsigned char>, vector<unsigned char>> sig;
    sig.first.resize(len_r);
    sig.second.resize(len_s);
    file.read(reinterpret_cast<char*>(sig.first.data()), len_r);
    file.read(reinterpret_cast<char*>(sig.second.data()), len_s);
    return sig;
}

// ---- Утилита: подпись файла и проверка (интерфейс как у ГОСТ) ----
void fips_sign_and_verify_file(const std::string& filename) {
    FIPS186 fips;
    fips.generate_keys();

    auto hash = sha256_file(filename);

    auto signature = fips.sign(hash);
    std::string sigfile = filename + ".fips.sig";
    fips.save_signature(signature, sigfile);
    std::cout << "FIPS signature saved to: " << sigfile << std::endl;

    auto loaded = fips.load_signature(sigfile);
    bool ok = fips.verify(hash, loaded);
    if (ok) std::cout << "FIPS signature verification: OK\n";
    else std::cout << "FIPS signature verification: FAILED\n";
}
