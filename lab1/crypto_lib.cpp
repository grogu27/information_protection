#include "crypto_lib.h"
#include <algorithm>
#include <cmath>

CryptoLibrary::CryptoLibrary() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}

// 1. Быстрое возведение в степень по модулю
long long CryptoLibrary::mod_pow(long long base, long long exponent, long long modulus) {
    if (modulus == 1) return 0;
    
    long long result = 1;
    base = base % modulus;
    
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }
        exponent = exponent >> 1;
        base = (base * base) % modulus;
    }
    
    return result;
}

// 2. Тест простоты Ферма
bool CryptoLibrary::fermat_primality_test(long long n, int k) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0) return false;
    
    for (int i = 0; i < k; i++) {
        long long a = 2 + rng() % (n - 3);
        if (mod_pow(a, n - 1, n) != 1) {
            return false;
        }
    }
    
    return true;
}

// 3. Обобщенный алгоритм Евклида
CryptoLibrary::EGCDResult CryptoLibrary::extended_gcd(long long a, long long b) {
    if (a == 0) {
        return {b, 0, 1};
    }
    
    EGCDResult result = extended_gcd(b % a, a);
    long long x = result.y - (b / a) * result.x;
    long long y = result.x;
    
    return {result.gcd, x, y};
}

// Генерация случайного числа в диапазоне
long long CryptoLibrary::generate_random_number(long long min, long long max) {
    std::uniform_int_distribution<long long> dist(min, max);
    return dist(rng);
}

// Генерация простого числа
long long CryptoLibrary::generate_prime(long long min, long long max) {
    while (true) {
        long long candidate = generate_random_number(min, max);
        if (fermat_primality_test(candidate, 20)) {
            return candidate;
        }
    }
}

// Ввод с клавиатуры
std::pair<long long, long long> CryptoLibrary::input_from_keyboard() {
    long long a, b;
    std::cout << "Введите число a: ";
    std::cin >> a;
    std::cout << "Введите число b: ";
    std::cin >> b;
    return {a, b};
}

// Автоматическая генерация чисел
std::pair<long long, long long> CryptoLibrary::auto_generate_numbers() {
    long long a = generate_random_number(1000, 1000000);
    long long b = generate_random_number(1000, 1000000);
    return {a, b};
}

// Автоматическая генерация простых чисел
std::pair<long long, long long> CryptoLibrary::auto_generate_primes() {
    long long a = generate_prime(10000, 1000000);
    long long b = generate_prime(10000, 1000000);
    return {a, b};
}

// Вспомогательные функции для работы со строками (для очень больших чисел)

// Умножение строковых чисел
std::string multiply_strings(const std::string& num1, const std::string& num2) {
    int n1 = num1.length(), n2 = num2.length();
    if (n1 == 0 || n2 == 0) return "0";
    
    std::vector<int> result(n1 + n2, 0);
    
    for (int i = n1 - 1; i >= 0; i--) {
        for (int j = n2 - 1; j >= 0; j--) {
            int mul = (num1[i] - '0') * (num2[j] - '0');
            int p1 = i + j, p2 = i + j + 1;
            int sum = mul + result[p2];
            
            result[p2] = sum % 10;
            result[p1] += sum / 10;
        }
    }
    
    std::string str = "";
    for (int i = 0; i < result.size(); i++) {
        if (!(str.empty() && result[i] == 0)) {
            str += std::to_string(result[i]);
        }
    }
    
    return str.empty() ? "0" : str;
}

// Сложение строковых чисел
std::string add_strings(const std::string& num1, const std::string& num2) {
    std::string result = "";
    int carry = 0;
    int i = num1.length() - 1;
    int j = num2.length() - 1;
    
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += num1[i--] - '0';
        if (j >= 0) sum += num2[j--] - '0';
        
        result = char(sum % 10 + '0') + result;
        carry = sum / 10;
    }
    
    return result;
}

// Проверка, что num1 >= num2
bool is_greater_or_equal(const std::string& num1, const std::string& num2) {
    if (num1.length() != num2.length()) {
        return num1.length() > num2.length();
    }
    return num1 >= num2;
}