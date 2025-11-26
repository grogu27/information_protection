#include "game.hpp"
#include "deck.hpp"
#include "utils.hpp"
#include <iostream>
#include <vector>

Game::Game(cpp_int prime, int nPlayers) : p(prime) {
    if (nPlayers < 2 || nPlayers > 6) throw std::invalid_argument("2–6 players");
    
    // Учебные ключи (взаимно простые с p-1)
    std::vector<cpp_int> secrets = {7, 9, 5, 11, 13, 17};
    for (int i = 0; i < nPlayers; ++i) {
        players.emplace_back(i + 1, p, secrets[i % secrets.size()]);
    }

    current_deck = Deck::getInitialDeck(); // [2..53]

    // Этап 1: каждый шифрует и перемешивает
    for (auto& pl : players) {
        current_deck = pl.encryptAndShuffle(current_deck);
    }
}

void Game::deal() {
    size_t idx = 0;

    // Каждому — по 2 карты (в порядке игроков)
    for (int round = 0; round < 2; ++round) {
        for (auto& pl : players) {
            if (idx < current_deck.size()) {
                pl.hand_enc.push_back(current_deck[idx++]);
            }
        }
    }

    // 5 общих карт
    std::vector<cpp_int> comm_enc;
    for (int i = 0; i < 5 && idx < current_deck.size(); ++i) {
        comm_enc.push_back(current_deck[idx++]);
    }

    // Собираем все d_i для расшифровки
    std::vector<cpp_int> all_d;
    for (auto& pl : players) all_d.push_back(pl.d);

    // Расшифровываем
    for (auto& pl : players) {
        for (auto& enc : pl.hand_enc) {
            pl.hand_plain.push_back(pl.fullDecrypt(enc, all_d));
        }
    }
    for (auto& enc : comm_enc) {
        community_plain.push_back(Player(0, p, 1).fullDecrypt(enc, all_d));
    }
}

void Game::reveal() {
    for (size_t i = 0; i < players.size(); ++i) {
        std::cout << "--- Player " << players[i].id << " ---\n";
        for (auto& c : players[i].hand_plain) {
            std::cout << Deck::cardToString(c) << " (" << c << ")\n";
        }
        std::cout << "\n";
    }

    std::cout << "--- Community ---\n";
    for (auto& c : community_plain) {
        std::cout << Deck::cardToString(c) << " (" << c << ")\n";
    }
}

bool Game::verify() {
    auto base = Deck::getInitialDeck();
    for (auto& m : base) {
        cpp_int enc = m;
        for (auto& pl : players) enc = mod_exp(enc, pl.e, p);
        cpp_int dec = enc;
        for (int i = static_cast<int>(players.size()) - 1; i >= 0; --i)
            dec = mod_exp(dec, players[i].d, p);
        if (dec != m) return false;
    }
    std::cout << "✅ Verification passed.\n";
    return true;
}