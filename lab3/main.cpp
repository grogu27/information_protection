#include <iostream>
//#include "src/crypto.hpp"
#include "src/crypto.hpp"

int main() {
    // // 1) Быстрое возведение в степень
    // std::cout << "--- Быстрое возведение в степень ---\n";
    // long long a, x, p;
    // std::cout << "Введите a, x, p: ";
    // std::cin >> a >> x >> p;
    // std::cout << "Результат: y = " << mod_pow(a, x, p) << "\n\n";

    // // 2) Тест простоты Ферма
    // std::cout << "--- Тест простоты Ферма ---\n";
    // long long n;
    // std::cout << "Введите число для проверки: ";
    // std::cin >> n;
    // if (is_prime_fermat(n, 100))
    //     std::cout << n << " вероятно простое\n\n";
    // else
    //     std::cout << n << " составное\n\n";

    // // 3) Расширенный алгоритм Евклида
    // std::cout << "--- Расширенный алгоритм Евклида (ручной ввод) ---\n";
    // long long a1, b1;
    // std::cout << "Введите a1 и b1: ";
    // std::cin >> a1 >> b1;
    // auto [gcd1, x1, y1]= extended_gcd2(a1, b1);
    // std::cout << "НОД(" << a1 << ", " << b1 << ") = " << gcd1 << "\n";
    // std::cout << "Уравнение: " << a1 << "*" << x1 << " + " << b1 << "*" << y1 << " = " << gcd1 << "\n\n";

    // // Вариант 2: случайные a, b
    // std::cout << "--- Расширенный алгоритм Евклида (random_ab) ---\n";
    // auto [a2, b2] = random_ab();
    // auto [gcd2, x2, y2] = extended_gcd2(a2, b2);
    // std::cout << "Случайные числа: a=" << a2 << ", b=" << b2 << "\n";
    // std::cout << "НОД(" << a2 << ", " << b2 <<  ") = " << gcd2 << "\n";
    // std::cout << "Уравнение: " << a2 << "*" << x2 << " + " << b2 << "*" << y2 << " = " << gcd2 << "\n\n";

    // // Вариант 3: случайные простые a, b
    // std::cout << "--- Расширенный алгоритм Евклида (random_prime_ab) ---\n";
    // auto [a3, b3] = random_prime_ab();
    // //long long x3, y3;
    // //long long g3 = extended_gcd(a3, b3, x3, y3);
    // auto [gcd3, x3, y3] = extended_gcd2(a3, b3);
    // std::cout << "Случайные простые числа: a=" << a3 << ", b=" << b3 << "\n";
    // std::cout << "НОД(" << a3 << ", " << b3 << ") = " << gcd3 << "\n";
    // std::cout << "Уравнение: " << a3 << "*" << x3 << " + " << b3 << "*" << y3 << " = " << gcd3 << "\n\n";
    // // 4) Дискретный логарифм: Шаг младенца, шаг великана
    // std::cout << "--- Дискретный логарифм (Baby-step Giant-step) ---\n";
    // int choice;
    // std::cout << "Выберите режим:\n";
    // std::cout << "1. Ввести a, y, p вручную\n";
    // std::cout << "2. Сгенерировать случайные параметры\n";
    // std::cin >> choice;

    // // 4) Дискретный логарифм: Шаг младенца, шаг великана
    // std::cout << "--- Дискретный логарифм (Baby-step Giant-step) ---\n";
    // int choice;
    // std::cout << "Выберите режим:\n";
    // std::cout << "1. Ввести a, y, p вручную\n";
    // std::cout << "2. Сгенерировать случайные параметры\n";
    // std::cin >> choice;

    // if (choice == 1) {
    //     long long a, y, p;
    //     std::cout << "Введите a, y, p: ";
    //     std::cin >> a >> y >> p;

    //     long long x = baby_step_giant_step(a, y, p);
    //     if (x == -1) {
    //         std::cout << "Решение не найдено.\n";
    //     } else {
    //         std::cout << "Найдено x = " << x << "\n";
    //         // Проверка
    //         if (mod_pow(a, x, p) == y % p) {
    //             std::cout << "Проверка: a^x mod p = y — верно!\n";
    //         } else {
    //             std::cout << "Ошибка: a^x mod p != y\n";
    //         }
    //     }
    // } else if (choice == 2) {
    //     auto [a, y, p, x_found] = bsgs_with_random_params();
    //     if (a == -1) {
    //         std::cout << "Не удалось сгенерировать параметры.\n";
    //     } else {
    //         std::cout << "Сгенерировано:\n";
    //         std::cout << "   a = " << a << "\n";
    //         std::cout << "   y = " << y << "\n";
    //         std::cout << "   p = " << p << "\n";
    //         std::cout << "Найдено x = " << x_found << "\n";

    //         if (x_found != -1) {
    //             if (mod_pow(a, x_found, p) == y % p) {
    //                 std::cout << "Проверка пройдена: a^x mod p = y\n";
    //             } else {
    //                 std::cout << "Ошибка в вычислениях!\n";
    //             }
    //         }
    //     }
    // } else {
    //     std::cout << "Неверный выбор. Пропускаем тест дискретного логарифма.\n";
    // }

    //   --- Diffie-Hellman Key Exchange (ручной ввод) ---
    // Введите простое число p: 23
    // Введите примитивный корень g по модулю p: 5
    // Введите секретный ключ X_A первого абонента (1 <= X_A <= p-2): 6
    // Введите секретный ключ X_B второго абонента (1 <= X_B <= p-2): 15

    // --- Результаты ---
    // Открытый ключ A: Y_A = 8
    // Открытый ключ B: Y_B = 19
    // Общий ключ (вычислен A): K = 2
    // Общий ключ (вычислен B): K = 2
    // ✅ Общий ключ успешно согласован: K = 2

    // 5) Diffie-Hellman Key Exchange
    std::cout << "\n\n--- Diffie-Hellman Key Exchange ---\n";
    int dh_choice;
    std::cout << "Выберите режим:\n";
    std::cout << "1. Ввести p, g, X_A, X_B вручную\n";
    std::cout << "2. Сгенерировать параметры автоматически\n";
    std::cin >> dh_choice;

    if (dh_choice == 1) {
        diffie_hellman_manual();
    } else if (dh_choice == 2) {
        diffie_hellman_auto();
    } else {
        std::cout << "Неверный выбор. Пропускаем Diffie-Hellman.\n";
    }

    return 0;
}
