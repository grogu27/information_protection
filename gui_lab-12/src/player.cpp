#include "player.hpp"
#include <algorithm>
#include <random>

Player::Player(int playerId) : id(playerId) {}

void Player::addCard(int card) {
    hand.push_back(card);
}

void Player::shuffleDeck(std::vector<int>& deck) {
    std::random_device rd;
    std::mt19937 g(rd() + id);
    std::shuffle(deck.begin(), deck.end(), g);
}

int Player::decryptCard(int card) {
    // Пока просто возвращаем карту, можно добавить настоящую расшифровку
    return card;
}
