#include "deck.hpp"
#include <sstream>

std::string Deck::cardToString(cpp_int card) {
    if (card < 2 || card > 53) return "??";
    int idx = static_cast<int>(card.convert_to<long long>()) - 2;
    static const char* suits[] = {"C","D","H","S"};
    static const char* ranks[] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
    int r = idx % 13;
    int s = idx / 13;
    std::stringstream ss;
    ss << ranks[r] << " of " << suits[s];
    return ss.str();
}

std::vector<cpp_int> Deck::getInitialDeck() {
    std::vector<cpp_int> deck;
    deck.reserve(52);
    for (int i = 2; i <= 53; ++i) deck.push_back(cpp_int(i));
    return deck;
}
