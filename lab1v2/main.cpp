#include <iostream>
//#include "src/crypto.hpp"
#include "src/crypto.hpp"

int main() {
    // 1) Быстрое возведение в степень
    std::cout << "--- Быстрое возведение в степень ---\n";
    long long a, x, p;
    std::cout << "Введите a, x, p: ";
    std::cin >> a >> x >> p;
    std::cout << "Результат: y = " << mod_pow(a, x, p) << "\n\n";

    // 2) Тест простоты Ферма
    std::cout << "--- Тест простоты Ферма ---\n";
    long long n;
    std::cout << "Введите число для проверки: ";
    std::cin >> n;
    if (is_prime_fermat(n, 100))
        std::cout << n << " вероятно простое\n\n";
    else
        std::cout << n << " составное\n\n";

    // 3) Расширенный алгоритм Евклида
    std::cout << "--- Расширенный алгоритм Евклида (ручной ввод) ---\n";
    long long a1, b1;
    std::cout << "Введите a1 и b1: ";
    std::cin >> a1 >> b1;
    auto [gcd1, x1, y1]= extended_gcd2(a1, b1);
    std::cout << "НОД(" << a1 << ", " << b1 << ") = " << gcd1 << "\n";
    std::cout << "Уравнение: " << a1 << "*" << x1 << " + " << b1 << "*" << y1 << " = " << gcd1 << "\n\n";

    // Вариант 2: случайные a, b
    std::cout << "--- Расширенный алгоритм Евклида (random_ab) ---\n";
    auto [a2, b2] = random_ab();
    auto [gcd2, x2, y2] = extended_gcd2(a2, b2);
    std::cout << "Случайные числа: a=" << a2 << ", b=" << b2 << "\n";
    std::cout << "НОД(" << a2 << ", " << b2 <<  ") = " << gcd2 << "\n";
    std::cout << "Уравнение: " << a2 << "*" << x2 << " + " << b2 << "*" << y2 << " = " << gcd2 << "\n\n";

    // Вариант 3: случайные простые a, b
    std::cout << "--- Расширенный алгоритм Евклида (random_prime_ab) ---\n";
    auto [a3, b3] = random_prime_ab();
    //long long x3, y3;
    //long long g3 = extended_gcd(a3, b3, x3, y3);
    auto [gcd3, x3, y3] = extended_gcd2(a3, b3);
    std::cout << "Случайные простые числа: a=" << a3 << ", b=" << b3 << "\n";
    std::cout << "НОД(" << a3 << ", " << b3 << ") = " << gcd3 << "\n";
    std::cout << "Уравнение: " << a3 << "*" << x3 << " + " << b3 << "*" << y3 << " = " << gcd3 << "\n\n";

    return 0;
}
