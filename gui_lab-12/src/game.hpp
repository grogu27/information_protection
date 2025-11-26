#pragma once
#include <vector>
#include "player.hpp"

class Game {
public:
    cpp_int p;
    std::vector<Player> players;
    std::vector<cpp_int> current_deck;
    std::vector<cpp_int> community_plain;

    Game(cpp_int prime, int nPlayers);
    void deal();
    void reveal();
    bool verify(); // проверяет корректность всей цепочки
};