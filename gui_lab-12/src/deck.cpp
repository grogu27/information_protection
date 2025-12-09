#include "deck.hpp"

const std::string CLUBS    = "C";
const std::string DIAMONDS = "D"; 
const std::string HEARTS   = "H";
const std::string SPADES   = "S";

std::vector<cpp_int> Deck::getInitialDeck() {
    std::vector<cpp_int> deck;
    deck.reserve(52);
    for (int i = 2; i <= 53; ++i) {
        deck.push_back(cpp_int(i));
    }
    return deck;
}

CardInfo Deck::getCardInfo(cpp_int card_val) {
    long long v = card_val.convert_to<long long>();
    if (v < 2 || v > 53) {
        return {"??", "?", sf::Color::Magenta, sf::Color(255, 200, 200)};
    }

    int idx = static_cast<int>(v) - 2;  // 0..51
    int rank_idx = idx % 13;
    int suit_idx = idx / 13;

    static const std::string RANKS[] = {
        "2","3","4","5","6","7","8","9","10","J","Q","K","A"
    };
    static const std::string SYMBOLS[] = {
        CLUBS, DIAMONDS, HEARTS, SPADES
    };
    static const sf::Color SUIT_COLORS[] = {
        sf::Color::Black,        // ♣
        sf::Color(200, 0, 0),    // ♦
        sf::Color(200, 0, 0),    // ♥
        sf::Color::Black         // ♠
    };
    static const sf::Color BG_COLORS[] = {
        sf::Color(30, 30, 30),   // тёмный фон — чёрные масти
        sf::Color(255, 240, 240),// светлый фон — красные масти
        sf::Color(255, 240, 240),
        sf::Color(30, 30, 30)
    };

    return {
        RANKS[rank_idx],
        SYMBOLS[suit_idx],
        SUIT_COLORS[suit_idx],
        BG_COLORS[suit_idx]
    };
}

std::string Deck::cardToString(cpp_int card) {
    auto ci = getCardInfo(card);
    return ci.rank + " " + ci.suitSymbol;
}