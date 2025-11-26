#include "player.hpp"
#include "utils.hpp"
#include <algorithm>
#include <random>

Player::Player(int playerId, cpp_int prime, cpp_int secret_d)
    : id(playerId), p(prime), d(secret_d) {
    e = mod_inverse(d, p - 1);
}

std::vector<cpp_int> Player::encryptAndShuffle(const std::vector<cpp_int>& deck) {
    std::vector<cpp_int> out;
    out.reserve(deck.size());
    for (auto& c : deck) {
        out.push_back(mod_exp(c, e, p));
    }
    std::random_device rd;
    std::mt19937 g(rd() + id);
    std::shuffle(out.begin(), out.end(), g);
    return out;
}

cpp_int Player::partialDecrypt(cpp_int c) {
    return mod_exp(c, d, p);
}

cpp_int Player::fullDecrypt(cpp_int encrypted, const std::vector<cpp_int>& all_d) {
    cpp_int dec = encrypted;
    // Расшифровываем в обратном порядке (последний шифровал → первый расшифровывает)
    for (int i = static_cast<int>(all_d.size()) - 1; i >= 0; --i) {
        dec = mod_exp(dec, all_d[i], p);
    }
    return dec;
}