// // Вспомогательная функция: модульное обратное
// long long mod_inverse(long long a, long long m) {
//     auto [g, x, y] = extended_gcd2(a, m);
//     if (g != 1) return -1;
//     x %= m;
//     if (x < 0) x += m;
//     return x;
// }

// // Генерация простого > 256 (уже определена выше, но оставляем объявление здесь)
// // long long generate_prime_for_crypto();  -- определено раньше

// // =============== ЛАБА 4: АФФИННЫЙ ШИФР ("Шамир") ===============
// bool shamir_process_file(const std::string& in_file, const std::string& out_file,
//                          long long C1, long long C2, long long p, bool decrypt) {
//     std::ifstream in(in_file, std::ios::binary);
//     std::ofstream out(out_file, std::ios::binary);
//     if (!in || !out) return false;

//     if (decrypt) {
//         long long inv = mod_inverse(C1, p);
//         if (inv == -1) return false;
//         char byte;
//         while (in.read(&byte, 1)) {
//             long long y = static_cast<unsigned char>(byte);
//             long long x = ((y - C2 + p) % p * inv) % p;
//             if (x > 255) return false;
//             out.put(static_cast<char>(x));
//         }
//     } else {
//         char byte;
//         while (in.read(&byte, 1)) {
//             long long x = static_cast<unsigned char>(byte);
//             long long y = (C1 * x + C2) % p;
//             if (y > 255) {
//                 std::cerr << "Ошибка: p слишком мало, y = " << y << " > 255\n";
//                 return false;
//             }
//             out.put(static_cast<char>(y));
//         }
//     }
//     return true;
// }

// void lab4_shamir() {
//     int mode;
//     std::cout << "\n--- Лабораторная №4: Аффинный шифр (\"Шамир\") ---\n";
//     std::cout << "1. Ввести p, C1, C2 вручную\n";
//     std::cout << "2. Сгенерировать параметры\n";
//     std::cin >> mode;

//     long long p, C1, C2;
//     if (mode == 1) {
//         std::cout << "p (простое, >256): "; std::cin >> p;
//         std::cout << "C1 (1 <= C1 < p, gcd(C1,p)=1): "; std::cin >> C1;
//         std::cout << "C2 (0 <= C2 < p): "; std::cin >> C2;
//         if (C1 <= 0 || C1 >= p || C2 < 0 || C2 >= p) {
//             std::cerr << "Неверные границы!\n"; return;
//         }
//         if (mod_inverse(C1, p) == -1) {
//             std::cerr << "C1 не обратим по модулю p!\n"; return;
//         }
//     } else {
//         p = generate_prime_for_crypto();
//         std::random_device rd;
//         std::mt19937_64 gen(rd());
//         std::uniform_int_distribution<long long> dist(1, p - 1);
//         C1 = dist(gen);
//         while (mod_inverse(C1, p) == -1) C1 = dist(gen);
//         C2 = dist(gen) % p;
//         std::cout << "Сгенерировано: p=" << p << ", C1=" << C1 << ", C2=" << C2 << "\n";
//     }

//     std::string in_file, out_file;
//     std::cout << "Входной файл: "; std::cin >> in_file;
//     std::cout << "Выходной файл: "; std::cin >> out_file;

//     int op; std::cout << "1. Шифровать  2. Расшифровать: "; std::cin >> op;
//     if (op == 1) {
//         if (shamir_process_file(in_file, out_file, C1, C2, p, false))
//             std::cout << "Успешно зашифровано.\n";
//         else
//             std::cerr << "Ошибка шифрования.\n";
//     } else if (op == 2) {
//         if (shamir_process_file(in_file, out_file, C1, C2, p, true))
//             std::cout << "Успешно расшифровано.\n";
//         else
//             std::cerr << "Ошибка расшифровки.\n";
//     }
// }