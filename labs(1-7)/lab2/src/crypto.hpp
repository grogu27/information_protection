#ifndef CRYPTO_H
#define CRYPTO_H

#include <cstdint>
#include <utility>
#include <vector>

// 1) Быстрое возведение в степень по модулю   y=a^x mod p    (p - простое число, x - от 1 до p-1, )
long long mod_pow(long long a, long long x, long long p);

// 2) Тест простоты Ферма
//    k - количество итераций                   p = 1 - 1/2^k              a^(p-1)modp = 1
//Позволяет быстро отбрасывать составные числа без полного перебора делителей.
bool is_prime_fermat(long long n, int k);

// 3) Расширенный алгоритм Евклида
//    Возвращает НОД, а также коэффициенты x, y такие что a*x + b*y = gcd(a, b)
long long extended_gcd(long long a, long long b, long long &x, long long &y);
std::tuple<long long, long long, long long> extended_gcd2(long long a, long long b);

//std::pair<long long, long long> input_ab();         
std::pair<long long, long long> random_ab();        
std::pair<long long, long long> random_prime_ab();  

// 4) Решение дискретного логарифма методом "Шаг младенца, шаг великана"
//    Решает уравнение: y = a^x mod p, возвращает x
//    Если решения нет — возвращает -1
long long baby_step_giant_step(long long a, long long y, long long p);

// Дополнительно: с возможностью генерации случайных параметров
std::tuple<long long, long long, long long, long long> bsgs_with_random_params();

#endif // CRYPTO_H
