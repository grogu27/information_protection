#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    long long key;
    long long value;
} Pair;

int compare_pair(const void *a, const void *b) {
    const Pair *pa = (const Pair *)a;
    const Pair *pb = (const Pair *)b;
    if (pa->key < pb->key) return -1;
    if (pa->key > pb->key) return 1;
    return 0;
}

long long binary_search(Pair *arr, int size, long long key) {
    int left = 0, right = size - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid].key == key) return arr[mid].value;
        if (arr[mid].key < key) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// Быстрое возведение в степень по модулю
long long mod_pow(long long a, long long x, long long p) {
    long long result = 1;
    a %= p;
    while (x > 0) {
        if (x & 1) result = (result * a) % p;
        a = (a * a) % p;
        x >>= 1;
    }
    return result;
}

// Расширенный алгоритм Евклида для обратного элемента
long long extended_gcd2(long long a, long long b, long long *x, long long *y) {
    long long u1 = a, u2 = 1, u3 = 0;
    long long v1 = b, v2 = 0, v3 = 1;

    while (v1 != 0) {
        long long q = u1 / v1;
        long long t1 = u1 % v1;
        long long t2 = u2 - q * v2;
        long long t3 = u3 - q * v3;

        u1 = v1; u2 = v2; u3 = v3;
        v1 = t1; v2 = t2; v3 = t3;
    }

    *x = u2;
    *y = u3;
    return u1;
}

// Алгоритм "Шаг младенца, шаг великана"
long long discrete_log(long long a, long long y, long long p) {
    long long n = (long long)sqrt(p) + 1;
    Pair *pairs = (Pair *)malloc(n * sizeof(Pair));
    if (!pairs) return -1;

    // Шаг младенца: сохраняем a^j mod p
    long long aj = 1;
    for (long long j = 0; j < n; j++) {
        pairs[j].key = aj;
        pairs[j].value = j;
        aj = (aj * a) % p;
    }

    // Сортируем массив по ключу для бинарного поиска
    qsort(pairs, n, sizeof(Pair), compare_pair);

    // Вычисляем a^(-n) mod p
    long long x, y0;
    extended_gcd2(a, p, &x, &y0);
    if (x < 0) x += p;
    long long inv_a = mod_pow(x, n, p);

    long long gamma = y;
    for (long long i = 0; i <= n; i++) {
        long long j = binary_search(pairs, n, gamma);
        if (j != -1) {
            free(pairs);
            return i * n + j;
        }
        gamma = (gamma * inv_a) % p;
    }

    free(pairs);
    return -1; // решения нет
}