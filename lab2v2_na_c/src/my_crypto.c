#include "my_crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PAIR_SIZE 100003

int rand_initialized = 0;

int my_rand() {
    if (rand_initialized == 0) {
        srand(time(NULL));  
        rand_initialized = 1;
    }
    return rand();
}

// быстрое возведение в степень
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

// расширенный алгоритм Евклида (рекурсивный)
long long extended_gcd(long long a, long long b, long long *x, long long *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long x1, y1;
    long long gcd = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return gcd;
}

// расширенный алгоритм Евклида (итеративный)
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

// тест Ферма
int is_prime_fermat(long long n, int k) {
    if (n < 4) return (n == 2 || n == 3);

    for (int i = 0; i < k; i++) {
        long long a = 2 + my_rand() % (n - 3);
        long long x, y;
        if (extended_gcd2(a, n, &x, &y) != 1) return 0;
        if (mod_pow(a, n - 1, n) != 1) return 0;
    }
    return 1;
}


void random_ab(long long *a, long long *b) {
    *a = 2 + my_rand() % 1000000;
    *b = 2 + my_rand() % 1000000;
}

void random_prime_ab(long long *a, long long *b) {
    long long candidate;
    // генерируем первое простое
    while (1) {
        candidate = 100 + my_rand() % 100000;
        if (is_prime_fermat(candidate, 50)) {
            *a = candidate;
            break;
        }
    }

    // do {
    // candidate = 100 + rand() % 100000;
    // } while (!is_prime_fermat(candidate, 50));
    // *a = candidate;

    // генерируем второе простое
    while (1) {
        candidate = 100 + my_rand() % 100000;
        if (candidate != *a && is_prime_fermat(candidate, 50)) {
            *b = candidate;
            break;
        }
    }

    // do {
    // candidate = 100 + rand() % 100000;
    // } while (candidate == *a || !is_prime_fermat(candidate, 50));
    // *b = candidate;

}
    typedef struct {
        long long key;
        long long value;
    } Pair;

    static Pair pairs[PAIR_SIZE];

    static void pair_clear() {
        for (int i = 0; i < PAIR_SIZE; i++)
            pairs[i].key = -1;
    }

    static void pair_insert(long long k, long long v) {
        long long idx = k % PAIR_SIZE;
        while (pairs[idx].key != -1) idx = (idx + 1) % PAIR_SIZE;
        pairs[idx].key = k;
        pairs[idx].value = v;
    }

    static long long pair_find(long long k) {
        long long idx = k % PAIR_SIZE;
        while (pairs[idx].key != -1) {
            if (pairs[idx].key == k)
                return pairs[idx].value;
            idx = (idx + 1) % PAIR_SIZE;
        }
        return -1;  // не найдено
    }

    long long discrete_log(long long a, long long y, long long p) {
        long long n = (long long)sqrt(p) + 1;

        pair_clear();

        // Шаг младенца: a^j mod p
        long long aj = 1;
        for (long long j = 0; j < n; j++) {
            pair_insert(aj, j);
            aj = (aj * a) % p;
        }

        // a^(-n) mod p
        long long inv_a = a, x, y0;
        extended_gcd2(a, p, &x, &y0);
        if (x < 0) x += p;
        inv_a = mod_pow(x, n, p); // a^(-n) mod p

        long long gamma = y;
        for (long long i = 0; i <= n; i++) {
            long long j = pair_find(gamma);
            if (j != -1) {
                return i * n + j;
            }
            gamma = (gamma * inv_a) % p;
        }

        return -1; // решения нет
    }

