#include "game.hpp"
#include <iostream>

Game::Game(int nPlayers) : numPlayers(nPlayers), deck(52) {
    for(int i=0;i<numPlayers;++i)
        players.emplace_back(i+1);
}

void Game::start() {
    // Каждый игрок перемешивает колоду по очереди
    for(auto &p : players)
        p.shuffleDeck(deck.cards);

    deal();
    reveal();
}

void Game::deal() {
    for(int i=0;i<2;++i) {
        for(auto &p: players) {
            int c = deck.draw();
            if(c != -1) p.addCard(p.decryptCard(c));
        }
    }

    for(int i=0;i<5;++i) {
        int c = deck.draw();
        if(c != -1) community.push_back(c);
    }
}

void Game::reveal() {
    for(auto &p: players) {
        std::cout << "--- Player " << p.id << " cards ---\n";
        for(auto c: p.hand)
            std::cout << deck.cardToString(c) << "\n";
        std::cout << "\n";
    }

    std::cout << "--- Community cards ---\n";
    for(auto c: community)
        std::cout << deck.cardToString(c) << "\n";
    std::cout << std::endl;
}
