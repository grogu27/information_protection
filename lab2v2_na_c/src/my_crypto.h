#ifndef MY_CRYPTO_H
#define MY_CRYPTO_H

#include <stdint.h>

// 1) Быстрое возведение в степень по модулю
long long mod_pow(long long a, long long x, long long p);

// 2) Тест простоты Ферма
int is_prime_fermat(long long n, int k);

// 3) Расширенный алгоритм Евклида
long long extended_gcd(long long a, long long b, long long *x, long long *y);
long long extended_gcd2(long long a, long long b, long long *x, long long *y);

// Генерация случайных чисел
void random_ab(long long *a, long long *b);
void random_prime_ab(long long *a, long long *b);

long long discrete_log(long long a, long long y, long long p); //O(sqrt(p) * log p)

#endif // MY_CRYPTO_H
