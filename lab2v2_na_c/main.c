#include <stdio.h>
#include "src/my_crypto.h"
#include <time.h>
#include <stdlib.h>
//#include <my_crypto.h>

int main() {
    //srand(time(NULL));
    // 1) Быстрое возведение в степень
    printf("--- Быстрое возведение в степень ---\n");
    long long a, x, p;
    printf("Введите a, x, p: ");
    scanf("%lld %lld %lld", &a, &x, &p);
    printf("Результат: y = %lld\n\n", mod_pow(a, x, p));

    // 2) Тест простоты Ферма
    printf("--- Тест простоты Ферма ---\n");
    long long n;
    printf("Введите число для проверки: ");
    scanf("%lld", &n);
    if (is_prime_fermat(n, 100))
        printf("%lld вероятно простое\n\n", n);
    else
        printf("%lld составное\n\n", n);

    // 3) Расширенный алгоритм Евклида (ручной ввод)
    printf("--- Расширенный алгоритм Евклида (ручной ввод) ---\n");
    long long a1, b1, x1, y1;
    printf("Введите a1 и b1: ");
    scanf("%lld %lld", &a1, &b1);
    long long g1 = extended_gcd2(a1, b1, &x1, &y1);
    printf("НОД(%lld, %lld) = %lld\n", a1, b1, g1);
    printf("Уравнение: %lld*%lld + %lld*%lld = %lld\n\n", a1, x1, b1, y1, g1);

    // случайные a, b
    printf("--- Расширенный алгоритм Евклида (random_ab) ---\n");
    long long a2, b2, x2, y2;
    random_ab(&a2, &b2);
    long long g2 = extended_gcd2(a2, b2, &x2, &y2);
    printf("Случайные числа: a=%lld, b=%lld\n", a2, b2);
    printf("НОД(%lld, %lld) = %lld\n\n", a2, b2, g2);

    // случайные простые a, b
    printf("--- Расширенный алгоритм Евклида (random_prime_ab) ---\n");
    long long a3, b3, x3, y3;
    random_prime_ab(&a3, &b3);
    long long g3 = extended_gcd2(a3, b3, &x3, &y3);
    printf("Случайные простые числа: a=%lld, b=%lld\n", a3, b3);
    printf("НОД(%lld, %lld) = %lld\n", a3, b3, g3);



    // ----LAB2----
    long long a10, y10, p10;
    printf("\n\n--- Шаг младенца, шаг великана ---\n");
    printf("Введите a, y, p: ");
    scanf("%lld %lld %lld", &a10, &y10, &p10);

    long long x10 = discrete_log(a10, y10, p10);
    if (x10 == -1)
        printf("Решение не найдено\n");
    else
        printf("x = %lld, проверка: %lld^%lld mod %lld = %lld\n",
               x10, a10, x10, p10, mod_pow(a10, x10, p10));

    return 0;
}
