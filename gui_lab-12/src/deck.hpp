#pragma once
#include <vector>
#include <string>

class Deck {
public:
    std::vector<int> cards;

    Deck(int numCards = 52); // создаёт колоду
    void shuffle();           // перемешивает колоду
    int draw();               // взять верхнюю карту
    std::string cardToString(int card); // представление карты как строка
};
