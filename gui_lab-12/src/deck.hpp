#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <boost/multiprecision/cpp_int.hpp>

using cpp_int = boost::multiprecision::cpp_int;

struct CardInfo {
    std::string rank;        // "2", "J", "A", ...
    std::string suitSymbol;  // "♣", "♦", ...
    sf::Color suitColor;     // цвет символа (чёрный / красный)
    sf::Color bgColor;       // цвет фона карты
};

class Deck {
public:
    static std::vector<cpp_int> getInitialDeck();   // 52 карты: 2..53
    static CardInfo getCardInfo(cpp_int card);
    // Для совместимости — можно оставить, но лучше не использовать:
    static std::string cardToString(cpp_int card);
};