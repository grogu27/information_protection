#include <iostream>
#include "src//crypto.hpp"

int main() {
    int choice;
    while (true) {
        std::cout << "\n=== Криптографическая библиотека ===\n";
        std::cout << "1. (Лаб.1) Быстрое возведение в степень\n";
        std::cout << "2. (Лаб.1) Тест Ферма\n";
        std::cout << "3. (Лаб.1) Расширенный Евклид\n";
        std::cout << "4. (Лаб.2) Дискретный логарифм (BSGS)\n";
        std::cout << "5. (Лаб.3) Diffie-Hellman\n";
        std::cout << "6. (Лаб.4) Шифр Шамира\n";
        std::cout << "7. (Лаб.5) Шифр Эль-Гамаля\n";  
        std::cout << "0. Выход\n";
        std::cout << "Выбор: ";
        std::cin >> choice;

        if (choice == 0) break;

        if (choice == 1) {
            long long a, x, p;
            std::cout << "a x p: "; std::cin >> a >> x >> p;
            std::cout << "a^x mod p = " << mod_pow(a, x, p) << "\n";
        }
        else if (choice == 2) {
            long long n; int k;
            std::cout << "n k: "; std::cin >> n >> k;
            std::cout << n << (is_prime_fermat(n, k) ? " — вероятно простое\n" : " — составное\n");
        }
        else if (choice == 3) {
            long long a, b;
            std::cout << "a b: "; std::cin >> a >> b;
            auto [g, x, y] = extended_gcd2(a, b);
            std::cout << "gcd=" << g << ", x=" << x << ", y=" << y << "\n";
        }
        else if (choice == 4) {
            int mode; std::cout << "1. Вручную 2. Авто: "; std::cin >> mode;
            if (mode == 1) {
                long long a, y, p;
                std::cout << "a y p: "; std::cin >> a >> y >> p;
                long long x = baby_step_giant_step(a, y, p);
                std::cout << "x = " << (x == -1 ? "не найден" : std::to_string(x)) << "\n";
            } else {
                auto [a, y, p, x] = bsgs_with_random_params();
                if (a == -1) std::cout << "Ошибка генерации\n";
                else std::cout << "a=" << a << " y=" << y << " p=" << p << " x=" << x << "\n";
            }
        }
        else if (choice == 5) {
            int mode; std::cout << "1. Вручную 2. Авто: "; std::cin >> mode;
            if (mode == 1) diffie_hellman_manual();
            else diffie_hellman_auto();
        }
        else if (choice == 6) {
            lab4_shamir();
        }
        else if (choice == 7) {
            lab5_elgamal();
        }
        else std::cout << "Неверный выбор.\n";
    }
    return 0;
}
