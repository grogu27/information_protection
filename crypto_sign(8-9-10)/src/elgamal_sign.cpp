#include "elgamal_sign.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <cassert>

using u64 = unsigned long long;

// Простые функции
u64 mod_exp(u64 base, u64 exp, u64 mod) {
    u64 res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return res;
}

u64 egcd(u64 a, u64 b, u64& x, u64& y) {
    if (b == 0) { x = 1; y = 0; return a; }
    u64 x1, y1;
    u64 d = egcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
}

u64 mod_inv(u64 a, u64 m) {
    u64 x, y;
    u64 g = egcd(a, m, x, y);
    if (g != 1) return 0;
    return (x % m + m) % m;
}

// Генерация простого числа (простой способ, до 10^7)
bool is_prime(u64 n) {
    if (n < 2) return false;
    for (u64 i = 2; i * i <= n; ++i)
        if (n % i == 0) return false;
    return true;
}

u64 find_prime(u64 min_val = 1000000) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<u64> dis(min_val, min_val + 100000);
    u64 p;
    do {
        p = dis(gen);
    } while (!is_prime(p));
    return p;
}

// Найти примитивный корень по модулю p
u64 find_primitive_root(u64 p) {
    for (u64 g = 2; g < p; ++g) {
        bool ok = true;
        u64 order = p - 1;
        for (u64 i = 2; i * i <= order; ++i) {
            if (order % i == 0) {
                if (mod_exp(g, order / i, p) == 1) { ok = false; break; }
                if (mod_exp(g, order / (order / i), p) == 1) { ok = false; break; }
            }
        }
        if (ok) return g;
    }
    return 2;
}

struct ElGamalKey {
    u64 p, g, x, y; // x — секретный, y = g^x mod p — открытый
};

void save_elgamal_keys(const ElGamalKey& key) {
    std::ofstream priv("elgamal_priv.key");
    priv << key.p << " " << key.g << " " << key.x << "\n";
    std::ofstream pub("elgamal_pub.key");
    pub << key.p << " " << key.g << " " << key.y << "\n";
}

ElGamalKey load_elgamal_keys(bool pub_only = false) {
    ElGamalKey k;
    if (!pub_only) {
        std::ifstream priv("elgamal_priv.key");
        priv >> k.p >> k.g >> k.x;
        k.y = mod_exp(k.g, k.x, k.p);
    } else {
        std::ifstream pub("elgamal_pub.key");
        pub >> k.p >> k.g >> k.y;
    }
    return k;
}

struct ElGamalSignature {
    u64 r, s;
};

std::vector<ElGamalSignature> elgamal_sign_hash(const std::vector<unsigned char>& hash, const ElGamalKey& key) {
    std::vector<ElGamalSignature> sigs;
    std::random_device rd;
    std::mt19937 gen(rd());

    for (unsigned char b : hash) {
        u64 m = b;
        u64 k;
        do {
            k = std::uniform_int_distribution<u64>(1, key.p - 2)(gen);
        } while (egcd(k, key.p - 1, k, k) != 1); // k взаимно просто с p-1

        u64 r = mod_exp(key.g, k, key.p);
        u64 k_inv = mod_inv(k, key.p - 1);
        u64 s = (k_inv * ((m - key.x * r) % (key.p - 1))) % (key.p - 1);
        if (s < 0) s += key.p - 1;

        sigs.push_back({r, s});
    }
    return sigs;
}

bool elgamal_verify_hash(const std::vector<unsigned char>& hash, const std::vector<ElGamalSignature>& sigs, const ElGamalKey& key) {
    if (hash.size() != sigs.size()) return false;
    for (size_t i = 0; i < hash.size(); ++i) {
        u64 m = hash[i];
        u64 r = sigs[i].r;
        u64 s = sigs[i].s;
        if (r <= 0 || r >= key.p) return false;
        u64 v1 = mod_exp(key.y, r, key.p);
        u64 v2 = mod_exp(r, s, key.p);
        u64 lhs = (v1 * v2) % key.p;
        u64 rhs = mod_exp(key.g, m, key.p);
        if (lhs != rhs) return false;
    }
    return true;
}

void elgamal_sign_file(const std::string& filename) {
    auto hash = sha256_file(filename);

    u64 p = find_prime(1000000); // ~10^6, меньше 10!
    u64 g = find_primitive_root(p);
    std::random_device rd;
    std::mt19937 gen(rd());
    u64 x = std::uniform_int_distribution<u64>(1, p - 2)(gen);
    ElGamalKey key{p, g, x, mod_exp(g, x, p)};

    save_elgamal_keys(key);
    auto sigs = elgamal_sign_hash(hash, key);

    std::vector<unsigned char> out;
    for (auto& sig : sigs) {
        // Просто сериализуем как 2*u64 → 16 байт
        for (int i = 0; i < 8; ++i) out.push_back((sig.r >> (i * 8)) & 0xFF);
        for (int i = 0; i < 8; ++i) out.push_back((sig.s >> (i * 8)) & 0xFF);
    }
    write_file(filename + "_sig", out);
    std::cout << "ElGamal signature saved to " << filename + "_sig\n";
}

void elgamal_verify_file(const std::string& filename) {
    auto hash = sha256_file(filename);
    auto raw_sig = read_file(filename + "_sig");

    if (raw_sig.size() % 16 != 0) {
        std::cout << "Invalid signature size\n";
        return;
    }

    std::vector<ElGamalSignature> sigs;
    for (size_t i = 0; i < raw_sig.size(); i += 16) {
        u64 r = 0, s = 0;
        for (int j = 0; j < 8; ++j) r |= (u64(raw_sig[i + j]) << (j * 8));
        for (int j = 0; j < 8; ++j) s |= (u64(raw_sig[i + 8 + j]) << (j * 8));
        sigs.push_back({r, s});
    }

    auto key = load_elgamal_keys(true);
    bool ok = elgamal_verify_hash(hash, sigs, key);

    std::ofstream out(filename + "_ver");
    out << (ok ? "VALID" : "INVALID");
    std::cout << "ElGamal verification: " << (ok ? "VALID" : "INVALID") << "\n";
}