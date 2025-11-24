#pragma once
#include <vector>

class Player {
public:
    int id;
    std::vector<int> hand;

    Player(int playerId);
    void addCard(int card);

    // Перемешивание колоды и секретность
    void shuffleDeck(std::vector<int>& deck);
    int decryptCard(int card); // для настоящей секретности
};
