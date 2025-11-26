#include "deck.hpp"
#include <algorithm>
#include <random>
#include <sstream>

Deck::Deck(int numCards) {
    cards.resize(numCards);
    for(int i=0; i<numCards; ++i)
        cards[i] = i;
}

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

int Deck::draw() {
    if(cards.empty()) return -1;
    int c = cards.back();
    cards.pop_back();
    return c;
}

std::string Deck::cardToString(int card) {
    static const char* suits[] = {"C","D","H","S"};
    static const char* ranks[] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
    int r = card % 13;
    int s = card / 13;
    std::stringstream ss;
    ss << ranks[r] << " of " << suits[s];
    return ss.str();
}
