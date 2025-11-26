// src/utils.hpp
#pragma once
#include <boost/multiprecision/cpp_int.hpp>
using boost::multiprecision::cpp_int;

inline cpp_int mod_exp(cpp_int base, cpp_int exp, cpp_int mod) {
    if (mod == 1) return 0;
    cpp_int result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

inline cpp_int mod_inverse(cpp_int a, cpp_int m) {
    cpp_int m0 = m, t, q;
    cpp_int x0 = 0, x1 = 1;
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