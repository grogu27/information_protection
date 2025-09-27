#ifndef CRYPTO_H
#define CRYPTO_H

#include <cstdint>
#include <utility>
#include <tuple>
#include <string>
#include <vector>

// Лаба 1
long long mod_pow(long long a, long long x, long long p);
bool is_prime_fermat(long long n, int k = 20);
std::tuple<long long, long long, long long> extended_gcd2(long long a, long long b);
std::pair<long long, long long> random_ab();
std::pair<long long, long long> random_prime_ab();

// Лаба 2
long long baby_step_giant_step(long long a, long long y, long long p);
std::tuple<long long, long long, long long, long long> bsgs_with_random_params();
std::vector<long long> get_prime_factors(long long n);
bool is_primitive_root(long long g, long long p);
long long find_primitive_root(long long p);

// Лаба 3
void diffie_hellman_manual();
void diffie_hellman_auto();

// Вспомогательные
long long mod_inverse(long long a, long long m);

// Лаба 4: Аффинный шифр (называем "Шамир" по условию)
void lab4_shamir();

// Лаба 5: Эль-Гамаль
void lab5_elgamal();

// Лаба 6: RSA
void lab6_rsa();

#endif // CRYPTO_H