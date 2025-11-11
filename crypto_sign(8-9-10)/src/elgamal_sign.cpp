#include "elgamal_sign.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <cassert>
#include <numeric>

using u64 = unsigned long long;
using i64 = long long;

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

u64 gcd(u64 a, u64 b) {
    while (b != 0) {
        u64 temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

u64 mod_inv(u64 a, u64 m) {
    i64 m0 = m, t, q;
    i64 x0 = 0, x1 = 1;
    
    if (m == 1) return 0;
    
    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    
    if (x1 < 0) x1 += m0;
    return x1;
}

// Генерация простого числа
bool is_prime(u64 n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (u64 i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

u64 find_prime(u64 min_val = 1000000) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<u64> dis(min_val, min_val + 1000000);
    u64 p;
    do {
        p = dis(gen);
    } while (!is_prime(p));
    return p;
}

// Найти примитивный корень по модулю p
u64 find_primitive_root(u64 p) {
    std::vector<u64> factors;
    u64 phi = p - 1;
    u64 n = phi;
    
    // Факторизуем phi = p-1
    for (u64 i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            factors.push_back(i);
            while (n % i == 0) n /= i;
        }
    }
    if (n > 1) factors.push_back(n);
    
    // Ищем примитивный корень
    for (u64 g = 2; g < p; ++g) {
        bool ok = true;
        for (u64 factor : factors) {
            if (mod_exp(g, phi / factor, p) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
    return 2;
}

struct ElGamalKey {
    u64 p, g, x, y;
};

void save_elgamal_keys(const ElGamalKey& key) {
    std::ofstream priv("elgamal_priv.key");
    priv << key.p << " " << key.g << " " << key.x << "\n";
    priv.close();
    
    std::ofstream pub("elgamal_pub.key");
    pub << key.p << " " << key.g << " " << key.y << "\n";
    pub.close();
}

ElGamalKey load_elgamal_keys(bool pub_only = false) {
    ElGamalKey k;
    if (!pub_only) {
        std::ifstream priv("elgamal_priv.key");
        if (!priv) throw std::runtime_error("Cannot open private key file");
        priv >> k.p >> k.g >> k.x;
        k.y = mod_exp(k.g, k.x, k.p);
        priv.close();
    } else {
        std::ifstream pub("elgamal_pub.key");
        if (!pub) throw std::runtime_error("Cannot open public key file");
        pub >> k.p >> k.g >> k.y;
        pub.close();
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
    std::uniform_int_distribution<u64> dis(1, key.p - 2);

    for (unsigned char byte : hash) {
        u64 m = byte;
        
        // Генерируем k взаимно простое с p-1
        u64 k;
        do {
            k = dis(gen);
        } while (gcd(k, key.p - 1) != 1);
        
        u64 r = mod_exp(key.g, k, key.p);
        u64 k_inv = mod_inv(k, key.p - 1);
        
        // Вычисляем s = k^(-1) * (m - x*r) mod (p-1)
        i64 temp = static_cast<i64>(m) - static_cast<i64>(key.x) * static_cast<i64>(r);
        temp = temp % static_cast<i64>(key.p - 1);
        if (temp < 0) temp += key.p - 1;
        
        u64 s = (k_inv * static_cast<u64>(temp)) % (key.p - 1);
        
        sigs.push_back({r, s});
    }
    return sigs;
}

bool elgamal_verify_hash(const std::vector<unsigned char>& hash, const std::vector<ElGamalSignature>& sigs, const ElGamalKey& key) {
    if (hash.size() != sigs.size()) {
        std::cout << "Hash and signature size mismatch: " << hash.size() << " vs " << sigs.size() << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < hash.size(); ++i) {
        u64 m = hash[i];
        u64 r = sigs[i].r;
        u64 s = sigs[i].s;
        
        // Проверяем границы
        if (r == 0 || r >= key.p || s == 0 || s >= key.p - 1) {
            std::cout << "Invalid signature bounds at position " << i << std::endl;
            return false;
        }
        
        // Проверяем: g^m ≡ y^r * r^s (mod p)
        u64 left = mod_exp(key.g, m, key.p);
        u64 right1 = mod_exp(key.y, r, key.p);
        u64 right2 = mod_exp(r, s, key.p);
        u64 right = (right1 * right2) % key.p;
        
        if (left != right) {
            std::cout << "Verification failed at byte " << i << std::endl;
            std::cout << "Hash byte: " << m << std::endl;
            std::cout << "Left: " << left << std::endl;
            std::cout << "Right: " << right << std::endl;
            std::cout << "Right1: " << right1 << std::endl;
            std::cout << "Right2: " << right2 << std::endl;
            std::cout << "r: " << r << ", s: " << s << std::endl;
            return false;
        }
    }
    return true;
}

void elgamal_sign_file(const std::string& filename) {
    std::cout << "Computing SHA256 hash..." << std::endl;
    auto hash = sha256_file(filename);
    std::cout << "Hash computed, length: " << hash.size() << " bytes" << std::endl;
    
    std::cout << "Generating ElGamal keys..." << std::endl;
    u64 p = find_prime(1000000);
    u64 g = find_primitive_root(p);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<u64> dis(1, p - 2);
    u64 x = dis(gen);
    u64 y = mod_exp(g, x, p);
    
    ElGamalKey key{p, g, x, y};
    std::cout << "Keys generated: p=" << p << ", g=" << g << ", y=" << y << std::endl;

    std::cout << "Saving keys..." << std::endl;
    save_elgamal_keys(key);
    
    std::cout << "Signing hash..." << std::endl;
    auto sigs = elgamal_sign_hash(hash, key);
    std::cout << "Generated " << sigs.size() << " signature pairs" << std::endl;

    // Сохраняем подпись
    std::vector<unsigned char> out;
    for (auto& sig : sigs) {
        for (int i = 0; i < 8; ++i) out.push_back((sig.r >> (i * 8)) & 0xFF);
        for (int i = 0; i < 8; ++i) out.push_back((sig.s >> (i * 8)) & 0xFF);
    }
    
    write_file(filename + "_sig", out);
    std::cout << "ElGamal signature saved to " << filename + "_sig" << std::endl;
}

void elgamal_verify_file(const std::string& filename) {
    std::cout << "Computing SHA256 hash..." << std::endl;
    auto hash = sha256_file(filename);
    std::cout << "Hash computed, length: " << hash.size() << " bytes" << std::endl;
    
    std::cout << "Loading signature..." << std::endl;
    auto raw_sig = read_file(filename + "_sig");
    std::cout << "Signature file size: " << raw_sig.size() << " bytes" << std::endl;

    if (raw_sig.size() % 16 != 0) {
        std::cout << "Invalid signature size: " << raw_sig.size() << " (not divisible by 16)" << std::endl;
        return;
    }

    std::vector<ElGamalSignature> sigs;
    for (size_t i = 0; i < raw_sig.size(); i += 16) {
        u64 r = 0, s = 0;
        for (int j = 0; j < 8; ++j) r |= (u64(raw_sig[i + j]) << (j * 8));
        for (int j = 0; j < 8; ++j) s |= (u64(raw_sig[i + 8 + j]) << (j * 8));
        sigs.push_back({r, s});
    }
    std::cout << "Loaded " << sigs.size() << " signature pairs" << std::endl;

    std::cout << "Loading public key..." << std::endl;
    auto key = load_elgamal_keys(true);
    std::cout << "Public key loaded: p=" << key.p << ", g=" << key.g << ", y=" << key.y << std::endl;
    
    std::cout << "Verifying signature..." << std::endl;
    bool ok = elgamal_verify_hash(hash, sigs, key);

    std::ofstream out(filename + "_ver");
    out << (ok ? "VALID" : "INVALID");
    std::cout << "ElGamal verification: " << (ok ? "VALID" : "INVALID") << std::endl;
}