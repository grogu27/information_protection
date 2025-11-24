#pragma once
#include "deck.hpp"
#include "player.hpp"
#include <vector>

class Game {
public:
    int numPlayers;
    Deck deck;
    std::vector<Player> players;
    std::vector<int> community;

    Game(int nPlayers);
    void start();
private:
    void deal();
    void reveal();
};
