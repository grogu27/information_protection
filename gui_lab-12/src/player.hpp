#pragma once
#include <vector>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>

using cpp_int = boost::multiprecision::cpp_int;

class Player {
public:
    int id;
    cpp_int p;      // общее простое
    cpp_int d, e;   // secret & public key

    std::vector<cpp_int> hand_enc;   // полученные зашифрованные карты
    std::vector<cpp_int> hand_plain; // расшифрованные

    Player(int playerId, cpp_int prime, cpp_int secret_d);

    // Шифрует + перемешивает
    std::vector<cpp_int> encryptAndShuffle(const std::vector<cpp_int>& deck);

    // Частичная расшифровка (для общего протокола)
    cpp_int partialDecrypt(cpp_int c);

    // Полная расшифровка — применяет ВСЕ ключи по цепочке (для post-hoc)
    cpp_int fullDecrypt(cpp_int encrypted, const std::vector<cpp_int>& all_d);
};