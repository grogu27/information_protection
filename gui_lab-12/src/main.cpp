#include "game.hpp"
#include <iostream>

int main() {
    int numPlayers;
    std::cout << "Enter number of players: ";
    std::cin >> numPlayers;

    if(numPlayers < 2 || numPlayers > 10) {
        std::cout << "Supported 2-10 players\n";
        return 1;
    }

    Game game(numPlayers);
    game.start();
    return 0;
}
