#include <SFML/Graphics.hpp>
#include "game.hpp"
#include <iostream>

int main() {
    int numPlayers;
    std::cout << "Enter number of players (2-6): ";
    std::cin >> numPlayers;
    if(numPlayers < 2 || numPlayers > 6) return 1;

    Game game(numPlayers);
    game.start(); // раздача карт и формирование community

    sf::RenderWindow window(sf::VideoMode(1024, 768), "Mental Poker");
    sf::Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"); // стандартный шрифт

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(0,128,0)); // зеленый стол

        // рисуем карты игроков
        for(int i=0; i<numPlayers; ++i) {
            for(int j=0; j<2; ++j) {
                sf::RectangleShape card(sf::Vector2f(60,90));
                card.setFillColor(sf::Color::White);
                card.setOutlineColor(sf::Color::Black);
                card.setOutlineThickness(2);
                card.setPosition(50 + j*70, 50 + i*120);
                window.draw(card);

                sf::Text text(game.deck.cardToString(game.players[i].hand[j]), font, 12);
                text.setFillColor(sf::Color::Black);
                text.setPosition(55 + j*70, 55 + i*120);
                window.draw(text);
            }
        }

        // рисуем общие карты
        for(int i=0; i<5; ++i) {
            sf::RectangleShape card(sf::Vector2f(60,90));
            card.setFillColor(sf::Color::Yellow);
            card.setOutlineColor(sf::Color::Black);
            card.setOutlineThickness(2);
            card.setPosition(250 + i*70, 600);
            window.draw(card);

            sf::Text text(game.deck.cardToString(game.community[i]), font, 14);
            text.setFillColor(sf::Color::Black);
            text.setPosition(255 + i*70, 610);
            window.draw(text);
        }

        window.display();
    }

    return 0;
}
