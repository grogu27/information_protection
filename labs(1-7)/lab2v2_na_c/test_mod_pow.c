#include "src/my_crypto.h"
#include <stdio.h>
#include <time.h>

int main() {
    long long a = 7;
    long long x = 100000000;
    long long p = 1000000007;

    printf("Тест: a=%lld, x=%lld, p=%lld\n", a, x, p);

    clock_t start = clock();
    long long y_fast = mod_pow(a, x, p);
    clock_t end = clock();
    double t_fast = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Быстрое возведение: y = %lld (время: %.6f сек)\n", y_fast, t_fast);

    start = clock();
    long long y_std = 1;
    for (long long i = 0; i < x; i++) {
        y_std = (y_std * a) % p;
    }
    end = clock();
    double t_std = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Обычное возведение: y = %lld (время: %.6f сек)\n", y_std, t_std);

    if (y_fast == y_std)
        printf("Результаты совпадают\n");
    else
        printf("Результаты не совпадают\n");

    return 0;
}
