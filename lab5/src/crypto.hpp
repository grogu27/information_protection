// crypto.hpp
#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <utility>

// Лаб 1: Базовые функции
long long mod_pow(long long a, long long x, long long p);
bool is_prime_fermat(long long n, int k = 20);
std::tuple<long long, long long, long long> extended_gcd2(long long a, long long b);
std::pair<long long, long long> random_ab();
std::pair<long long, long long> random_prime_ab();

// Лаб 2: BSGS
long long baby_step_giant_step(long long a, long long y, long long p);
std::tuple<long long, long long, long long, long long> bsgs_with_random_params();

// Лаб 3: Диффи-Хеллман
void diffie_hellman_manual();
void diffie_hellman_auto();

// Лаб 4: Шамир
void lab4_shamir();
std::pair<long long, long long> generate_shamir_keys(long long p);
bool shamir_three_pass_step(const std::string& in_file, const std::string& out_file,
                           long long exp, long long p);

// Лаб 5: Эль-Гамаль
void elgamal_encrypt_file(const std::string& input_file, const std::string& output_file,
                         long long p, long long g, long long y);
void elgamal_decrypt_file(const std::string& input_file, const std::string& output_file,
                         long long p, long long x);
void lab5_elgamal();

// Вспомогательные функции
long long mod_inverse(long long a, long long m);
void write_long(std::ofstream& out, long long val);
bool read_long(std::ifstream& in, long long &val);
long long generate_prime_for_crypto();
long long find_primitive_root(long long p);

#endif